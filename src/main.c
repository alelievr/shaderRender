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

#include "shaderpixel.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <string.h>

vec4        mouse = {0, 0, 0, 0};
vec2        scroll = {0, 0};
vec4        move = {0, 0, 0, 0};
vec2		window = {WIN_W, WIN_H};
#if DOUBLE_PRECISION
dvec4		fractalWindow = {-1, -1, 1, 1}; //xmin, ymin, xmax, ymax
#else
vec4		fractalWindow = {-1, -1, 1, 1}; //xmin, ymin, xmax, ymax
#endif
int        	keys = 0;
int         input_pause = 0;
long        lastModifiedFile = 0;

float points[] = {
   	-1.0f,  -1.0f,
    -1.0f, 1.0f,
	1.0f, 1.0f,
	1.0f, 1.0f,
	1.0f, -1.0f,
   	-1.0f,  -1.0f,
};

static void		usage(const char *n) __attribute__((noreturn));
static void		usage(const char *n)
{
	printf("usage: %s <shader>\n", n);
	exit(0);
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

	return vao;
}

void		updateUniforms(GLint *unis, GLint *images, int *sounds)
{
	struct timeval	t;
	static int		frames = 0;
	static time_t	lTime = 0;
	static int		glTextures[] = {GL_TEXTURE1, GL_TEXTURE2, GL_TEXTURE3, GL_TEXTURE4, GL_TEXTURE5, GL_TEXTURE6, GL_TEXTURE7, GL_TEXTURE8};

	if (lTime == 0)
		lTime = time(NULL);
	gettimeofday(&t, NULL);

	glUniform1f(unis[0], (float)(time(NULL) - lTime) + (float)t.tv_usec / 1000000.0);
	glUniform1i(unis[1], frames++);
	glUniform4f(unis[2], mouse.x, WIN_H - mouse.y, mouse.y, mouse.y);
	glUniform2f(unis[3], scroll.x, scroll.y);
	glUniform4f(unis[4], move.x, move.y, move.z, move.w);
	glUniform2f(unis[5], window.x, window.y);
#if DOUBLE_PRECISION
	glUniform4d(unis[6], fractalWindow.x, fractalWindow.y, fractalWindow.z, fractalWindow.w);
#else
	glUniform4f(unis[6], fractalWindow.x, fractalWindow.y, fractalWindow.z, fractalWindow.w);
#endif

	int		i = 0;
	for (int j = 0; j < 4; j++)
		if (images[j] != 0)
		{
			glActiveTexture(glTextures[i++]);
			glBindTexture(GL_TEXTURE_2D, images[j]);
		}
	GLuint			soundTex[4];

	soundTex[0] = get_sound_texture(sounds[0]);
	soundTex[1] = get_sound_texture(sounds[1]);
	soundTex[2] = get_sound_texture(sounds[2]);
	soundTex[3] = get_sound_texture(sounds[3]);

	for (int j = 0; j < 4; j++)
		if (sounds[j] != 0)
		{
			glActiveTexture(glTextures[i++]);
			glBindTexture(GL_TEXTURE_2D, soundTex[j]);
		}

	glUniform1i(unis[10], images[0]);
	glUniform1i(unis[11], images[1]);
	glUniform1i(unis[12], images[2]);
	glUniform1i(unis[13], images[3]);

	glUniform1i(unis[14], soundTex[0]);
	glUniform1i(unis[15], soundTex[1]);
	glUniform1i(unis[16], soundTex[2]);
	glUniform1i(unis[17], soundTex[3]);
}

void		update_keys(void)
{
	vec2	winSize;

	winSize.x = fractalWindow.z - fractalWindow.x;
	winSize.y = fractalWindow.w - fractalWindow.y;
	if (BIT_GET(keys, RIGHT))
	{
		move.x += MOVE_AMOUNT;
		fractalWindow.x += winSize.x / SCALE;
		fractalWindow.z += winSize.x / SCALE;
	}
	if (BIT_GET(keys, LEFT))
	{
		move.x -= MOVE_AMOUNT;
		fractalWindow.x -= winSize.x / SCALE;
		fractalWindow.z -= winSize.x / SCALE;
	}
	if (BIT_GET(keys, UP))
	{
		move.y += MOVE_AMOUNT;
		fractalWindow.y += winSize.y / SCALE;
		fractalWindow.w += winSize.y / SCALE;
	}
	if (BIT_GET(keys, DOWN))
	{
		move.y -= MOVE_AMOUNT;
		fractalWindow.y -= winSize.y / SCALE;
		fractalWindow.w -= winSize.y / SCALE;
	}
	if (BIT_GET(keys, FORWARD))
		move.z += MOVE_AMOUNT;
	if (BIT_GET(keys, BACK))
		move.z -= MOVE_AMOUNT;
	if (BIT_GET(keys, PLUS))
	{
		move.w += MOVE_AMOUNT;
		fractalWindow.z += -.5 * winSize.x / 25;
		fractalWindow.w += -.5 * winSize.y / 25;
		fractalWindow.x += .5 * winSize.x / 25;
		fractalWindow.y += .5 * winSize.y / 25;
	}
	if (BIT_GET(keys, MOIN))
	{
		move.w -= MOVE_AMOUNT;
		fractalWindow.z += -.5 * winSize.x / -25;
		fractalWindow.w += -.5 * winSize.y / -25;
		fractalWindow.x += .5 * winSize.x / -25;
		fractalWindow.y += .5 * winSize.y / -25;
	}
}

