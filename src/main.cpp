#include <iostream>
#include <string>
#include "ShaderApplication.hpp"

static void	usage(char *prog) __attribute__((noreturn));
static void	usage(char *prog)
{
	std::cout << "usage: " << prog << " <shader file>" << std::endl;
	exit(0);
}

int		main(int ac, char ** av)
{
	ShaderApplication	app;

	if (ac != 2)
		usage(av[0]);

	if (app.LoadShader(av[1]))
		app.RenderLoop();

	return 0;
}
