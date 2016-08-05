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
#define LOAD_SYM(x) x##1 = dlsym(fmod_handler, #x); if (!x) puts(dlerror()), exit(-1);
#define PROTO_SYM(x) typeof(x) *x##1

static FMOD_SYSTEM	*fmod_system;
static void			*fmod_handler;
PROTO_SYM(FMOD_System_Create);
PROTO_SYM(FMOD_System_Init);
PROTO_SYM(FMOD_System_CreateSound);
PROTO_SYM(FMOD_Sound_SetLoopCount);
PROTO_SYM(FMOD_System_PlaySound);
PROTO_SYM(FMOD_System_Close);
PROTO_SYM(FMOD_System_Release);

void			fmod_init(void)
{
	fmod_handler = dlopen(FMOD_LIB, RTLD_LAZY);
	if (!fmod_handler)
		puts(dlerror()), exit(-1);
	LOAD_SYM(FMOD_System_Create);
	LOAD_SYM(FMOD_System_Init);
	LOAD_SYM(FMOD_System_CreateSound);
	LOAD_SYM(FMOD_Sound_SetLoopCount);
	LOAD_SYM(FMOD_System_PlaySound);
	LOAD_SYM(FMOD_System_Close);
	LOAD_SYM(FMOD_System_Release);

	FMOD_System_Create1(&fmod_system);
	FMOD_System_Init1(fmod_system, 1, FMOD_INIT_NORMAL, NULL);
}

FMOD_SOUND			*load_sound(char *fname)
{
	FMOD_SOUND *music;

	if (FMOD_System_CreateSound1(fmod_system, fname, FMOD_2D | FMOD_CREATESTREAM, 0, &music) != FMOD_OK)
		printf("unable to load %s\n", fname), exit(-1);
	FMOD_Sound_SetLoopCount1(music, -1);
	return music;
}

void			play_sound(FMOD_SOUND *s)
{
	FMOD_System_PlaySound1(fmod_system, s, NULL, 0, NULL);
}

void			fmod_deinit(void)
{
	FMOD_System_Close1(fmod_system);
	FMOD_System_Release1(fmod_system);
}
