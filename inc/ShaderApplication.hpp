/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ShaderApplication.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpirsch <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/26 03:37:10 by jpirsch           #+#    #+#             */
/*   Updated: 2017/06/10 02:20:08 by alelievr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHADER_APPLICATION
# define SHADER_APPLICATION
# include "shaderpixel.h"
# include "ShaderRender.hpp"

class ShaderApplication
{
	public:
		ShaderApplication(bool server = false);
		virtual ~ShaderApplication(void);

		bool	LoadShader(const std::string & shaderFile);
		void	FocusShader(const int programIndex);
		void	RenderLoop(void);

	private:
		ShaderRender		*shaderRender;
		GLFWwindow			*window;
};

#endif
