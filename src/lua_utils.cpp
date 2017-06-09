/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lua_utils.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpirsch <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/06/04 03:13:28 by jpirsch           #+#    #+#             */
/*   Updated: 2017/06/08 08:39:18 by jpirsch          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lua_utils.hpp"

unsigned short* pop_ushort( lua_State* state, int narg,
												size_t* outLength )
{
	int nb = (int)lua_objlen( state, narg );
	*outLength = nb ;
	unsigned short* indices = (ushort*)(malloc( nb * sizeof(unsigned short) ));
	unsigned short* pt = indices;
	for( int i = 1; i <= nb; ++i )
	{
		lua_pushinteger( state, i ); // key
		lua_gettable( state, narg ); // Value at key (lua_gettable pops the key)
		*pt = (unsigned short)luaL_checkinteger( state, -1 );
		lua_pop(state, 1);
		++pt;
	}
	return indices;
}

#define READ_FIELD( dst, i )				\
{											\
	lua_rawgeti( state, -1, i );			\
	(dst) = luaL_checknumber(state, -1);	\
	lua_pop(state, 1);						\
}

vec2* pop_vec2(lua_State *state, int narg, size_t *outLength)
{
    int nb = (int)lua_objlen( state, narg );
    *outLength = nb;
    vec2* vectors = (vec2*)(malloc( nb * sizeof(vec2) ));
    vec2* pt = vectors;
    for( int i = 1; i <= nb; ++i )
    {
        // vectors[vectorIndex]
        lua_pushinteger( state, i ); // key
        lua_gettable( state, -2 ); // Value at key (lua_gettable pops the key)
        if( lua_istable( state, -1) )
        {
			READ_FIELD( pt->x, 1 );
			READ_FIELD( pt->y, 2 );
            ++pt;
        }
        else
        {
            std::cout << "The table contains something else than a table" << std::endl;
            lua_error(state);
            return 0;
        }
        lua_pop(state, 1); // Remove the subtable
    }
    return vectors;
}

vec3* pop_vec3(lua_State* state, int narg, size_t* outLength)
{
	int count = (int)lua_objlen( state, narg );
	*outLength = count;
	vec3* vectors = (vec3*)(malloc( *outLength * sizeof(vec3) ));
	vec3* pt = vectors;
	for( int i = 0; i < count; ++i )
	{
		// vectors[vectorIndex]
		lua_pushinteger( state, i+1 ); // key
		lua_gettable( state, narg ); // Value at key (lua_gettable pops the key)
		if( lua_istable( state, -1 ) )
		{
			READ_FIELD( pt->x, 1 );
			READ_FIELD( pt->y, 2 );
			READ_FIELD( pt->z, 3 );
			++pt;
		}
		else
		{
            std::cout << "The table contains something else than a table" << std::endl;
			lua_error(state);
			return 0;
		}
		lua_pop(state, 1); // Remove the subtable
	}
	return vectors;
}
