#include "shaderpixel.h"
#include "ShaderChannel.hpp"
#include "ShaderProgram.hpp"

#define	IMAGE_EXT	(const char *[]){"jpg", "png", "tiff", "jpeg", "bmp", NULL}
#define	SHADER_EXT	(const char *[]){"glsl", "fs", NULL}
#define	SOUND_EXT	(const char *[]){"wav", "wave", NULL}

ShaderChannel::ShaderChannel(void)
{
	std::cout << "Default constructor of ShaderChannel called" << std::endl;
	this->_channelFile = "";
	this->_index = 0;
	this->_type = ShaderChannelType::CHANNEL_NONE;
	this->_program = NULL;
	this->_textureId = 0;
}

ShaderChannel::ShaderChannel(ShaderChannel const & src)
{
	*this = src;
	std::cout << "Copy constructor called" << std::endl;
}

ShaderChannel::~ShaderChannel(void) {}

bool		ShaderChannel::loadImage(const std::string & file, int mode)
{
	_type = ShaderChannelType::CHANNEL_IMAGE;

	int		flags = 0;

	if (mode & CHAN_NEAREST)
		flags |= SOIL_FLAG_NEAREST;
	else
		flags |= SOIL_FLAG_MIPMAPS;

	if (mode & CHAN_VFLIP)
		flags |= SOIL_FLAG_INVERT_Y;

	if (mode & CHAN_CLAMP)
		;
	else
		flags |= SOIL_FLAG_TEXTURE_REPEATS;

	flags |= SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT;

	if (_textureId)
		glDeleteTextures(1, (GLuint *)&_textureId);
	_textureId = SOIL_load_OGL_texture(file.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, flags);
	if (_textureId == 0)
		printf("can't load texture: %s\n", SOIL_last_result()), exit(-1);
	printf("loaded texture at id: %i\n", _textureId);

	return true;
}

bool		ShaderChannel::loadShader(const std::string & file, int mode)
{
	_type = ShaderChannelType::CHANNEL_PROGRAM;

	//TODO: reload shader in _program

	GLuint	fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

	GLuint renderedTexture;
	glGenTextures(1, &renderedTexture);
	glBindTexture(GL_TEXTURE_2D, renderedTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, framebuffer_size.x, framebuffer_size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//TODO: load mode link the image
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderedTexture, 0);

	if (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("renderbuffer error: %i\n", glGetError());
		puts("fbo status error !\n");
		return false;
	}

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	_program->setFramebufferId(fbo);
	_program->setRenderId(renderedTexture);

	return true;
}

bool		ShaderChannel::loadSound(const std::string & file)
{
	if (_textureId)
		glDeleteTextures(1, (GLuint *)&_textureId);

	//TODO
	//_textureId = load_wav_file(file);
	_type = ShaderChannelType::CHANNEL_SOUND;
	return (_textureId != -1);
}

bool		ShaderChannel::updateChannel(const std::string & file, int mode)
{
	_channelFile = file;

	const char *file_path = file.c_str();
	if (checkFileExtention(file_path, IMAGE_EXT))
		return (loadImage(file, mode));
	if (checkFileExtention(file_path, SHADER_EXT))
		return (loadShader(file, mode));
	if (checkFileExtention(file_path, SOUND_EXT))
		return (loadSound(file));
	return false;
}


ShaderChannel &	ShaderChannel::operator=(ShaderChannel const & src)
{
	std::cout << "Assignment operator called" << std::endl;

	if (this != &src) {
		this->_channelFile = src.getChannelFile();
		this->_index = src.getIndex();
		this->_type = src.getType();
		this->_program = src.getProgram();
		this->_textureId = src.getTextureId();
		this->_loaded = src.getLoaded();
	}
	return (*this);
}

std::string			ShaderChannel::getChannelFile(void) const { return (this->_channelFile); }

int					ShaderChannel::getIndex(void) const { return (this->_index); }
void				ShaderChannel::setIndex(int tmp) { this->_index = tmp; }

ShaderChannelType	ShaderChannel::getType(void) const { return (this->_type); }

ShaderProgram *		ShaderChannel::getProgram(void) const { return (this->_program); }

int					ShaderChannel::getTextureId(void) const { return (this->_textureId); }

bool				ShaderChannel::getLoaded(void) const { return this->_loaded; }

std::ostream &	operator<<(std::ostream & o, ShaderChannel const & r)
{
	o << "tostring of the class" << std::endl;
	(void)r;
	return (o);
}
