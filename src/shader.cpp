/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shader.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alelievr <alelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created  2016/07/21 00:13:40 by alelievr          #+#    #+#             */
/*   Updated  2016/07/21 00:17:52 by alelievr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shaderpixel.h"
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>

char		*strreplace(char *str, const char *find, const char *rep)
{
	while (*str)
	{
		if (!(str = strstr(str, find)))
			break ;
		memmove(str, rep, strlen(find));
	}
	return str;
}

GLuint		CompileShaderFragment(t_program *p, int fd, bool fatal, bool loadChannels)
{
}

GLuint		CompileShaderVertex(bool fatal)
{
}

bool		createProgram(t_program *prog, const char *file, bool fatal, bool loadChannels)
{
}
