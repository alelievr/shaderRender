/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lua_utils.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpirsch <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/06/04 06:31:22 by jpirsch           #+#    #+#             */
/*   Updated: 2017/06/08 06:23:25 by jpirsch          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LUA_UTILS
# define LUA_UTILS
# include <vector>
# include <shaderpixel.h>
# include <lua.h>
# include <lualib.h>
# include <lauxlib.h>

unsigned short*	pop_ushort(lua_State* state, int narg, size_t* outLength);
vec2*			pop_vec2(lua_State *state, int narg, size_t *outLength);
vec3*			pop_vec3(lua_State* state, int narg, size_t* outLength);

#endif
