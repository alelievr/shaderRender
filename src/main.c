/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alelievr <alelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/07/11 18:11:03 by alelievr          #+#    #+#             */
/*   Updated: 2016/07/12 14:55:29 by alelievr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shaderpixel.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdbool.h>

float points[] = {
   	-1.0f,  -1.0f,
    -1.0f, 1.0f,
	1.0f, 1.0f,
	1.0f, 1.0f,
	1.0f, -1.0f,
   	-1.0f,  -1.0f,
	WIN_W, WIN_H,
	WIN_W, WIN_H,
	WIN_W, WIN_H,
	WIN_W, WIN_H,
	WIN_W, WIN_H,
	WIN_W, WIN_H,
};

float			mouseX = 0, mouseY = 0, buttonClick = 0, scrollX = 0, scrollY = 0;
long			lastModifiedFile = 0;

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
	(void)win;
	mouseX = x;
	mouseY = y;
}

static void mouse_click_callback(GLFWwindow *win, int button, int action, int mods)
{
	(void)win;
	(void)action;
	(void)mods;
	if (action == 1)
		buttonClick = 1;
	else
		buttonClick = 0;
}

static void mouse_scroll_callback(GLFWwindow *win, double xOffset, double yOffset)
{
	scrollX = xOffset;
	scrollY = yOffset;
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
	glfwSetMouseButtonCallback(win, mouse_click_callback);
	glfwSetScrollCallback(win, mouse_scroll_callback);
	glfwMakeContextCurrent (win);
	glfwSwapInterval(1);

	return (win);
}

void		checkCompilation(GLuint shader, bool fatal)
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
		if (fatal)
			printf("shader compilation error, exiting\n"), exit(-1);
	}
}

void		checkLink(GLuint program, bool fatal)
{
	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
	if(isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		char		buff[maxLength];
		glGetProgramInfoLog(program, maxLength, &maxLength, buff);
		printf("%s\n", buff);

		glDeleteProgram(program);
		if (fatal)
			printf("link error, exiting\n"), exit(-1);
	}
}

char		*getFileSource(int fd)
{
	struct stat	st;
	fstat(fd, &st);
	return mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
}

GLuint		CompileShaderFragment(int fd, bool fatal)
{
	GLuint		ret;
	const char	*srcs[] = {fragment_shader_text, getFileSource(fd)};

	ret = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(ret, 2, srcs, NULL);
	glCompileShader(ret);
	checkCompilation(ret, fatal);
	return (ret);
}

GLuint		CompileShaderVertex(bool fatal)
{
	GLuint		ret;

	ret = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(ret, 1, &vertex_shader_text, NULL);
	glCompileShader(ret);
	checkCompilation(ret, fatal);
	return (ret);
}

GLuint		createProgram(int fd, bool fatal)
{
	GLuint		program;

	GLuint		shaderVertex = CompileShaderVertex(fatal);
	GLuint		shaderFragment = CompileShaderFragment(fd, fatal);

	program = glCreateProgram();
	glAttachShader(program, shaderVertex);
	glAttachShader(program, shaderFragment);
	glLinkProgram(program);
	checkLink(program, fatal);

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

GLuint		createVAO(GLuint vbo, int program)
{
	GLint		fragPos;
	GLint		resPos;
	GLuint		vao = 0;

	glGenVertexArrays (1, &vao);
	glBindVertexArray (vao);
	glEnableVertexAttribArray (0);
	glBindBuffer (GL_ARRAY_BUFFER, vbo);

	fragPos = glGetAttribLocation(program, "fragPosition");
	glEnableVertexAttribArray(fragPos);
	glVertexAttribPointer(fragPos, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*) 0);

	resPos = glGetAttribLocation(program, "iResolutionIn");
	glEnableVertexAttribArray(resPos);
	glVertexAttribPointer(resPos, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)(sizeof(float) * 12));
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
	glUniform4f(unis[2], mouseX, WIN_H - mouseY, buttonClick, buttonClick);
	glUniform2f(unis[3], scrollX, scrollY);
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
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);

	glfwSwapBuffers(win);
	glfwPollEvents();
}

GLint		*getUniformLocation(GLuint program)
{
	static GLint unis[0xF0];

	unis[0] = glGetUniformLocation(program, "iGlobalTime");
	unis[1] = glGetUniformLocation(program, "iFrame");
	unis[2] = glGetUniformLocation(program, "iMouse");
	unis[3] = glGetUniformLocation(program, "iScroll");
	return unis;
}

int			getFileFd(const char *fname)
{
	struct stat	st;
	int		fd = open(fname, O_RDONLY);
	fstat(fd, &st);
	lastModifiedFile = st.st_mtime;
	if (fd == -1 || !S_ISREG(st.st_mode))
		printf("not a valid file: %s\n", fname), exit(-1);
	return fd;
}

void		checkFileChanged(GLuint *program, char *file, int *fd)
{
	struct stat		st;
	stat(file, &st);

	if (lastModifiedFile != st.st_mtime)
	{
		lastModifiedFile = st.st_mtime;
		close(*fd);
		*fd = open(file, O_RDONLY);
		*program = createProgram(*fd, false);
		printf("recompiling shader !\n");
	}
}

int			main(int ac, char **av)
{
	if (ac != 2)
		usage(*av);

	int			fd = getFileFd(av[1]);

	GLFWwindow *win = init(av[1]);

	GLuint		program = createProgram(fd, true);
	GLuint		vbo = createVBO();
	GLuint		vao = createVAO(vbo, program);
	GLint		*unis = getUniformLocation(program);

	while (!glfwWindowShouldClose(win))
	{
		checkFileChanged(&program, av[1], &fd);
		loop(win, program, vao, unis);
	}

	close(fd);
	glfwTerminate();
	return (0);
}
