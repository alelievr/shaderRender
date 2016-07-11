/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alelievr <alelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/07/11 18:11:03 by alelievr          #+#    #+#             */
/*   Updated: 2016/07/11 21:50:03 by alelievr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shaderpixel.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

float points[] = {
   	-1.0f,  -1.0f,
    -1.0f, 1.0f,
	1.0f, 1.0f,
	1.0f, -1.0f,
	WIN_W, WIN_H,
};

float			mouseX = 0, mouseY = 0;

static void		usage(const char *n) __attribute__((noreturn));
static void		usage(const char *n)
{
	printf("usage: %s <shader>\n", n);
	exit(0);
}

static void error_callback(int error, const char* description)
{
	    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static void mouse_callback(GLFWwindow *win, double x, double y)
{
	mouseX = x;
	mouseY = y;
}

GLFWwindow	*init(char *name)
{
	GLFWwindow *win;
	GLuint		vertex_buffer;

	//GLFW
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
		printf("glfwInit error !\n"), exit(-1);
	glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
 	glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 2);
 	glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
 	glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	if (!(win = glfwCreateWindow(WIN_W, WIN_H, name, NULL, NULL)))
		printf("failed to create window !\n"), exit(-1);
	glfwSetKeyCallback(win, key_callback);
	glfwSetCursorPosCallback(win, mouse_callback);
	glfwMakeContextCurrent (win);
	glfwSwapInterval(1);

	return (win);
}

void		checkCompilation(GLuint shader)
{
	GLint isCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if(isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		char		buff[maxLength];
		glGetShaderInfoLog(shader, maxLength, &maxLength, buff);
		printf("%s\n", buff);

		glDeleteShader(shader);
		printf("shader compilation error, exiting\n");
		exit(-1);
	}
}

void		checkLink(GLuint program)
{
	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
	if(isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		char		buff[maxLength];
		glGetProgramInfoLog(program, maxLength, &maxLength, buff);

		glDeleteProgram(program);
		printf("link error, exiting\n");
		exit(-1);
	}
}

GLuint		CompileShaderFragment(void)
{
	GLuint		ret;

	ret = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(ret, 1, &fragment_shader_text, NULL);
	glCompileShader(ret);
	checkCompilation(ret);
	return (ret);
}

GLuint		CompileShaderVertex(char *fname)
{
	(void)fname;
	GLuint		ret;
	const char	*srcs[] = {vertex_shader_text, vertex_shader_image_text};

	ret = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(ret, 2, srcs, NULL);
	glCompileShader(ret);
	checkCompilation(ret);
	return (ret);
}

GLuint		createProgram(char *fname)
{
	GLuint		program;
	GLint		fragPos;
	GLint		resPos;
	(void)fname;

	GLuint		shaderVertex = CompileShaderVertex(fname);
	GLuint		shaderFragment = CompileShaderFragment();

	program = glCreateProgram();
	glAttachShader(program, shaderVertex);
	glAttachShader(program, shaderFragment);
	glLinkProgram(program);
	checkLink(program);

	fragPos = glGetAttribLocation(program, "fragPosition");
	glEnableVertexAttribArray(fragPos);
	glVertexAttribPointer(fragPos, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*) 0);

	fragPos = glGetAttribLocation(program, "iResolution");
	glEnableVertexAttribArray(fragPos);
	glVertexAttribPointer(fragPos, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)(sizeof(float) * 8));

	return program;
}

GLuint		createVBO(void)
{
	GLuint vbo = 0;
	glGenBuffers (1, &vbo);
	glBindBuffer (GL_ARRAY_BUFFER, vbo);
	glBufferData (GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
	return vbo;
}

GLuint		createVAO(GLuint vbo)
{
	GLuint vao = 0;
	glGenVertexArrays (1, &vao);
	glBindVertexArray (vao);
	glEnableVertexAttribArray (0);
	glBindBuffer (GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer (0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	return vao;
}

void		updateUniforms(GLint *unis)
{
	struct timeval	t;
	static int		frames = 0;
	static time_t	lTime = 0;

	if (lTime == 0)
		lTime = time(NULL);
	gettimeofday(&t, NULL);

	glUniform1f(unis[0], (float)(time(NULL) - lTime) + (float)t.tv_usec / 1000000.0);
	glUniform1i(unis[1], frames++);
	glUniform2f(unis[2], mouseX, mouseY);
}

void		loop(GLFWwindow *win, GLuint program, GLuint vao, GLint *unis)
{
	float ratio;
	int width, height;
	glfwGetFramebufferSize(win, &width, &height);
	ratio = width / (float) height;
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT);

	updateUniforms(unis);

	glUseProgram(program);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glfwSwapBuffers(win);
	glfwPollEvents();
}

GLint		*getUniformLocation(GLuint program)
{
	static GLint unis[0xF0];

	unis[0] = glGetUniformLocation(program, "iGlobalTime");
	unis[1] = glGetUniformLocation(program, "iFrame");
	unis[2] = glGetUniformLocation(program, "iMouse");
	return unis;
}

int			main(int ac, char **av)
{
	if (ac != 2)
		usage(*av);

	GLFWwindow *win = init(av[1]);

	GLuint		program = createProgram(av[1]);
	GLuint		vbo = createVBO();
	GLuint		vao = createVAO(vbo);
	GLint		*unis = getUniformLocation(program);

	while (!glfwWindowShouldClose(win))
		loop(win, program, vao, unis);

	glfwTerminate();
	return (0);
}
