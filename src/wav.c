/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wav.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alelievr <alelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/07/30 16:57:22 by alelievr          #+#    #+#             */
/*   Updated: 2016/08/05 21:46:17 by alelievr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shaderpixel.h"
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>

static t_sound		sounds[0xF0];

char		*getRawFrameData(int id)
{
	static char	buff[0xF000];
	int		bit_per_frame = sounds[id].riff.sample_rate / 60;

	if (bit_per_frame == 0)
		return NULL;
	int		r = read(sounds[id].fd, buff, bit_per_frame * sounds[id].riff.bit_per_sample / 8);
	return buff;
}

GLvoid		*raw_sound_to_data(char *buff, char *data, int size, int bit_per_sample)
{
	int		j = 0;

	for (int i = 0; i < size; i++)
	{
		int		value = 0;
		int		r = 1;
		if (bit_per_sample == 16)
		{
			value = *(short *)(data + i * 2);
			r = 256;
		}
		else
			value = *(data + i);
		buff[j++] = value / r;
		buff[j++] = value / r;
		buff[j++] = value / r;
		buff[j++] = 255;
	}
	return buff;
}

GLuint		get_sound_texture(int id)
{
	char	*data = getRawFrameData(id);

	if (data == NULL)
		return 0;
	GLvoid *d = raw_sound_to_data(sounds[id].image_buffer, data, sounds[id].tex_length,
			sounds[id].riff.bit_per_sample);

	glBindTexture(GL_TEXTURE_2D, sounds[id].gl_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sounds[id].tex_length, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, d);
	return sounds[id].gl_id;
}

int			load_wav_file(char *f)
{
	static int	id = 0;
	int			fd;
	int			ret;
	riff_header	header;

	if ((fd = open(f, O_RDONLY)) == -1)
		perror("open"), exit(-1);
	if ((ret = read(fd, &header, sizeof(riff_header))) == -1)
		perror("read"), exit(-1);
	++id;
	printf("sample rate: %i\n", header.sample_rate);
	printf("file format: %c%c%c%c\n", header.format[0], header.format[1], header.format[2], header.format[3]);
	printf("bit per sample: %i\n", header.bit_per_sample);


	GLuint		texId;

	int			imgWidth = header.sample_rate / 60;

	printf("imgWidth: %i\n", imgWidth);
	char		*baseData = malloc(sizeof(int) * imgWidth);
	memset(baseData, 127, sizeof(int) * imgWidth);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth,
			1, 0, GL_RGBA, GL_UNSIGNED_BYTE, baseData);

	FMOD_SOUND *s = load_sound(f);

	sounds[id] = (t_sound){id, fd, imgWidth, texId, baseData, s, WAVE, header};
	return id;
}

void		play_all_sounds(void)
{
	for (int i = 1; i < 0xF0; i++)
	{
		if (sounds[i].sound == NULL)
			break ;
		play_sound(sounds[i].sound);
	}
}
