/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LuaGL.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpirsch <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/26 05:35:14 by jpirsch           #+#    #+#             */
/*   Updated: 2017/06/03 02:32:06 by jpirsch          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LuaGL.hpp"

static const luaL_Reg Functions[] =
{
	{"get_prog", get_prog},
	{"use_prog", use_prog},
	{NULL, NULL}
};

ShaderRender  *getSR(ShaderRender *shadren)
{
	static ShaderRender *sr;

	if (shadren == NULL)
		return (sr);
	else if (shadren)
		sr = shadren;
	return (NULL);
}

lua_State	*getL(lua_State *l)
{
	static lua_State	*L;

	if (l == NULL)
		return (L);
	else if (l)
		L = l;
	return (NULL);
}

int	init_LuaGL(ShaderRender *sr)
{
	lua_State	*L;

	getSR(sr);
	L = luaL_newstate();    // All Lua contexts
	getL(L);
	
    // Into the lua "gol" table
	luaL_register(L, "gol", Functions);
	luaL_openlibs(L);       // Load Lua libraries

	load_run_script(L, "lua/init_loop.lua");
	lua_getfield(L, LUA_GLOBALSINDEX, "init_oo");
	lua_call(L, 0, 0);
}

int	load_run_script(lua_State *L, char *script)
{
	int	status, result;

	status = luaL_loadfile(L, script);
	if (status)
	{
		fprintf(stderr, "Couldn't load file: %s\n", lua_tostring(L, -1));
		exit(1);
	}
	result = lua_pcall(L, 0, LUA_MULTRET, 0); // Lua run script
	if (result)
	{
		fprintf(stderr, "Failed to run script: %s\n", lua_tostring(L, -1));
		exit(1);
	}
	return (0);
}

int	get_prog(lua_State *L)
{
	ShaderProgram	*prog;

	prog = getSR(NULL)->getProgram();
	lua_pushinteger(L, lua_Integer(prog));
	return (1);
}

int	use_prog(lua_State *L)
{
	ShaderProgram	*prog;

	prog = (ShaderProgram*)(luaL_checkinteger(L, 1));
	prog->use();
	return (0);
}
