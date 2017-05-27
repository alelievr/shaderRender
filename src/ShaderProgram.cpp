/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ShaderProgram.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alelievr <alelievr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/05/07 20:35:27 by alelievr          #+#    #+#             */
/*   Updated: 2017/05/26 04:55:55 by jpirsch          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ShaderProgram.hpp"

//#define DEBUG

ShaderProgram::ShaderProgram(void)
{
	this->_id = 0;
	this->_framebufferId = 0;
	this->_renderId = -1;
	this->_lastModified = 0;
	this->_vbo = -1;
	this->_vao = -1;

	this->_channels = new ShaderChannel[MAX_CHANNEL_COUNT];

	updateRenderSurface(RenderSurface::FULL_WINDOW);
}

ShaderProgram::~ShaderProgram(void) {
	delete [] this->_channels;
}

#define CHECK_ACTIVE_FLAG(x, y) if (strstr(piece, x)) mode |= y;
const std::string		ShaderProgram::loadSourceFile(const std::string & filePath)
{
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
			}
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
	if (_fragmentFileSources.size() == 0)
		return false;

	GLuint		fragmentShaderId;
	//TODO: multi-shader file management for fragments and vertex
	const char	*srcs[] = {fragment_shader_text, _fragmentFileSources[0].source.c_str()};

	std::cout << "compiling " << _fragmentFileSources[0].file << "\n";

	fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderId, 2, srcs, NULL);
	glCompileShader(fragmentShaderId);
	if (!checkCompilation(fragmentShaderId))
		return false;

	GLuint		vertexShaderId;
	const char	*vertex_main_src;

	vertex_main_src = vertex_buffer_shader_text;
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

	if ((fragPos = glGetAttribLocation(_id, "fragPosition")) < 0)
		return false;
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

//TODO: delete numbers
void		ShaderProgram::updateUniform1(const std::string & uniformName, int value) {
#ifdef DEBUG
	std::cout << uniformName << " -> " << (int)_uniforms[uniformName] << std::endl;
#endif
   glUniform1i(_uniforms[uniformName], value);
}
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
#ifdef DEBUG
	std::cout << "use " << _id << std::endl;
#endif
	glUseProgram(_id);
}

void		ShaderProgram::draw(void)
{
	if (_renderId != -1)
	{
		GLenum buffers[] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, buffers);
	}
#ifdef DEBUG
	std::cout << "drawing program: " << _id << " to " << _vao << "\n";
#endif
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
	static bool		once = true;

	if (once)
	{
		if (_vbo != (GLuint)-1)
			glDeleteBuffers(1, &_vbo);
		glGenBuffers(1, &_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		//TODO: Vector3 management
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * _renderCount * 2, _renderVertices, GL_STATIC_DRAW);

		if (_vao != (GLuint)-1)
			glDeleteVertexArrays(1, &_vao);
		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	}
	once = false;
}

int				ShaderProgram::getProgramId(void) const { return (this->_id); }

int				ShaderProgram::getFramebufferId(void) const { return (this->_framebufferId); }
void			ShaderProgram::setFramebufferId(int tmp) { this->_framebufferId = tmp; }

int				ShaderProgram::getRenderId(void) const { return (this->_renderId); }
void			ShaderProgram::setRenderId(int tmp) { this->_renderId = tmp; }

ShaderChannel	*ShaderProgram::getChannel(int index) {
	return (this->_channels + index);
}

std::ostream &	operator<<(std::ostream & o, ShaderProgram const & r)
{
	o << "tostring of the class" << std::endl;
	(void)r;
	return (o);
}
