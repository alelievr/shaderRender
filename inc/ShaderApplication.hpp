/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ShaderApplication.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpirsch <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/26 03:37:10 by jpirsch           #+#    #+#             */
/*   Updated: 2017/06/06 19:19:06 by alelievr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shaderpixel.h"
#include "ShaderRender.hpp"

#ifndef SHADER_APPLICATION
# define SHADER_APPLICATION

class ShaderApplication
{
	public:
		ShaderApplication(bool server = false);
		virtual ~ShaderApplication(void);

		bool	LoadShader(const std::string & shaderFile);
		void	SwapShaderRender(void);
		void	RenderLoop(void);

	private:
		ShaderRender		*currentShaderRender;
		ShaderRender		*bufferedShaderRender;
		GLFWwindow			*window;
//		lua_State			*L;
//		int					status;

//		int					load_run_script(lua_State *L, char *script);
};

#endif
