/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LuaGL.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpirsch <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/26 05:39:09 by jpirsch           #+#    #+#             */
/*   Updated: 2017/06/09 04:44:20 by jpirsch          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LUAGL
# define LUAGL
# include "lua_utils.hpp"
# include "ShaderRender.hpp"
# include "Element.hpp"

class ShaderRender;

ShaderRender	*getSR(ShaderRender *shadren);
lua_State		*getL(lua_State *L);
int				init_LuaGL(ShaderRender* ShadRen);
int				load_run_script(lua_State *L, char *script);
int				get_prog(lua_State *L);
int				use_prog(lua_State *L);
int				create_element(lua_State *L);
int				draw_element(lua_State *L);
int				delete_element(lua_State *L);

//	std::vector< std::function< int<lua_State *> > >	funcs;
//void			add_func(lua_func LuaGL::*(luaState *) func);


#endif
