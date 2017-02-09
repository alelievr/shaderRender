/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wav.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alelievr <alelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/07/30 16:57:22 by alelievr          #+#    #+#             */
/*   Updated: 2017/02/09 13:10:57 by alelievr         ###   ########.fr       */
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
	int		r = read(sounds[id].fd, buff, bit_per_frame *
			sounds[id].riff.bit_per_sample / 8 * sounds[id].riff.num_channels);
	return buff;
}

float		get_sample_value(char *data, int i, int bit_per_sample, int chan)
{
	float		value;

	if (bit_per_sample == 32)
		value = (*(float *)(data + (i * 4 * chan)) + 1) / 2;
	else if (bit_per_sample == 16)
		value = (float)(*(short *)(data + (i * 2 * chan)) + SHRT_MAX) / (float)SHRT_MAX;
	else
		value = (float)(*(data + (i * chan)) + 127) / 256;
	if (value > 1)
		value = 1;
	if (value < 0)
		value = 0;
	return value;
}

GLvoid		*raw_sound_to_data(char *buff, char *data, int size, int bit_per_sample, int chan)
{
	int		j = 0;

	for (int i = 0; i < size; i++)
	{
		float	f1 = get_sample_value(data, i, bit_per_sample, chan);
		float	f2 = get_sample_value(data, i + 1, bit_per_sample, chan);
		char	value = (f1 + f2) * 128;
		buff[j++] = value;
		buff[j++] = value;
		buff[j++] = value;
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
			sounds[id].riff.bit_per_sample, sounds[id].riff.num_channels);

	glBindTexture(GL_TEXTURE_2D, sounds[id].gl_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sounds[id].tex_length, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, d);
	return sounds[id].gl_id;
}

int			load_wav_file(const char *f)
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
	printf("channels: %i\n", header.num_channels);


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
