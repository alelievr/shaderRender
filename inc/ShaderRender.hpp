/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ShaderRender.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alelievr <alelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/07 21:28:59 by alelievr          #+#    #+#             */
/*   Updated: 2017/06/10 02:30:00 by alelievr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

# include "shaderpixel.h"
# include <vector>
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

#include "ShaderProgram.hpp"
#include "ShaderChannel.hpp"
#include "LuaGL.hpp"

//#define UNIFORM_DEBUG 1

class ShaderRender
{
	private:
		vec4			mouse = {0, 0, 0, 0};
		vec2			scroll = {0, 0};
		vec4			move = {0, 0, 0, 1};
		vec3			forward = {0, 0, 1};
#if DOUBLE_PRECISION
		dvec4			fractalWindow = {-1, -1, 1, 1}; //xmin, ymin, xmax, ymax
#else
		vec4			fractalWindow = {-1, -1, 1, 1}; //xmin, ymin, xmax, ymax
#endif
		int				keys = 0;
		int				input_pause = 0;

		std::vector< ShaderProgram * >	_programs;
		std::vector< std::size_t >		_currentRenderedPrograms;

		vec2			angleAmount;
		int				cursor_mode;
		float			lastPausedTime;
		bool			programLoaded;

		void			updateUniforms(ShaderProgram *p);
		vec3			vec3Cross(vec3 v1, vec3 v2);
		void			updateKeys(void);
		void			displayWindowFps(void);
		void			foreachShaderChannels(std::function< void(ShaderProgram *, ShaderChannel *)> callback, ShaderProgram *currentShaderProgram = NULL);

	public:
		ShaderRender(void);
		ShaderRender(const ShaderRender & rhs) = delete;
		ShaderRender &		operator=(ShaderRender & ths) = delete;

		virtual ~ShaderRender(void);

		void				render(GLFWwindow *win);
		bool				attachShader(const std::string file);
		void				SetCurrentRenderedShader(const size_t programIndex);
		void				AddCurrentRenderedShader(const size_t programIndex);
		void				DeleteCurrentRenderedShader(const size_t programIndex);
		void				ClearCurrentRenderedShader(void);
		void				initShaderRenders(ShaderRender rs);

		void				windowSizeCallback(int winX, int winY);
		void				framebufferSizeCallback(int width, int height);
		void				scrollCallback(double xOffset, double yOffset);
		void				clickCallback(int button, int action, int mods);
		void				mousePositionCallback(GLFWwindow *win, double x, double y);
		void				keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

		ShaderChannel		*getChannel(const int programIndex, const int channel) const;
		ShaderProgram		*getProgram(const int programIndex) const;
};
