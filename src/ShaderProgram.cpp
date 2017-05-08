/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ShaderProgram.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alelievr <alelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/07 20:35:27 by alelievr          #+#    #+#             */
/*   Updated: 2017/05/08 02:25:59 by alelievr         ###   ########.fr       */
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
	this->_vbo = -1;
	this->_vao = -1;

	updateRenderSurface(RenderSurface::FULL_WINDOW);
}

ShaderProgram::~ShaderProgram(void) {}

#define CHECK_ACTIVE_FLAG(x, y) if (strstr(piece, x)) mode |= y;
const std::string		ShaderProgram::loadSourceFile(const std::string & filePath)
{
	int				chan = 0;
	int				mode;
	std::string		fileSource = "";
	std::string		line;
	std::smatch		match;

	std::ifstream	file(filePath);

	while (std::getline(file, line)) {
		if (std::regex_match(line, match, std::regex("^\\s*#\\s*pragma\\s{1,}iChannel(\\d)\\s{1,}([^\\s]*)\\s*(\\w*)\\s*(\\w*)\\s*(\\w*)\\s*(\\w*)\\s*(\\w*)\\s*(\\w*)\\s*(\\w*)\\s*(\\w*)\\s*(\\w*).*")))
		{
			std::string channelFile = match[2];
			int			channelIndex = std::stoi(match[1]);

			mode = 0;
			for (size_t i = 2; i < match.size(); ++i) {
				std::ssub_match sub_match = match[i];
				const char * piece = sub_match.str().c_str();
				CHECK_ACTIVE_FLAG("linear", CHAN_LINEAR);
				CHECK_ACTIVE_FLAG("nearest", CHAN_NEAREST);
				CHECK_ACTIVE_FLAG("mipmap", CHAN_MIPMAP);
				CHECK_ACTIVE_FLAG("v-flip", CHAN_VFLIP);
				CHECK_ACTIVE_FLAG("clamp", CHAN_CLAMP);
				std::cout << "  submatch " << i << ": " << piece << '\n';
			}
			std::cout << "mode: " << mode << std::endl;
			_channels[channelIndex].updateChannel(channelFile, mode);
		}
		else
			fileSource += line + "\n";
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

	loadUniformLocations();

	//inizialize fragPosition
	GLint       fragPos;

	fragPos = glGetAttribLocation(_id, "fragPosition");
	glEnableVertexAttribArray(fragPos);
	glVertexAttribPointer(fragPos, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*) 0);

	return true;
}

void		ShaderProgram::loadUniformLocations(void)
{
	_uniforms["iGlobalTime"] = glGetUniformLocation(_id, "iGlobalTime");
	_uniforms["iFrame"] = glGetUniformLocation(_id, "iFrame");
	_uniforms["iMouse"] = glGetUniformLocation(_id, "iMouse");
	_uniforms["iScrollAmount"] = glGetUniformLocation(_id, "iScrollAmount");
	_uniforms["iMoveAmount"] = glGetUniformLocation(_id, "iMoveAmount");
	_uniforms["iResolution"] = glGetUniformLocation(_id, "iResolution");
	_uniforms["iFractalWindow"] = glGetUniformLocation(_id, "iFractalWindow");
	_uniforms["iForward"] = glGetUniformLocation(_id, "iForward");

	_uniforms["iChannel0"] = glGetUniformLocation(_id, "iChannel0");
	_uniforms["iChannel1"] = glGetUniformLocation(_id, "iChannel1");
	_uniforms["iChannel2"] = glGetUniformLocation(_id, "iChannel2");
	_uniforms["iChannel3"] = glGetUniformLocation(_id, "iChannel3");
	_uniforms["iChannel4"] = glGetUniformLocation(_id, "iChannel4");
	_uniforms["iChannel5"] = glGetUniformLocation(_id, "iChannel5");
	_uniforms["iChannel6"] = glGetUniformLocation(_id, "iChannel6");
	_uniforms["iChannel7"] = glGetUniformLocation(_id, "iChannel7");
}

