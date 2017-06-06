/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shaderpixel.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alelievr <alelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created  2016/07/11 18:11:58 by alelievr          #+#    #+#             */
/*   Updated  2016/07/25 19:04:51 by alelievr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHADERPIXEL_H
# define SHADERPIXEL_H

# include <stdio.h>
# include <stdlib.h>
# include <stdbool.h>
# include <iostream>
# define GL_GLEXT_PROTOTYPES
# define GLFW_INCLUDE_GLCOREARB
# include "GLFW/glfw3.h"
# include "SOIL2.h"
# include "fmod.h"

# if __APPLE__
#  define FMOD_LIB "fmod/lib/libfmod.dylib"
#  include <OpenGL/glext.h>
# else
#  define FMOD_LIB "fmod/lib/libfmod-linux.so.8.8"
#  include <bsd/string.h>
# endif

# define unused __attribute__((unused))

# define MAX_CHANNEL_COUNT	8
# define WIN_W				1920
# define WIN_H				1080
# define SCALE				70
//# define DOUBLE_PRECISION 1

typedef struct timeval			Timeval;

typedef struct s_vec2
{
	float x;
	float y;
}				vec2;

typedef struct s_vec3
{
	float x;
	float y;
	float z;
}				vec3;

typedef struct s_vec4
{
	float x;
	float y;
	float z;
	float w;
}				vec4;

typedef struct s_dvec4
{
	double x;
	double y;
	double z;
	double w;
}				dvec4;

typedef struct	s_riff_header
{
	int			:32;				//0
	int			chunk_size;			//4
	char		format[4];			//8
	int			:32;				//12
	int			sub_chunk1_size;	//16
	short		audio_format;		//20
	short		num_channels;		//22
	int			sample_rate;		//24
	int			byte_rate;			//28
	short		block_align;		//32
	short		bit_per_sample;		//34
}				riff_header;

enum			e_channel_type
{
	CHAN_NONE,
	CHAN_IMAGE,
	CHAN_SOUND,
	CHAN_SHADER,
};

enum			e_channel_mode
{
	CHAN_NEAREST			= 0x01,
	CHAN_LINEAR				= 0x02,
	CHAN_MIPMAP				= 0x04,
	CHAN_VFLIP				= 0x08,
//	CHAN_CLAMP_BORDER		= 0x10,
//	CHAN_CLAMP_EDGE			= 0x20,
	CHAN_CLAMP				= 0x40,
};

typedef struct	s_channel
{
	GLint		id;
	int			type;
	char		file_path[1024];
}				t_channel;

typedef struct	s_program
{
	int			id;
	t_channel	channels[8];
	char		program_path[1024];
	int			fd;
	long		last_modified;
	GLint		unis[0xF0];
	GLuint		render_texture;
}				t_program;

enum			KEY_BITS
{
	RIGHT,
	LEFT,
	UP,
	DOWN,
	FORWARD,
	BACK,
	PLUS,
	MOIN,
};

enum			SOUND_FORMAT
{
	WAVE,
};

typedef struct	s_sound
{
	int				id;
	int				fd;
	int				tex_length;
	GLint			gl_id;
	char			*image_buffer;
	FMOD_SOUND		*sound;
	enum SOUND_FORMAT	sound_format;
	riff_header	riff;
}				t_sound;

#define BIT_SET(i, pos, v) (v) ? (i |= 1 << pos) : (i &= ~(1 << pos))
#define BIT_GET(i, pos) (i >> pos) & 1
#define MOVE_AMOUNT 0.01f;

extern vec4			mouse;
extern vec2			scroll;
extern vec4			move;
extern vec2			window;
extern vec2			framebuffer_size;
extern vec3			forward;
#if DOUBLE_PRECISION
extern dvec4		fractalWindow;
#else
extern vec4			fractalWindow;
#endif
extern int			keys;
extern int			input_pause;
extern long			lastModifiedFile;
extern float		pausedTime;

//WINDOW:
GLFWwindow		*init(char *fname);

//SHADERS:
bool			createProgram(t_program *p, const char *path, bool fatal, bool loadChannels);
void			updateUniformLocation(t_program *p);

//UTILS
float			getCurrentTime(void);
void			loadChannel(t_program *prog, t_channel *channel, const char *file, int mode);
bool			checkFileExtention(const char *filename, const char **exts);
std::string		basename( std::string const & pathname );

//SOUND LOAD
void			fmod_init(void);
int				load_wav_file(const char *f);
FMOD_SOUND		*load_sound(const char *f);

//SOUND CONTROL
void			play_all_sounds(void);
void			pause_all_sounds(void);
void			play_sound(FMOD_SOUND *s);
void			pause_sound(FMOD_SOUND *s);
GLuint			get_sound_texture(int id);

static const char* vertex_shader_text =
"#version 330\n"
"in vec2		fragPosition;\n"
"out vec4		outColor;\n"
"void main()\n"
"{\n"
"	gl_Position = vec4(fragPosition, 0.0, 1.0);\n"
"}\n";

static const char* vertex_buffer_shader_text =
"#version 330\n"
"layout(location = 0) in vec2		fragPosition;\n"
"out vec4		outColor;\n"
"void main()\n"
"{\n"
"	gl_Position = vec4(fragPosition, 0.0, 1.0);\n"
"}\n";

static const char* fragment_shader_texture = 
"uniform sampler2D	tex;\n"
"\n"
"void mainImage(vec2 fragCoord)\n"
"{\n"
"	vec2 uv = fragCoord.xy / iResolution.xy;\n"
"	fragColor = texture(tex, uv);\n"
"}\n";

static const char* fragment_shader_text =
"#version 330\n"
#if DOUBLE_PRECISION
"#extension GL_ARB_gpu_shader_fp64 : enable\n"
#endif
"in vec4 outColor;\n"
"out vec4 fragColor;\n"
"\n"
"uniform vec2		iResolution;\n"
"uniform float		iGlobalTime;\n"
"uniform int		iFrame;\n"
"uniform vec4		iMouse;\n"
"uniform vec2		iScrollAmount;\n"
"uniform vec4		iMoveAmount;\n"
"uniform vec3		iForward;\n"
#if DOUBLE_PRECISION
"uniform dvec4		iFractalWindow;\n"
#else
"uniform vec4		iFractalWindow;\n"
#endif
"uniform sampler2D	iChannel0;\n"
"uniform sampler2D	iChannel1;\n"
"uniform sampler2D	iChannel2;\n"
"uniform sampler2D	iChannel3;\n"
"uniform sampler2D	iChannel4;\n"
"uniform sampler2D	iChannel5;\n"
"uniform sampler2D	iChannel6;\n"
"uniform sampler2D	iChannel7;\n"
"\n"
"void mainImage(vec2 f);\n"
"\n"
"vec4 texture2D(sampler2D s, vec2 coord, float f)\n"
"{\n"
"	return texture(s, coord, f);\n"
"}\n"
"\n"
"vec4 texture2D(sampler2D s, vec2 coord)\n"
"{\n"
"	return texture(s, coord);\n"
"}\n"
"\n"
"void main()\n"
"{\n"
"	mainImage(gl_FragCoord.xy);\n"
"}\n"
"#line 1\n";

#endif
