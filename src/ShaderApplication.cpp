#include "ShaderApplication.hpp"
#include "LuaGL.hpp"

static ShaderRender		*renderShader;

ShaderApplication::ShaderApplication(void)
{
	glfwSetErrorCallback(
		[](int code, const char *description)
		{
			std::cout << "GLFW error: " << description << std::endl;
			exit(-1);
		}
	);

	//GLFW and shaderRender initializations
	if (!glfwInit())
		printf("glfwInit error !\n"), exit(-1);
	glfwWindowHint (GLFW_SAMPLES, 4);
	glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	if (!(window = glfwCreateWindow(WIN_W, WIN_H, "ShaderApp", NULL, NULL)))
		printf("failed to create window !\n"), exit(-1);

	glfwMakeContextCurrent (window);
	glfwSwapInterval(1);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	mShaderRender = new ShaderRender();
	renderShader = mShaderRender;

	glfwSetFramebufferSizeCallback(window,
		[](GLFWwindow *, int width, int height) {
			renderShader->framebufferSizeCallback(width, height);
		}
	);

	glfwSetWindowSizeCallback(window,
		[](GLFWwindow *, int w, int h) {
			renderShader->windowSizeCallback(w, h);
		}
	);

	glfwSetScrollCallback(window,
		[](GLFWwindow *, double x, double y) {
			renderShader->scrollCallback(x, y);
		}
	);

	glfwSetKeyCallback(window,
		[](GLFWwindow *win, int key, int scancode, int action, int mods) {
			renderShader->keyCallback(win, key, scancode, action, mods);
		}
	);

	glfwSetMouseButtonCallback(window,
		[](GLFWwindow *, int button, int action, int modifiers) {
			renderShader->clickCallback(button, action, modifiers);
		}
	);

	glfwSetCursorPosCallback(window,
		[](GLFWwindow *win, double x, double y) {
			renderShader->mousePositionCallback(win, x, y);
		}
	);

	int		fw, fh;
	glfwGetFramebufferSize(window, &fw, &fh);
	framebuffer_size.x = fw;
	framebuffer_size.y = fh;
}

bool	ShaderApplication::LoadShader(const std::string & shaderFile)
{
	return renderShader->attachShader(shaderFile);
}

void	ShaderApplication::RenderLoop(void)
{
	while (!glfwWindowShouldClose(window))
	{
		mShaderRender->render(window);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
}

ShaderApplication::~ShaderApplication()
{
	delete mShaderRender;
}
