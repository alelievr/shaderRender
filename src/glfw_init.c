/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   glfw_init.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alelievr <alelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created  2016/07/21 00:12:11 by alelievr          #+#    #+#             */
/*   Updated  2016/07/21 17:56:22 by alelievr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shaderpixel.h"
#include <math.h>

static vec2 	angleAmount;
static int		cursor_mode;
static float	lastPausedTime;

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	struct timeval		t;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	if (key == GLFW_KEY_RIGHT || key == GLFW_KEY_D)
		BIT_SET(keys, RIGHT, action == GLFW_PRESS || action == GLFW_REPEAT);
	if (key == GLFW_KEY_LEFT || key == GLFW_KEY_A)
		BIT_SET(keys, LEFT, action == GLFW_PRESS || action == GLFW_REPEAT);
	if (key == GLFW_KEY_UP || key == GLFW_KEY_W)
		BIT_SET(keys, FORWARD, action == GLFW_PRESS || action == GLFW_REPEAT);
	if (key == GLFW_KEY_DOWN || key == GLFW_KEY_S)
		BIT_SET(keys, BACK, action == GLFW_PRESS || action == GLFW_REPEAT);
	if (key == GLFW_KEY_Q)
		BIT_SET(keys, UP, action == GLFW_PRESS || action == GLFW_REPEAT);
	if (key == GLFW_KEY_E)
		BIT_SET(keys, DOWN, action == GLFW_PRESS || action == GLFW_REPEAT);
	if (key == GLFW_KEY_KP_ADD || key == GLFW_KEY_EQUAL)
		BIT_SET(keys, PLUS, action == GLFW_PRESS || action == GLFW_REPEAT);
	if (key == GLFW_KEY_KP_SUBTRACT || key == GLFW_KEY_MINUS)
		BIT_SET(keys, MOIN, action == GLFW_PRESS || action == GLFW_REPEAT);
	if (key == GLFW_KEY_X)
		BIT_SET(keys, TIME_ADD, action == GLFW_PRESS || action == GLFW_REPEAT);
	if (key == GLFW_KEY_Z)
		BIT_SET(keys, TIME_SUB, action == GLFW_PRESS || action == GLFW_REPEAT);
	if (key == GLFW_KEY_SPACE && action != GLFW_REPEAT && action == GLFW_PRESS)
	{
		input_pause ^= action;
		if (input_pause)
			lastPausedTime = getCurrentTime();
		else
			pausedTime += getCurrentTime() - lastPausedTime;
	}
	if (key == GLFW_KEY_C)
		cursor_mode ^= action == GLFW_PRESS;
	if (key == GLFW_KEY_R)
	{
#if DOUBLE_PRECISION
		fractalWindow = (dvec4){-1, -1, 1, 1};
#else
		fractalWindow = (vec4){-1, -1, 1, 1};
#endif
		move = (vec4){0, 0, 0, 1};
		forward = (vec3){0, 0, 1};
		scroll = (vec2){0, 0};
	}

	glfwSetInputMode(window, GLFW_CURSOR, (cursor_mode) ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}

static void mouse_callback(GLFWwindow *win, double x, double y)
{
	(void)win;
	mouse.x = x;
	mouse.y = y;
	if (glfwGetInputMode(win, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
	{
    	angleAmount.x += ((window.x / 2.) - mouse.x) / 200;
    	angleAmount.y += ((window.y / 2.) - mouse.y) / 200;

		if (angleAmount.y > 1.5f)
			angleAmount.y = 1.5f;
		if (angleAmount.y < -1.5f)
			angleAmount.y = -1.5f;
    	forward.x = cos(angleAmount.y) * sin(angleAmount.x);
    	forward.y = sin(angleAmount.y);
    	forward.z = cos(angleAmount.y) * cos(angleAmount.x);
	}
}

static void mouse_click_callback(GLFWwindow *win, int button, int action, int mods)
{
	(void)win;
	(void)action;
	(void)mods;
	if (button == 0 && action == 1)
		mouse.z = 1;
	else if (button == 0)
		mouse.z = 0;
	if (button == 1 && action == 1)
		mouse.w = 1;
	else if (button == 1)
		mouse.w = 0;
}

static void mouse_scroll_callback(GLFWwindow *win, double xOffset, double yOffset)
{
	scroll.x += xOffset;
	scroll.y += yOffset;

	vec2 ratemin;
	vec2 ratemax;
	vec2 diff;

	vec2 fractal_mouse;
	fractal_mouse.x = mouse.x;
	fractal_mouse.y = window.y - mouse.y;
	ratemin.x = (fractal_mouse.x / window.x);
	ratemin.y = (fractal_mouse.y / window.y);
	ratemax.x = (fractal_mouse.x - window.x) / window.x;
	ratemax.y = (fractal_mouse.y - window.y) / window.y;
	diff.x = (fractalWindow.z - fractalWindow.x);
	diff.y = (fractalWindow.w - fractalWindow.y);
	fractalWindow.z += ratemax.x * diff.x * yOffset / 30;
	fractalWindow.w += ratemax.y * diff.y * yOffset / 30;
	fractalWindow.x += ratemin.x * diff.x * yOffset / 30;
	fractalWindow.y += ratemin.y * diff.y * yOffset / 30;
}

static void resize_callback(GLFWwindow *win, int width, int height)
{
	window.x = width;
	window.y = height;
}

GLFWwindow	*init(char *name)
{
	GLFWwindow *win;

	//GLFW
	glfwSetErrorCallback(error_callback);
	if (!glfwInit())
		printf("glfwInit error !\n"), exit(-1);
	glfwWindowHint (GLFW_SAMPLES, 4);
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
	glfwSetWindowSizeCallback(win, resize_callback);
	glfwMakeContextCurrent (win);
	glfwSwapInterval(1);
	glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	return (win);
}
