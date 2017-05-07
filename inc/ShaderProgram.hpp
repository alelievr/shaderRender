/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ShaderProgram.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alelievr <alelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/07 20:35:23 by alelievr          #+#    #+#             */
/*   Updated: 2017/05/07 22:08:04 by alelievr         ###   ########.fr       */
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

# define CHANNEL_COUNT 8

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
		ShaderChannel						_channels[CHANNEL_COUNT];
		long								_lastModified;
		std::map< std::string, GLuint >		_uniforms;

		const std::string					loadSourceFile(const std::string & file);
		bool								checkCompilation(GLuint shader);
		bool								checkLink(GLuint program);

	public:
		ShaderProgram(void);
		ShaderProgram(const ShaderProgram&) = delete;
		virtual ~ShaderProgram(void);

		ShaderProgram &	operator=(ShaderProgram const & src) = delete;

		bool	loadFragmentFile(const std::string & file);
		bool	loadVertexFile(const std::string & file);
		bool	compileAndLink(void);
		void	Use(void);
		void	updateUniforms(void);
		void	deleteProgram(void);
		void	reloadModifiedFiles(void);
		
		int		getId(void) const;
		void	setId(int tmp);
		
		int		getFramebufferId(void) const;
		void	setFramebufferId(int tmp);
		
		int		getRenderId(void) const;
		void	setRenderId(int tmp);
		
		ShaderChannel	getChannel(int index) const;
		void	setChannel(ShaderChannel chan, int index);
};

std::ostream &	operator<<(std::ostream & o, ShaderProgram const & r);

#endif
