/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LuaGL.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpirsch <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/26 05:35:14 by jpirsch           #+#    #+#             */
/*   Updated: 2017/06/01 22:31:09 by alelievr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LuaGL.hpp"


/*static const luaL_Reg lgl[] =
{
	{"func", my_func},
	{NULL, NULL}
};*/

LuaGL::LuaGL(void)
{
	L = luaL_newstate();    // All Lua contexts
        // Register C functions to Lua
        static const luaL_Reg Functions[] = {
/*            {"create_program",			gol_create_program},
            {"delete_program",          gol_delete_program},
            {"use_program",				gol_use_program},
            
            {"get_uniform_location",	gol_get_uniform_location},
            {"set_uniform_float",		gol_set_uniform_float},
            {"set_uniform_int",			gol_set_uniform_int},
            {"set_uniform_vec4",		gol_set_uniform_vec4},
            {"set_uniform_mat4",		gol_set_uniform_mat4},
            {"get_attribute_location",	gol_get_attribute_location},
            
            {"clear",					gol_clear},
            {"clearColor",				gol_clearColor},
            {"set_line_width",			gol_set_line_width},
            {"draw_line_strip",			gol_draw_line_strip},
            {"draw_line_loop",			gol_draw_line_loop},
            
            {"create_element",			gol_create_element},
            {"delete_element",			gol_delete_element},
            
            {"draw_element",			gol_draw_element},
            
            {"get_size_tex2d",			gol_get_size_tex2d},
            {"bind_tex2d",				gol_bind_tex2d},
            {"delete_tex2d",			gol_delete_tex2d},
            
            {"create_fbo",				gol_create_fbo},
            {"delete_fbo",				gol_delete_fbo},
            {"bind_fbo",				gol_bind_fbo},
            {"get_fbo_tex2d",			gol_get_fbo_tex2d},*/
            {"func",			func},
            
            {NULL, NULL}  // sentinel
        };
    
    // Into the "gol" table (acts like a namespace)
	luaL_register(L, "gol", Functions);
    //    luaL_newlib(L, Functions);
	//	lua_setglobal(L, "gol");
	luaL_openlibs(L);       // Load Lua libraries

	load_run_script(L, "lua/init_loop.lua");
	lua_getfield(L, LUA_GLOBALSINDEX, "init_oo");
	lua_call(L, 0, 0);
//	load_run_script(L, "lua/draw.lua");
}

int	LuaGL::load_run_script(lua_State *L, const char *script)
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

int	LuaGL::func(lua_State *L)
{
	(void)L;
	printf("yolo i binded a function :)\n");
	return (0);
}

lua_State	*LuaGL::getL(void)
{
	return (L);
}

LuaGL::~LuaGL(void) {}
