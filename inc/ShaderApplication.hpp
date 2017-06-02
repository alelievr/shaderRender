/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ShaderApplication.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpirsch <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/26 03:37:10 by jpirsch           #+#    #+#             */
/*   Updated: 2017/06/02 18:51:54 by alelievr         ###   ########.fr       */
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
		void	RenderLoop(void);

	private:
		ShaderRender		*mShaderRender;
		GLFWwindow			*window;
//		lua_State			*L;
//		int					status;

//		int					load_run_script(lua_State *L, char *script);
};

#endif
