/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ShaderProgram.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alelievr <alelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/07 20:35:27 by alelievr          #+#    #+#             */
/*   Updated: 2017/05/07 22:08:02 by alelievr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ShaderProgram.hpp"


ShaderProgram::ShaderProgram(void)
{
	std::cout << "Default constructor of ShaderProgram called" << std::endl;
	this->_id = 0;
	this->_framebufferId = 0;
	this->_renderId = 0;
	this->_lastModified = 0;
}

ShaderProgram::~ShaderProgram(void)
{
	std::cout << "Destructor of ShaderProgram called" << std::endl;
}

#define CHECK_ACTIVE_FLAG(x, y) if (strstr(line, x)) mode |= y;
const std::string		ShaderProgram::loadSourceFile(const std::string & filePath)
{
	int				chan = 0;
	std::string		fileSource = "";
	std::string		line;
	std::smatch		match;

	std::ifstream	file(filePath);

	while (std::getline(file, line)) {
		std::cout << line << std::endl;
		if (std::regex_match(line, match, std::regex("^\\s*#\\s*pragma[\\s]{1,}iChannel\\d\\s{1,}.*")))
		{
			for (size_t i = 0; i < match.size(); ++i) {
				std::ssub_match sub_match = match[i];
				std::string piece = sub_match.str();
				std::cout << "  submatch " << i << ": " << piece << '\n';
			}   

			//TODO
			int		mode = 0;
			int		channelIndex = 0;
			std::string file = "textures/EXPLOSION.png";
			/*line += 18;
			strreplace(line, "//", "\0\0");
			int i = 0;
			int chan = line[-2] - '0';
			printf("chan = %i\n", chan);
			while (*line && !isspace(*line))
				p->channels[chan].file_path[i++] = *line++;
			p->channels[chan].file_path[i++] = 0;
			CHECK_ACTIVE_FLAG("linear", CHAN_LINEAR);
			CHECK_ACTIVE_FLAG("nearest", CHAN_NEAREST);
			CHECK_ACTIVE_FLAG("mipmap", CHAN_MIPMAP);
			CHECK_ACTIVE_FLAG("v-flip", CHAN_VFLIP);
			CHECK_ACTIVE_FLAG("clamp", CHAN_CLAMP);*/
			_channels[channelIndex].updateChannel(file, mode);
		}
		else
			fileSource += line;
	}
	return fileSource;
}

bool		ShaderProgram::loadFragmentFile(const std::string & file)
{
	try {
		_fragmentFileSources.push_back(ShaderFile(file, loadSourceFile(file)));
		return true;
	} catch (const std::exception & e) {
		return false;
	}
}

bool		ShaderProgram::loadVertexFile(const std::string & file)
{
	try {
		_vertexFileSources.push_back(ShaderFile(file, loadSourceFile(file)));
		return true;
	} catch (const std::exception & e) {
		return false;
	}
}

bool		ShaderProgram::compileAndLink(void)
{
	struct	stat	st;

	if (_fragmentFileSources.size() == 0)
		return false;

	GLuint		fragmentShaderId;
	//TODO: multi-shader file management for fragments and vertex
	const char	*srcs[] = {fragment_shader_text, _fragmentFileSources[0].source.c_str()};

	fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderId, 2, srcs, NULL);
	glCompileShader(fragmentShaderId);
	if (!checkCompilation(fragmentShaderId))
		return false;

	GLuint		vertexShaderId;
	static bool	render_shader = false;
	const char	*vertex_main_src;

	if (render_shader)
		vertex_main_src = vertex_buffer_shader_text;
	else
		vertex_main_src = vertex_shader_text;
	vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderId, 1, &vertex_main_src, NULL);
	glCompileShader(vertexShaderId);
	if (!checkCompilation(vertexShaderId))
		return false;

	if (_id != 0)
		glDeleteProgram(_id);
	_id = glCreateProgram();
	printf("new program ID: %i\n", _id);
	glAttachShader(_id, vertexShaderId);
	glAttachShader(_id, fragmentShaderId);
	glLinkProgram(_id);
	if (!checkLink(_id))
		return false;

	//TODO: get all uniforms for update

	return true;
}

void		ShaderProgram::updateUniforms(void)
{

}

void		ShaderProgram::Use(void)
{
	glUseProgram(_id);
}

void		ShaderProgram::deleteProgram(void)
{
	glUseProgram(0);
	glDeleteProgram(_id);
	_fragmentFileSources.clear();
	_vertexFileSources.clear();
}

bool		ShaderProgram::checkLink(GLuint program)
{
	GLint isLinked = 0;

	glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		char		buff[maxLength];
		glGetProgramInfoLog(program, maxLength, &maxLength, buff);
		printf("%s\n", buff);

		glDeleteProgram(program);
		return false;
	}
	return true;
}

bool		ShaderProgram::checkCompilation(GLuint shader)
{
	GLint isCompiled = 0;

	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		char		buff[maxLength];
		glGetShaderInfoLog(shader, maxLength, &maxLength, buff);
		printf("%s\n", buff);

		glDeleteShader(shader);
		return false;
	}
	return true;
}

void		ShaderProgram::reloadModifiedFiles(void)
{
	//TODO
}

int			ShaderProgram::getId(void) const { return (this->_id); }
void		ShaderProgram::setId(int tmp) { this->_id = tmp; }

int			ShaderProgram::getFramebufferId(void) const { return (this->_framebufferId); }
void		ShaderProgram::setFramebufferId(int tmp) { this->_framebufferId = tmp; }

int			ShaderProgram::getRenderId(void) const { return (this->_renderId); }
void		ShaderProgram::setRenderId(int tmp) { this->_renderId = tmp; }

ShaderChannel	ShaderProgram::getChannel(int index) const { return (this->_channels[index]); }
void			ShaderProgram::setChannel(ShaderChannel tmp, int index) { this->_channels[index] = tmp; }

std::ostream &	operator<<(std::ostream & o, ShaderProgram const & r)
{
	o << "tostring of the class" << std::endl;
	(void)r;
	return (o);
}
