/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alelievr <alelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/09 12:36:31 by alelievr          #+#    #+#             */
/*   Updated: 2017/05/05 02:53:40 by alelievr         ###   ########.fr       */
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

	return (float)(time(NULL) - lTime) + (float)t.tv_usec / 1000000.0 - pausedTime;
}

void		updateUniformLocation(t_program *prog)
{
	const int	program = prog->id;

	prog->unis[0] = glGetUniformLocation(program, "iGlobalTime");
	prog->unis[1] = glGetUniformLocation(program, "iFrame");
	prog->unis[2] = glGetUniformLocation(program, "iMouse");
	prog->unis[3] = glGetUniformLocation(program, "iScrollAmount");
	prog->unis[4] = glGetUniformLocation(program, "iMoveAmount");
	prog->unis[5] = glGetUniformLocation(program, "iResolution");
	prog->unis[6] = glGetUniformLocation(program, "iFractalWindow");
	prog->unis[7] = glGetUniformLocation(program, "iForward");

	prog->unis[10] = glGetUniformLocation(program, "iChannel0");
	prog->unis[11] = glGetUniformLocation(program, "iChannel1");
	prog->unis[12] = glGetUniformLocation(program, "iChannel2");
	prog->unis[13] = glGetUniformLocation(program, "iChannel3");
	prog->unis[14] = glGetUniformLocation(program, "iChannel4");
	prog->unis[15] = glGetUniformLocation(program, "iChannel5");
	prog->unis[16] = glGetUniformLocation(program, "iChannel6");
	prog->unis[17] = glGetUniformLocation(program, "iChannel7");
}

