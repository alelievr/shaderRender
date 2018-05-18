/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alelievr <alelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/09 12:36:31 by alelievr          #+#    #+#             */
/*   Updated: 2017/09/17 00:18:18 by alelievr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shaderpixel.h"
#include <string.h>
#include <time.h>
#include <sys/time.h>

bool		checkFileExtention(const char *file, const char **exts)
{
	const char	*ext = file + strlen(file) - 1;

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

float		getCurrentTime(void)
{
	struct timeval	t;
	static time_t	lTime = 0;
	
	if (lTime == 0)
		lTime = time(NULL);
	gettimeofday(&t, NULL);

	return (float)(time(NULL) - lTime) + (float)t.tv_usec / 1000000.0 - pausedTime + added_time;
}