void		ShaderProgram::updateUniform1(const std::string & uniformName, int value) { glUniform1i(_uniforms[uniformName], value); }
void		ShaderProgram::updateUniform1(const std::string & uniformName, int count, int *values) { glUniform1iv(_uniforms[uniformName], count, values); }
void		ShaderProgram::updateUniform2(const std::string & uniformName, int value1, int value2) { glUniform2i(_uniforms[uniformName], value1, value2); }
void		ShaderProgram::updateUniform2(const std::string & uniformName, int count, int *values) { glUniform2iv(_uniforms[uniformName], count, values); }
void		ShaderProgram::updateUniform3(const std::string & uniformName, int value1, int value2, int value3) { glUniform3i(_uniforms[uniformName], value1, value2, value3); }
void		ShaderProgram::updateUniform3(const std::string & uniformName, int count, int *values) { glUniform3iv(_uniforms[uniformName], count, values); }
void		ShaderProgram::updateUniform4(const std::string & uniformName, int value1, int value2, int value3, int value4) { glUniform4i(_uniforms[uniformName], value1, value2, value3, value4); }
void		ShaderProgram::updateUniform4(const std::string & uniformName, int count, int *values) { glUniform4iv(_uniforms[uniformName], count, values); }

void		ShaderProgram::updateUniform1(const std::string & uniformName, float value) { glUniform1f(_uniforms[uniformName], value); }
void		ShaderProgram::updateUniform1(const std::string & uniformName, int count, float *values) { glUniform1fv(_uniforms[uniformName], count, values); }
void		ShaderProgram::updateUniform2(const std::string & uniformName, float value1, float value2) { glUniform2f(_uniforms[uniformName], value1, value2); }
void		ShaderProgram::updateUniform2(const std::string & uniformName, int count, float *values) { glUniform2fv(_uniforms[uniformName], count, values); }
void		ShaderProgram::updateUniform3(const std::string & uniformName, float value1, float value2, float value3) { glUniform3f(_uniforms[uniformName], value1, value2, value3); }
void		ShaderProgram::updateUniform3(const std::string & uniformName, int count, float *values) { glUniform3fv(_uniforms[uniformName], count, values); }
void		ShaderProgram::updateUniform4(const std::string & uniformName, float value1, float value2, float value3, float value4) { glUniform4f(_uniforms[uniformName], value1, value2, value3, value4); }
void		ShaderProgram::updateUniform4(const std::string & uniformName, int count, float *values) { glUniform4fv(_uniforms[uniformName], count, values); }

void		ShaderProgram::use(void)
{
	glUseProgram(_id);
}

void		ShaderProgram::draw(void)
{
	glBindVertexArray(_vao);
	glDrawArrays(_renderMode, 0, _renderCount);
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

void			ShaderProgram::updateRenderSurface(const RenderSurface & surf)
{
	const		float	*points;

	switch (surf)
	{
		case RenderSurface::FULL_WINDOW:
			points = (float[]){
    			-1.0f, -1.0f,
     			-1.0f,  1.0f,
      			 1.0f,  1.0f,
       			 1.0f,  1.0f,
        		 1.0f, -1.0f,
         		-1.0f, -1.0f,
      		};
			break ;
		case RenderSurface::HALF_WINDOW:
			points = (float[]){
    			-.5f, -.5f,
     			-.5f,  .5f,
      			 .5f,  .5f,
       			 .5f,  .5f,
        		 .5f, -.5f,
         		-.5f, -.5f,
      		};
			break ;
		case RenderSurface::QUARTER_WINDOW:
			points = (float[]){
    			-.25f, -.25f,
     			-.25f,  .25f,
      			 .25f,  .25f,
       			 .25f,  .25f,
        		 .25f, -.25f,
         		-.25f, -.25f,
      		};
			break ;
	}

	updateRenderSurface(points, GL_TRIANGLE_STRIP, 6);
}

void			ShaderProgram::updateRenderSurface(const float *points, GLenum renderMode, int count)
{
	_renderVertices = points;
	_renderMode = renderMode;
	_renderCount = count;
	updateVAO();
}

void			ShaderProgram::updateVAO(void)
{
	if (_vbo != -1)
		glDeleteBuffers(1, &_vbo);
	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	//TODO: Vector3 management
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * _renderCount * 2, _renderVertices, GL_STATIC_DRAW);

	if (_vao != -1)
		glDeleteVertexArrays(1, &_vao);
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
}

int				ShaderProgram::getProgramId(void) const { return (this->_id); }

int				ShaderProgram::getFramebufferId(void) const { return (this->_framebufferId); }
void			ShaderProgram::setFramebufferId(int tmp) { this->_framebufferId = tmp; }

int				ShaderProgram::getRenderId(void) const { return (this->_renderId); }
void			ShaderProgram::setRenderId(int tmp) { this->_renderId = tmp; }

ShaderChannel	*ShaderProgram::getChannel(int index) { return (this->_channels + index); }

std::ostream &	operator<<(std::ostream & o, ShaderProgram const & r)
{
	o << "tostring of the class" << std::endl;
	(void)r;
	return (o);
}
