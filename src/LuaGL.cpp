/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LuaGL.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpirsch <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/26 05:35:14 by jpirsch           #+#    #+#             */
/*   Updated: 2017/06/09 04:10:39 by jpirsch          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LuaGL.hpp"

static const luaL_Reg Functions[] =
{
	{"get_prog", get_prog},
	{"use_prog", use_prog},
	{"create_element", create_element},
	{"draw_element", draw_element},
	{"delete_element", delete_element},
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

int	create_element(lua_State *L)
{
    size_t ind_nb; // indices
    unsigned short* ind = pop_ushort( L, 1, &ind_nb );
    
    size_t pos_nb; // positions
    vec3* pos = pop_vec3( L, 2, &pos_nb );
    
    size_t uv_nb; // texture coordinates
    vec2* uv = pop_vec2( L, 3, &uv_nb );
    
	Element	*elem = new Element(ind, ind_nb, pos, pos_nb, uv, uv_nb);
	lua_pushinteger(L, lua_Integer(elem));
	return (1);
}

int	draw_element(lua_State *L)
{
	Element *elem = (Element*)(luaL_checkinteger(L, 1));
	GLuint vert_loc = (GLuint)(luaL_checkinteger(L, 2));
	GLuint uv_loc = (GLuint)(luaL_checkinteger(L, 3));
	
	elem->draw(vert_loc, uv_loc);
	return (0);
}

int				delete_element(lua_State *L)
{
	Element	*elem;

	elem = (Element*)(luaL_checkinteger(L, 1));
	delete elem;
	return (0);
}
