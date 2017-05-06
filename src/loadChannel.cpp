/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   loadChannel.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alelievr <alelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/09 11:53:14 by alelievr          #+#    #+#             */
/*   Updated: 2017/05/06 03:48:50 by alelievr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shaderpixel.h"
#include <string.h>

#define	IMAGE_EXT	(const char *[]){"jpg", "png", "tiff", "jpeg", "bmp", NULL}
#define	SHADER_EXT	(const char *[]){"glsl", "fs", NULL}
#define	SOUND_EXT	(const char *[]){"wav", "wave", NULL}

static int		staticProgramCounter = 0;

static void		loadImage(t_channel *chan, const char *file, int mode)
{
	int		flags = 0;

	if (mode & CHAN_NEAREST)
		flags |= SOIL_FLAG_NEAREST;
	else
		flags |= SOIL_FLAG_MIPMAPS;

	if (mode & CHAN_VFLIP)
		flags |= SOIL_FLAG_INVERT_Y;

	if (mode & CHAN_CLAMP)
		;
	else
		flags |= SOIL_FLAG_TEXTURE_REPEATS;

	flags |= SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT;

	if (chan->id)
		glDeleteTextures(1, (GLuint *)&chan->id);
	chan->id = SOIL_load_OGL_texture(file, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, flags);
	if (chan->id == 0)
		printf("can't load texture: %s\n", SOIL_last_result()), exit(-1);
	printf("loaded texture at id: %i\n", chan->id);
	chan->type = CHAN_IMAGE;
}

void			loadShader(t_program *prog, t_channel *chan, const char *file)
{
	chan->type = CHAN_SHADER;

	prog += staticProgramCounter++;
	if (createProgram(prog, chan->file_path, true, true))
		updateUniformLocation(prog);

	GLuint	rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo); 
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB8, framebuffer_size.x, framebuffer_size.y);

	GLuint	fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

	GLuint renderedTexture;
	glGenTextures(1, &renderedTexture);
	glBindTexture(GL_TEXTURE_2D, renderedTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, framebuffer_size.x, framebuffer_size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderedTexture, 0);

	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo);
	if (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("renderbuffer error: %i\n", glGetError());
		puts("fbo status error !"), exit(-1);
	}

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	printf("%p\n", prog);
	chan->id = fbo;
	printf("created renderTexture: %i\n", renderedTexture);
	prog->render_texture = renderedTexture;
	(void)file;
}

void			loadSound(t_channel *chan, const char *file, int mode)
{
	if (chan->id)
		glDeleteTextures(1, (GLuint *)&chan->id);

	(void)mode;
	chan->id = load_wav_file(file);
	chan->type = CHAN_SOUND;
}

void			loadChannel(t_program *prog, t_channel *chan, const char *file, int mode)
{
	GLuint		ret;	

	if (file != chan->file_path)
		strcpy(chan->file_path, file);
	if (checkFileExtention(file, IMAGE_EXT))
		return (loadImage(chan, file, mode));
	if (checkFileExtention(file, SHADER_EXT))
		return (loadShader(prog, chan, file));
	if (checkFileExtention(file, SOUND_EXT))
		return (loadSound(chan, file, mode));
}
