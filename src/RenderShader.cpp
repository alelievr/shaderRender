/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alelievr <alelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created  2016/07/11 18:11:03 by alelievr          #+#    #+#             */
/*   Updated  2016/07/25 18:53:04 by alelievr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RenderShader.hpp"

vec2		framebuffer_size = {0, 0};
float		pausedTime = 0;

RenderShader::RenderShader()
{
	fmod_init();

	angleAmount = vec2{0, 0};
	cursor_mode = 0;
	lastPausedTime = 0;
	programLoaded = false;
}

GLuint		RenderShader::createVBO(void)
{
	GLuint vbo = 0;
	glGenBuffers (1, &vbo);
	glBindBuffer (GL_ARRAY_BUFFER, vbo);
	glBufferData (GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
	return vbo;
}

GLuint		RenderShader::createVAO(GLuint vbo)
{
	GLint		fragPos;
	GLuint		vao = 0;

	glGenVertexArrays (1, &vao);
	glBindVertexArray (vao);
	glEnableVertexAttribArray (0);
	glBindBuffer (GL_ARRAY_BUFFER, vbo);

	fragPos = glGetAttribLocation(program->id, "fragPosition");
	glEnableVertexAttribArray(fragPos);
	glVertexAttribPointer(fragPos, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*) 0);

	return vao;
}

void		RenderShader::updateUniforms(GLint *unis, t_channel *channels)
{
	static int		frames = 0;

	float ti = getCurrentTime();
	if (!input_pause)
		glUniform1f(unis[0], ti);
	glUniform1i(unis[1], frames++);
	if (!input_pause)
		glUniform4f(unis[2], mouse.x, window.y - mouse.y, mouse.y, mouse.y);
	glUniform2f(unis[3], scroll.x, scroll.y);
	glUniform4f(unis[4], move.x, move.y, move.z, move.w);
	glUniform2f(unis[5], framebuffer_size.x, framebuffer_size.y);
	glUniform3f(unis[7], forward.x, forward.y, forward.z);
#if UNIFORM_DEBUG
	printf("window: %f/%f\n", framebuffer_size.x, framebuffer_size.y);
	printf("scroll: %f/%f\n", scroll.x, scroll.y);
	printf("move: %f/%f/%f/%f\n", move.x, move.y, move.z, move.w);
	printf("mouse: %f/%f/%f/%f\n", mouse.x, mouse.y, mouse.z, mouse.w);
	printf("time: %f\n", ti);
	printf("frames: %i\n", frames);
	printf("forward: %f/%f/%f\n", forward.x, forward.y, forward.z);
	if (!input_pause)
#endif
#if DOUBLE_PRECISION
		glUniform4d(unis[6], fractalWindow.x, fractalWindow.y, fractalWindow.z, fractalWindow.w);
#else
	glUniform4f(unis[6], fractalWindow.x, fractalWindow.y, fractalWindow.z, fractalWindow.w);
#endif

	int j = 0;
	for (int i = 0; channels[i].type; i++)
	{
		if (channels[i].type == CHAN_IMAGE)
		{
			glActiveTexture(GL_TEXTURE1 + j);
			glBindTexture(GL_TEXTURE_2D, channels[i].id);
			glUniform1i(unis[10 + j++], channels[i].id);
		}
		/*		if (channels[i].type == CHAN_SOUND)
				{
				int soundTexId = get_sound_texture(channels[i].id);
				glActiveTexture(GL_TEXTURE1 + i);
				glBindTexture(GL_TEXTURE_2D, soundTexId);
				glUniform1i(unis[10 + j++], soundTexId);
				}*/
	}
}

vec3        RenderShader::vec3Cross(vec3 v1, vec3 v2)
{
	return (vec3){v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x};
}

#define VEC3_ADD_DIV(v1, v2, f) { v1.x += v2.x / (f); v1.y += v2.y / (f); v1.z += v2.z / (f); }
void		RenderShader::updateKeys(void)
{
	vec2	winSize;

	vec3    right = vec3Cross(forward, vec3{0, 1, 0});
	vec3    up = vec3Cross(forward, right);

	winSize.x = fractalWindow.z - fractalWindow.x;
	winSize.y = fractalWindow.w - fractalWindow.y;
	if (BIT_GET(keys, RIGHT))
	{
		VEC3_ADD_DIV(move, right, 10 / move.w);
		fractalWindow.x += winSize.x / SCALE;
		fractalWindow.z += winSize.x / SCALE;
	}
	if (BIT_GET(keys, LEFT))
	{
		VEC3_ADD_DIV(move, -right, 10 / move.w);
		fractalWindow.x -= winSize.x / SCALE;
		fractalWindow.z -= winSize.x / SCALE;
	}
	if (BIT_GET(keys, UP))
		VEC3_ADD_DIV(move, up, 10 / move.w);
	if (BIT_GET(keys, DOWN))
		VEC3_ADD_DIV(move, -up, 10 / move.w);
	if (BIT_GET(keys, FORWARD))
	{
		VEC3_ADD_DIV(move, forward, 10 / move.w);
		fractalWindow.y += winSize.y / SCALE;
		fractalWindow.w += winSize.y / SCALE;
	}
	if (BIT_GET(keys, BACK))
	{
		VEC3_ADD_DIV(move, -forward, 10 / move.w);
		fractalWindow.y -= winSize.y / SCALE;
		fractalWindow.w -= winSize.y / SCALE;
	}
	if (BIT_GET(keys, PLUS))
	{
		move.w *= 1 + MOVE_AMOUNT;
		fractalWindow.z += -.5 * winSize.x / 25;
		fractalWindow.w += -.5 * winSize.y / 25;
		fractalWindow.x += .5 * winSize.x / 25;
		fractalWindow.y += .5 * winSize.y / 25;
	}
	if (BIT_GET(keys, MOIN))
	{
		move.w *= 1 - MOVE_AMOUNT;
		fractalWindow.z += -.5 * winSize.x / -25;
		fractalWindow.w += -.5 * winSize.y / -25;
		fractalWindow.x += .5 * winSize.x / -25;
		fractalWindow.y += .5 * winSize.y / -25;
	}
}

