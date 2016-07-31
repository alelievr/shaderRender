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
	if (key == GLFW_KEY_KP_ADD || key == GLFW_KEY_EQUAL)
		BIT_SET(keys, PLUS, action == GLFW_PRESS || action == GLFW_REPEAT);
	if (key == GLFW_KEY_KP_SUBTRACT || key == GLFW_KEY_MINUS)
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

static void resize_callback(GLFWwindow *win, int width, int height)
{
	window.x = width;
	window.y = height;
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
	glfwSetWindowSizeCallback(win, resize_callback);
	glfwMakeContextCurrent (win);
	glfwSwapInterval(1);

	return (win);
}
