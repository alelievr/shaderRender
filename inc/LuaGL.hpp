/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LuaGL.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpirsch <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/26 05:39:09 by jpirsch           #+#    #+#             */
/*   Updated: 2017/06/03 02:29:56 by jpirsch          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ShaderRender.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#ifndef LUAGL
# define LUAGL

class ShaderRender;

ShaderRender	*getSR(ShaderRender *shadren);
lua_State		*getL(lua_State *L);
int				init_LuaGL(ShaderRender* ShadRen);
int				load_run_script(lua_State *L, char *script);
int				get_prog(lua_State *L);
int				use_prog(lua_State *L);

//	std::vector< std::function< int<lua_State *> > >	funcs;
//void			add_func(lua_func LuaGL::*(luaState *) func);


#endif
