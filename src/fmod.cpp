/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fmod.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alelievr <alelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created  2016/07/31 20:44:24 by alelievr          #+#    #+#             */
/*   Updated  2016/07/31 20:44:24 by alelievr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shaderpixel.h"
#include <dlfcn.h>
#include "fmod.h"

static FMOD_SYSTEM	*fmod_system;

void			fmod_init(void)
{
	FMOD_System_Create(&fmod_system);
	FMOD_System_Init(fmod_system, 1, FMOD_INIT_NORMAL, NULL);
}

FMOD_SOUND			*load_sound(const char *fname)
{
	FMOD_SOUND *music;

	if (FMOD_System_CreateSound(fmod_system, fname, FMOD_2D | FMOD_CREATESTREAM, 0, &music) != FMOD_OK)
		printf("unable to load %s\n", fname), exit(-1);
	FMOD_Sound_SetLoopCount(music, -1);
	return music;
}

void			play_sound(FMOD_SOUND *s)
{
	FMOD_System_PlaySound(fmod_system, s, NULL, 0, NULL);
}

void			fmod_deinit(void)
{
	FMOD_System_Close(fmod_system);
	FMOD_System_Release(fmod_system);
}
