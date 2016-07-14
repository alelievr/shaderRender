/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shaderpixel.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alelievr <alelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/07/11 18:11:58 by alelievr          #+#    #+#             */
/*   Updated: 2016/07/14 17:04:18 by alelievr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHADERPIXEL_H
# define SHADERPIXEL_H

# define GLFW_INCLUDE_GLCOREARB
# include "GLFW/glfw3.h"

# define WIN_W 1080
# define WIN_H 720

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

static const char* vertex_shader_text =
"#version 330\n"
"in vec2		iResolutionIn;\n"
"out vec2		iResolution;\n"
"in vec2		fragPosition;\n"
"out vec4		outColor;\n"
"void main()\n"
"{\n"
"	iResolution = iResolutionIn;\n"
"	gl_Position = vec4(fragPosition, 0.0, 1.0);\n"
"}\n";

static const char* fragment_shader_image_text = 
"void mainImage(vec2 fragCoord)\n"
"{\n"
"	vec2 uv = fragCoord.xy / iResolution.xy;\n"
"	fragColor = vec4(uv, 0.5 + 0.5 * sin(iGlobalTime), 1.0);\n"
"}\n";

static const char* fragment_shader_text =
"#version 330\n"
"in vec4 outColor;\n"
"out vec4 fragColor;\n"
"\n"
"in vec2			iResolution;\n"
"uniform float		iGlobalTime;\n"
"uniform int		iFrame;\n"
"uniform vec4		iMouse;\n"
"uniform vec2		iScrollAmount;\n"
"uniform vec4		iMoveAmount;\n"
"\n"
"void mainImage(vec2 f);\n"
"\n"
"void main()\n"
"{\n"
"	mainImage(gl_FragCoord.xy);\n"
"}\n";

#endif
