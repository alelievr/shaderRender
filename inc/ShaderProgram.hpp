/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ShaderProgram.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alelievr <alelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/07 20:35:23 by alelievr          #+#    #+#             */
/*   Updated: 2017/05/08 02:03:49 by alelievr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHADERPROGRAM_HPP
# define SHADERPROGRAM_HPP
# include <iostream>
# include <string>
# include <vector>
# include <fstream>
# include <streambuf>
# include <regex>
# include <map>
# include <exception>
# include "sys/stat.h"

# include "shaderpixel.h"
# include "ShaderChannel.hpp"

enum class	RenderSurface
{
	FULL_WINDOW,
	HALF_WINDOW,
	QUARTER_WINDOW,
};

class		ShaderProgram
{
	private:
		struct ShaderFile {
			std::string		file;
			std::string		source;
			long			lastModified;

			ShaderFile(std::string file, std::string source)
			{
				struct stat		st;

				this->file = file;
				this->source = source;
				stat(file.c_str(), &st);
				lastModified = st.st_mtime;
			}
		};

		std::vector< ShaderFile >			_fragmentFileSources;
		std::vector< ShaderFile>			_vertexFileSources;
		GLuint								_id;
		int									_framebufferId;
		int									_renderId;
		ShaderChannel						_channels[MAX_CHANNEL_COUNT];
		long								_lastModified;
		std::map< std::string, GLuint >		_uniforms;

		GLuint								_vao;
		GLuint								_vbo;
		GLenum								_renderMode;
		const float							*_renderVertices;
		GLuint								_renderVAO;
		GLsizei								_renderCount;

		const std::string					loadSourceFile(const std::string & file);
		bool								checkCompilation(GLuint shader);
		bool								checkLink(GLuint program);
		void								updateVAO(void);
		void								loadUniformLocations(void);

	public:
		ShaderProgram(void);
		ShaderProgram(const ShaderProgram&) = delete;
		virtual ~ShaderProgram(void);

		ShaderProgram &	operator=(ShaderProgram const & src) = delete;

		bool	loadFragmentFile(const std::string & file);
		bool	loadVertexFile(const std::string & file);
		bool	compileAndLink(void);
		void	use(void);
		void	draw(void);
		void	deleteProgram(void);
		void	reloadModifiedFiles(void);
		void	updateRenderSurface(const RenderSurface & surface);
		void	updateRenderSurface(const float *points, GLenum renderMode, int count);

		void		updateUniform1(const std::string & uniformName, int value);
		void		updateUniform1(const std::string & uniformName, int count, int *values);
		void		updateUniform2(const std::string & uniformName, int value1, int value2);
		void		updateUniform2(const std::string & uniformName, int count, int *values);
		void		updateUniform3(const std::string & uniformName, int value1, int value2, int value3);
		void		updateUniform3(const std::string & uniformName, int count, int *values);
		void		updateUniform4(const std::string & uniformName, int value1, int value2, int value3, int value4);
		void		updateUniform4(const std::string & uniformName, int count, int *values);

		void		updateUniform1(const std::string & uniformName, float value);
		void		updateUniform1(const std::string & uniformName, int count, float *values);
		void		updateUniform2(const std::string & uniformName, float value1, float value2);
		void		updateUniform2(const std::string & uniformName, int count, float *values);
		void		updateUniform3(const std::string & uniformName, float value1, float value2, float value3);
		void		updateUniform3(const std::string & uniformName, int count, float *values);
		void		updateUniform4(const std::string & uniformName, float value1, float value2, float value3, float value4);
		void		updateUniform4(const std::string & uniformName, int count, float *values);

		int		getProgramId(void) const;

		int		getFramebufferId(void) const;
		void	setFramebufferId(int fbo);

		int		getRenderId(void) const;
		void	setRenderId(int renderId);
		
		ShaderChannel	*getChannel(int index);
};

std::ostream &	operator<<(std::ostream & o, ShaderProgram const & r);

#endif
