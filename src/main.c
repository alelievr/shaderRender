/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alelievr <alelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/07/11 18:11:03 by alelievr          #+#    #+#             */
/*   Updated: 2016/07/15 01:16:18 by alelievr         ###   ########.fr       */
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

#define BIT_SET(i, pos, v) (v) ? (i |= 1 << pos) : (i &= ~(1 << pos))
#define BIT_GET(i, pos) (i >> pos) & 1
#define MOVE_AMOUNT 0.01f;

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

enum			KEY_BITS
{
	RIGHT,
	LEFT,
	UP,
	DOWN,
	FORWARD,
	BACK,
	PLUS,
	MOIN,
};

vec4			mouse = {0, 0, 0, 0};
vec2			scroll = {0, 0};
vec4			move = {0, 0, 0, 0};
int				keys = 0;
int				input_pause = 0;
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
	if (key == GLFW_KEY_RIGHT || key == GLFW_KEY_D)
		BIT_SET(keys, RIGHT, action == GLFW_PRESS || action == GLFW_REPEAT);
	if (key == GLFW_KEY_LEFT || key == GLFW_KEY_A)
		BIT_SET(keys, LEFT, action == GLFW_PRESS || action == GLFW_REPEAT);
	if (key == GLFW_KEY_UP || key == GLFW_KEY_W)
		BIT_SET(keys, UP, action == GLFW_PRESS || action == GLFW_REPEAT);
	if (key == GLFW_KEY_DOWN || key == GLFW_KEY_S)
		BIT_SET(keys, DOWN, action == GLFW_PRESS || action == GLFW_REPEAT);
	if (key == GLFW_KEY_Q)
		BIT_SET(keys, FORWARD, action == GLFW_PRESS || action == GLFW_REPEAT);
	if (key == GLFW_KEY_E)
		BIT_SET(keys, BACK, action == GLFW_PRESS || action == GLFW_REPEAT);
	if (key == GLFW_KEY_KP_ADD)
		BIT_SET(keys, PLUS, action == GLFW_PRESS || action == GLFW_REPEAT);
	if (key == GLFW_KEY_KP_SUBTRACT)
		BIT_SET(keys, MOIN, action == GLFW_PRESS || action == GLFW_REPEAT);
	if (key == GLFW_KEY_SPACE)
		input_pause ^= action == GLFW_PRESS;
}

static void mouse_callback(GLFWwindow *win, double x, double y)
{
	(void)win;
	mouse.x = x;
	mouse.y = y;
}

static void mouse_click_callback(GLFWwindow *win, int button, int action, int mods)
{
	(void)win;
	(void)action;
	(void)mods;
	if (action == 1)
		mouse.y = 1;
	else
		mouse.y = 0;
}

static void mouse_scroll_callback(GLFWwindow *win, double xOffset, double yOffset)
{
	scroll.x += xOffset;
	scroll.y += yOffset;
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
	char	*ret = mmap(NULL, st.st_size + 1, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	if (ret == MAP_FAILED)
	{
		perror("mmap");
		return (NULL);
	}
	ret[st.st_size] = 0;
	return ret;
}

GLuint		CompileShaderFragment(int fd, bool fatal)
{
	GLuint		ret;
	const char	*srcs[] = {fragment_shader_text, getFileSource(fd)};

	if (srcs[1] == NULL)
		return (0);
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

	if (shaderVertex == 0 || shaderFragment == 0)
		return (0);
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
	glUniform4f(unis[2], mouse.x, WIN_H - mouse.y, mouse.y, mouse.y);
	glUniform2f(unis[3], scroll.x, scroll.y);
	glUniform4f(unis[4], move.x, move.y, move.z, move.w);
}

void		update_keys(void)
{
	if (BIT_GET(keys, RIGHT))
		move.x += MOVE_AMOUNT;
	if (BIT_GET(keys, LEFT))
		move.x -= MOVE_AMOUNT;
	if (BIT_GET(keys, UP))
		move.y += MOVE_AMOUNT;
	if (BIT_GET(keys, DOWN))
		move.y -= MOVE_AMOUNT;
	if (BIT_GET(keys, FORWARD))
		move.z += MOVE_AMOUNT;
	if (BIT_GET(keys, BACK))
		move.z -= MOVE_AMOUNT;
	if (BIT_GET(keys, PLUS))
		move.w += MOVE_AMOUNT;
	if (BIT_GET(keys, MOIN))
		move.w -= MOVE_AMOUNT;
}

void		loop(GLFWwindow *win, GLuint program, GLuint vao, GLint *unis)
{
	float ratio;
	int width, height;
	glfwGetFramebufferSize(win, &width, &height);
	ratio = width / (float) height;
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT);

	update_keys();

	if (!input_pause)
		updateUniforms(unis);

	//glEnable(GL_ARB_multisample);
	glEnable(GL_MULTISAMPLE);

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
	unis[3] = glGetUniformLocation(program, "iScrollAmount");
	unis[4] = glGetUniformLocation(program, "iMoveAmount");
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
		GLint new_program = createProgram(*fd, false);
		if (new_program != 0)
		{
			glDeleteProgram(*program);
			*program = new_program;
			printf("shader reloaded !\n");
			getUniformLocation(*program);
		}
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
