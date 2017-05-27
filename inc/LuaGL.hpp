/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LuaGL.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpirsch <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/26 05:39:09 by jpirsch           #+#    #+#             */
/*   Updated: 2017/05/26 15:37:45 by jpirsch          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ShaderRender.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#ifndef LUAGL
# define LUAGL

class	LuaGL
{
	public:
		LuaGL(void);
		~LuaGL(void);

		lua_State	*getL(void);

		int			load_run_script(lua_State *L, char *script);

	private:
		lua_State	*L;
		int			status;

		static int	func(lua_State *L);
};

#endif
