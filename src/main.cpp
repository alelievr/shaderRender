#include <iostream>
#include <string>
#include <thread>
#include <list>
#include <getopt.h>
#include "ShaderApplication.hpp"
#include "NetworkManager.hpp"
#include "Timer.hpp"

static bool		networkMustQuit = false;
static bool		server = false;
static bool		serverSentAllShadersToLoad;
static std::list< const std::string >	shadersToLoad;

static struct option longopts[] = {
	{ "server",     no_argument,            NULL,           1},
//	{ "shader",     required_argument,      NULL,           's'},
	{ NULL,         0,                      NULL,           0}
};

static void	usage(char *prog) __attribute__((noreturn));
static void	usage(char *prog)
{
	std::cout << "usage: " << prog << " <shader file>" << std::endl;
	exit(0);
}

static void options(int *ac, char ***av)
{
	int bflag, ch;

    bflag = 0;
    while ((ch = getopt_long(*ac, *av, "", longopts, NULL)) != -1)
        switch (ch) {
            case 1:
                server = true;
                break;
            default:
                usage(*av[0]);
     	}
    *ac -= optind;
    *av += optind;
}

static void NetworkThread(bool server, ShaderApplication *app)
{
	NetworkManager		nm(server);

	if (!server)
	{
		nm.SetShaderFocusCallback(
			[app](const Timeval *timing, const int programIndex)
			{
				if (app == NULL)
				{
					std::cout << "Can't focus a shader, shaderApplication not initialized" << std::endl;
					return ;
				}
				std::cout << "request shader focus on " << programIndex << std::endl;
				Timer::Timeout(timing,
					[programIndex, app](void)
					{
						std::cout << "focusing shader: " << programIndex << std::endl;
						app->FocusShader(programIndex);
					}
				);
			}
		);

		nm.SetShaderLoadCallback(
			[app](const std::string & shaderName, const bool last)
			{
				shadersToLoad.push_back(shaderName);
			
				if (last)
					serverSentAllShadersToLoad = true;
			}
		);

		while (!networkMustQuit)
			if (nm.Update() == NetworkStatus::Error)
				break ;
	}
	else
	{
		nm.ConnectCluster(E3);

		nm.LoadShaderOnGroup(0, "shaders/fractal/kifs.glsl");
		nm.LoadShaderOnGroup(0, "shaders/fractal/mandelbrot-orbit.glsl", true);

		int		group = 0;
		//int		group = nm.CreateNewGroup();

		//nm.MoveIMacToGroup(group, 9, 4, 3);

		std::cout << "focus shader 0 send !" << std::endl;
		nm.FocusShaderOnGroup(Timer::Now(), group, 0);
		nm.FocusShaderOnGroup(Timer::TimeoutInSeconds(10), group, 1);

		while (!networkMustQuit)
			if (nm.Update() == NetworkStatus::Error)
				break ;
		nm.CloseAllConnections();
	}
}

int		main(int ac, char **av)
{
	options(&ac, &av);

	ShaderApplication	app(server);
	std::thread			networkThread(NetworkThread, server, &app);

	while (!serverSentAllShadersToLoad)
		usleep(16000);

	app.loadingShaders = true;
	for (const std::string & shaderName : shadersToLoad)
		app.LoadShader(shaderName);
	app.loadingShaders = false;
	app.OnLoadingShaderFinished();

	app.RenderLoop();

	networkMustQuit = true;
	networkThread.join();

	return 0;
}
