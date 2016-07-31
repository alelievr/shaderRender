/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wav.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alelievr <alelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/07/30 16:57:22 by alelievr          #+#    #+#             */
/*   Updated: 2016/07/30 23:00:14 by alelievr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shaderpixel.h"

int			load_wav_file(char *f)
{
	int			fd;
	int			ret;
	riff_header	header;

	if ((fd = open(f, O_RDONLY)) == -1)
		perror("open"), exit(-1);
	if ((ret = read(fd, &header, sizeof(riff_header))) == -1)
		perror("read"), exit(-1);
	printf("sample rate: %i\n", header->sample_rate);
	printf("file format: %c%c%c%c\n", header->format[0], header->format[1], header->format[2], header->format[3])
	printf("bit per sample: %i\n", header->bit_per_sample);
}
