/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   loadChannel.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alelievr <alelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/09 11:53:14 by alelievr          #+#    #+#             */
/*   Updated: 2017/02/11 00:56:33 by alelievr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shaderpixel.h"
#include <string.h>

#define	IMAGE_EXT	(const char *[]){"jpg", "png", "tiff", "jpeg", "bmp", NULL}
#define	SHADER_EXT	(const char *[]){"glsl", "fs", NULL}
#define	SOUND_EXT	(const char *[]){"wav", "wave", NULL}

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

//	if (chan->id)
//		glDeleteTextures(1, (GLuint *)&chan->id);
	chan->id = SOIL_load_OGL_texture(file, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, flags);
	if (chan->id == 0)
		printf("can't load texture: %s\n", SOIL_last_result()), exit(-1);
	printf("loaded texture at id: %i\n", chan->id);
	chan->type = CHAN_IMAGE;
}

void			loadShader(t_channel *chan, const char *file)
{
	GLuint FramebufferName = 0;

	chan->type = CHAN_SHADER;
	glGenFramebuffers(1, &FramebufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

	GLuint renderedTexture;
	glGenTextures(1, &renderedTexture);

	glBindTexture(GL_TEXTURE_2D, renderedTexture);

	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, 1024, 768, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

	chan->id = FramebufferName;
	(void)file;
}

void			loadSound(t_channel *chan, const char *file, int mode)
{
	(void)mode;
	chan->id = load_wav_file(file);
	chan->type = CHAN_SOUND;
}

void			loadChannel(t_channel *chan, const char *file, int mode)
{
	GLuint		ret;	

	if (file != chan->file_path)
		strcpy(chan->file_path, file);
	if (checkFileExtention(file, IMAGE_EXT))
		return (loadImage(chan, file, mode));
	if (checkFileExtention(file, SHADER_EXT))
		return (loadShader(chan, file));
	if (checkFileExtention(file, SOUND_EXT))
		return (loadSound(chan, file, mode));
}

t_channel		*loadChannels(char **files)
{
	static t_channel	channs[0xF00];
	int					i = -1;

	while (files[++i])
	{
		bzero(channs + i, sizeof(t_channel));
		loadChannel(channs + i, files[i], 0);
	}
	return channs;
}
