/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Element.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpirsch <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/06/04 06:42:17 by jpirsch           #+#    #+#             */
/*   Updated: 2017/06/09 04:12:32 by jpirsch          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ELEMENT
# define ELEMENT
# include <shaderpixel.h>

typedef struct
{
	vec3	pos;
	vec2	uv;
}		vertices;

class Element
{
	public:
		Element(ushort *ind, int ind_nb, vec3 *pos, int pos_nb, vec2  *uv,
				int uv_nb);
		void	draw(GLuint vert_loc, GLuint uv_loc);
		virtual ~Element(void);

//		bool	LoadShader(const std::string & shaderFile);

	private:
		GLuint		_ind_nb;
		GLuint		_ind_id;
		GLuint		_vert_id;
};

#endif
