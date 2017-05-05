/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RenderShader.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alelievr <alelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/04 22:50:08 by alelievr          #+#    #+#             */
/*   Updated: 2017/05/05 14:16:19 by alelievr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RENDERSHADER_HPP
# define RENDERSHADER_HPP

# include "shaderpixel.h"
# include <stdio.h>
# include <stdlib.h>
# include <sys/time.h>
# include <fcntl.h>
# include <unistd.h>
# include <stdbool.h>
# include <sys/stat.h>
# include <string.h>
# include <time.h>
# include <math.h>

//#define UNIFORM_DEBUG 1

class RenderShader
{
	private:
		vec4        mouse = {0, 0, 0, 0};
		vec2        scroll = {0, 0};
		vec4        move = {0, 0, 0, 1};
		vec2		window = {WIN_W, WIN_H};
		vec3		forward = {0, 0, 1};
#if DOUBLE_PRECISION
		dvec4		fractalWindow = {-1, -1, 1, 1}; //xmin, ymin, xmax, ymax
#else
		vec4		fractalWindow = {-1, -1, 1, 1}; //xmin, ymin, xmax, ymax
#endif
		int        	keys = 0;
		int         input_pause = 0;

		GLuint		vao;
		t_program	program[0xF];

		vec2    angleAmount;
		int     cursor_mode;
		float   lastPausedTime;

		const float points[12] = {
   			-1.0f,  -1.0f,
    		-1.0f, 1.0f,
			1.0f, 1.0f,
			1.0f, 1.0f,
			1.0f, -1.0f,
   			-1.0f,  -1.0f,
		};

		GLuint		createVBO(void);
		GLuint		createVAO(GLuint vbo, t_program *program);
		void		updateUniforms(GLint *unis, t_channel *channels);
		vec3		vec3Cross(vec3 v1, vec3 v2);
		void		updateKeys(void);
		void		checkFileChanged(t_program *progs);
		void		displayWindowFps(void);

	public:
		RenderShader();
		RenderShader(const RenderShader & rhs) = delete;

		virtual ~RenderShader();

		void				render(void);
		void				loadShaderFile(char *file);
		void				initRenderShaders(RenderShader rs);
		void				windowSizeCallback(int winX, int winY);
		void				framebufferSizeCallback(int width, int height);
		void				scrollCallback(double xOffset, double yOffset);
		void				clickCallback(int button, int action, int mods);
		void				mousePositionCallback(GLFWwindow *win, double x, double y);
		void				keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);


		RenderShader &		operator=(RenderShader & ths) = delete;
};

#endif
