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
				Timer::Timeout(timing,
					[app, programIndex](void)
					{
						app->FocusShader(programIndex);
					}
				);
			}
		);

		nm.SetShaderLoadCallback(
			[](const std::string & shaderName, const bool last)
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
		nm.ConnectCluster(E1);

		nm.LoadShaderOnGroup(0, "shaders/fractal/kifs.glsl");
		nm.LoadShaderOnGroup(0, "shaders/fractal/mandelbrot-orbit.glsl", true);

		nm.FocusShaderOnGroup(Timer::Now(), 0, 0);
		nm.FocusShaderOnGroup(Timer::TimeoutInSeconds(10), 0, 1);

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

	for (const std::string & shaderName : shadersToLoad)
		app.LoadShader(shaderName);

	app.RenderLoop();

	networkMustQuit = true;
	networkThread.join();

	return 0;
}