void		loop(GLFWwindow *win, GLuint program, GLuint vao, GLint *unis, GLint *images, int *sounds)
{
	float ratio;
	int width, height;
	glfwGetFramebufferSize(win, &width, &height);
	ratio = width / (float) height;
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT);

	update_keys();

	//glEnable(GL_ARB_multisample);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_TEXTURE_2D);

	if (!input_pause)
		updateUniforms(unis, images, sounds);

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
	unis[5] = glGetUniformLocation(program, "iResolution");
	unis[6] = glGetUniformLocation(program, "iFractalWindow");

	unis[10] = glGetUniformLocation(program, "iChannel0");
	unis[11] = glGetUniformLocation(program, "iChannel1");
	unis[12] = glGetUniformLocation(program, "iChannel2");
	unis[13] = glGetUniformLocation(program, "iChannel3");

	unis[14] = glGetUniformLocation(program, "iSoundChannel0");
	unis[15] = glGetUniformLocation(program, "iSoundChannel1");
	unis[16] = glGetUniformLocation(program, "iSoundChannel2");
	unis[17] = glGetUniformLocation(program, "iSoundChannel3");
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

int			checkFileExtention(char *file, char **exts)
{
	char	*ext = file + strlen(file) - 1;

	while (ext != file && *ext != '.')
		ext--;
	ext++;
	while (*exts)
	{
		if (!strcmp(ext, *exts))
			return (1);
		exts++;
	}
	return (0);
}

GLint		*loadImages(char **av)
{
	static GLint	texts[0xF0];
	int				k = 0;
	int				width;
	int				height;

	for (int i = 0; av[i]; i++)
	{
		if (!checkFileExtention(av[i], (char *[]){"png", "jpg", "tiff", "jpeg", NULL}))
			continue ;
		texts[k] = SOIL_load_OGL_texture(av[i], SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
				SOIL_FLAG_MIPMAPS | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT | SOIL_FLAG_TEXTURE_REPEATS);
		if (texts[k] == 0)
			printf("can't load texture: %s\n", SOIL_last_result()), exit(-1);
		k++;
	}
	return texts;
}

int			*loadSounds(char **av)
{
	static int		sounds[0xF0];
	int				k = 0;

	for (int i = 0; av[i]; i++)
	{
		if (!checkFileExtention(av[i], (char *[]){"wav", NULL}))
			continue ;
		sounds[k++] = load_wav_file(av[i]);
	}
	return sounds;
}

int			main(int ac, char **av)
{
	if (ac < 2)
		usage(*av);

	int			fd = getFileFd(av[1]);
	double		t1;
	int			frameDisplay = 0;

	GLFWwindow *win = init(av[1]);
	fmod_init();

	GLuint		program = createProgram(fd, true);
	GLuint		vbo = createVBO();
	GLuint		vao = createVAO(vbo, program);
	GLint		*unis = getUniformLocation(program);
	GLint		*images = loadImages(av + 2);
	int			*sounds = loadSounds(av + 2);

	play_all_sounds();
	while ((t1 = glfwGetTime()), !glfwWindowShouldClose(win))
	{
		checkFileChanged(&program, av[1], &fd);
		loop(win, program, vao, unis, images, sounds);
		if (frameDisplay == 10)
		{
			printf("%sfps:%.3f%s", "\x1b\x37", 1 / (glfwGetTime() - t1), "\x1b\x38");
			fflush(stdout);
			frameDisplay = 0;
		}
		frameDisplay++;
	}

	close(fd);
	glfwTerminate();
	return (0);
}
