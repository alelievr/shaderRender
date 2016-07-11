/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shaderpixel.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alelievr <alelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/07/11 18:11:58 by alelievr          #+#    #+#             */
/*   Updated: 2016/07/11 21:50:40 by alelievr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHADERPIXEL_H
# define SHADERPIXEL_H

# define GLFW_INCLUDE_GLCOREARB
# include "GLFW/glfw3.h"

# define WIN_W 1920
# define WIN_H 1080

static const char* vertex_shader_image_text = 
"vec4 mainImage(vec2 fragCoord)\n"
"{\n"
"	vec2 uv = fragCoord.xy / iResolution.xy;\n"
"	return vec4(uv, 0.5 + 0.5 * sin(iGlobalTime), 1.0);\n"
"}\n"
;

static const char* vertex_shader_text =
"#version 330\n"
"in vec2		iResolution;\n"
"in vec2		fragPosition;\n"
"uniform float	iGlobalTime;\n"
"uniform int	iFrame;\n"
"uniform vec2	iMouse;\n"
"out vec4 fragColor;\n"
"vec4 mainImage(vec2 f);\n"
"void main()\n"
"{\n"
"	gl_Position = vec4(fragPosition, 0.0, 1.0);\n"
"	fragColor = mainImage(fragPosition);\n"
"}\n";

static const char* fragment_shader_text =
"#version 330\n"
"in vec4 fragColor;\n"
"out vec4 outColor;\n"
"void main()\n"
"{\n"
"   outColor = fragColor;\n"
"}\n";

#endif