void		RenderShader::render(GLFWwindow *win)
{
	checkFileChanged(program);

	updateKeys();

	//process render buffers if used:
	for (int i = 1; i < 0xF0; i++)
	{
		if (program[i].id)
		{
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, program->channels[i].id);
			glBlitFramebuffer(0,0,framebuffer_size.x, framebuffer_size.y, 0, 0, 512,512, GL_COLOR_BUFFER_BIT, GL_LINEAR);
			GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
			glDrawBuffers(1, DrawBuffers);

		//	glViewport(0, 0, framebuffer_size.x, framebuffer_size.y);

			glClearColor(1, 0, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glUseProgram(program[i].id);

			updateUniforms(program[i].unis, program[i].channels);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, program[i].render_texture);

			glBindVertexArray(vao);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);

			uint8_t *data = (uint8_t *)malloc((int)window.x * (int)window.y * 4);
			typedef struct
			{
				int width;
				int height;
				uint8_t *data;
				size_t size;
			}	ppm_image;

			glReadBuffer(GL_COLOR_ATTACHMENT0);
			glReadPixels(0, 0, (int)window.x, (int)window.y, GL_RGB, GL_UNSIGNED_BYTE, data);

			int fd = open("f.ppm", O_WRONLY | O_CREAT, 0644);
			ppm_image img = (ppm_image){(int)window.x, (int)window.y, data, static_cast< size_t >((int)window.x * (int)window.y * 3)};

			dprintf(fd, "P6\n# THIS IS A COMMENT\n%d %d\n%d\n", 
					img.width, img.height, 0xFF);
			write(fd, img.data, img.width * img.height * 3);
		}
		else
			break ;
	}

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

//	glViewport(0, 0, framebuffer_size.x, framebuffer_size.y);
	glClearColor(1, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(program->id);

	updateUniforms(program->unis, program->channels);

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);

	if (glfwGetInputMode(win, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
		glfwSetCursorPos(win, window.x / 2, window.y / 2);
}

void		RenderShader::checkFileChanged(t_program *progs)
{
	struct stat		st;

	for (int i = 0; progs[i].id; i++)
	{
		stat(progs[i].program_path, &st);
		if (progs[i].last_modified != st.st_mtime)
		{
			progs[i].last_modified = st.st_mtime;
			close(progs[i].fd);
			progs[i].fd = open(progs[i].program_path, O_RDONLY);
			if (createProgram(progs + i, progs[i].program_path, false, true))
				updateUniformLocation(progs + i);
		}
	}
}

void		RenderShader::displayWindowFps(void)
{
	static int		frames = 0;
	static double	last_time = 0;
	double			current_time = glfwGetTime();

	frames++;
	if (current_time - last_time >= 1.0)
	{
		printf("%sfps:%.3f%s", "\x1b\x37", 1.0 / (1000.0 / (float)frames) * 1000.0, "\x1b\x38");
		frames = 0;
		fflush(stdout);
		last_time++;
	}
}

void		RenderShader::loadShaderFile(char *file)
{
	bzero(program, sizeof(program));
	if (createProgram(program + 0, file, true, true))
		programLoaded = true;
	updateUniformLocation(program + 0);
	play_all_sounds();
	GLuint vbo = createVBO();
	vao = createVAO(vbo);
}

void		RenderShader::windowSizeCallback(int winX, int winY)
{
	window.x = winX;
	window.y = winY;
}

void		RenderShader::scrollCallback(double xOffset, double yOffset)
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

void		RenderShader::clickCallback(int button, int action, int mods)
{
	if (button == 0 && action == 1)
		mouse.z = 1;
	else if (button == 0)
		mouse.z = 0;
	if (button == 1 && action == 1)
		mouse.w = 1;
	else if (button == 1)
		mouse.w = 0;
}

void		RenderShader::framebufferSizeCallback(int width, int height)
{
	framebuffer_size.x = width;
	framebuffer_size.y = height;
}

void		RenderShader::mousePositionCallback(GLFWwindow *win, double x, double y)
{
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

void		RenderShader::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
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

t_channel	*RenderShader::getChannel(int chan)
{
	if (chan < 10 && programLoaded)
	{
		return &program[0].channels[chan];
	}
	return NULL;
}

void		RenderShader::updateChannel(t_channel *chan, const char *file, int mode)
{
	loadChannel(program, chan, file, mode);
}

RenderShader::~RenderShader() {}
