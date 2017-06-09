/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Element.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpirsch <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/06/04 06:42:02 by jpirsch           #+#    #+#             */
/*   Updated: 2017/06/09 04:14:16 by jpirsch          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Element.hpp"
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

Element::Element(ushort *ind, int ind_nb, vec3 *pos, int pos_nb, vec2  *uv,
				int uv_nb)
{
	this->_ind_nb = ind_nb;

	glGenBuffers(1, &_ind_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ind_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind_nb, ind, GL_STATIC_DRAW);
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0);

	// Group all vertex params in a single array
	vec3		*pt_pos = pos;
	vec2		*pt_uv = uv;
	vertices	*vertex = (vertices*)(malloc( pos_nb * sizeof(vertices)));
	vertices	*pt_vertex = vertex;

	for (int i = pos_nb ; i > 0 ; --i)
	{
		pt_vertex->pos = *pt_pos;
		pt_vertex->uv = *pt_uv;
		++pt_pos;
		++pt_uv;
		++pt_vertex;
	}
	glGenBuffers( 1, &this->_vert_id );
	glBindBuffer( GL_ARRAY_BUFFER, this->_vert_id );
	glBufferData( GL_ARRAY_BUFFER, pos_nb * sizeof(vertices), vertex,
				GL_STATIC_DRAW );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	free( vertex );
}

void	Element::draw(GLuint vert_loc, GLuint uv_loc)
{
	glBindBuffer(GL_ARRAY_BUFFER, this->_vert_id);

	glEnableVertexAttribArray(vert_loc);
	glEnableVertexAttribArray(uv_loc);

	glVertexAttribPointer(vert_loc,	3, GL_FLOAT, GL_FALSE, sizeof(vertices),
							BUFFER_OFFSET(0) );
	glVertexAttribPointer(uv_loc, 2, GL_FLOAT, GL_FALSE, sizeof(vertices),
							BUFFER_OFFSET(sizeof(vec3)) ); // After .position

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_ind_id);
	glDrawElements(GL_TRIANGLES, this->_ind_nb, GL_UNSIGNED_SHORT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
	glDisableVertexAttribArray(vert_loc);
	glDisableVertexAttribArray(uv_loc);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Element::~Element(void)
{
}
