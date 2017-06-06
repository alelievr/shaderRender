#include <iostream>
#include <string>
#include <thread>
#include <getopt.h>
#include "ShaderApplication.hpp"
#include "NetworkManager.hpp"
#include "Timer.hpp"

static bool		networkMustQuit = false;
static bool		server = false;
static char *	shader = NULL;

static struct option longopts[] = {
    { "server",     no_argument,            NULL,           1},
    { "shader",     required_argument,      NULL,           's'},
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
    while ((ch = getopt_long(*ac, *av, "s:", longopts, NULL)) != -1)
        switch (ch) {
            case 1:
                server = true;
                break;
			case 's':
				shader = optarg;
				break ;
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
		nm.SetShaderSwitchCallback(
			[app](Timeval *timing, const std::string & shaderName)
			{
				app->LoadShader(shaderName);
				Timer::Timeout(timing,
					[app](void)
					{
						app->SwapShaderRender();
					}
				);
			}
		);

		while (!networkMustQuit)
			if (nm.Update() == NetworkStatus::Error)
				break ;
	}
	else
	{
		nm.ConnectCluster(E1);
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

	//TODO: remove this, only for testing
	if (shader != NULL)
	{
		app.LoadShader(shader);
		app.SwapShaderRender();
	}

	app.RenderLoop();

	networkMustQuit = true;
	networkThread.join();

	return 0;
}
