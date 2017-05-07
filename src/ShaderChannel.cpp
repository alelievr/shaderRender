#include "ShaderChannel.hpp"


ShaderChannel::ShaderChannel(void)
{
	std::cout << "Default constructor of ShaderChannel called" << std::endl;
	this->_channelFile = "";
	this->_index = 0;
	this->_type = ShaderChanneltype::CHANNEL_NONE;
	this->_program = NULL;
	this->_textureId = 0;
}

ShaderChannel::ShaderChannel(ShaderChannel const & src)
{
	*this = src;
	std::cout << "Copy constructor called" << std::endl;
}

ShaderChannel::~ShaderChannel(void)
{
	std::cout << "Destructor of ShaderChannel called" << std::endl;
}

void		ShaderChannel::loadImage(const std::string & file, int mode)
{

	_type = ShaderChannelType::CHANNEL_IMAGED;
}

void		ShaderChannel::loadShader(const std::string & file, int mode)
{
	_type = ShaderChannelType::CHANNEL_SHADER;

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
		puts("fbo status error !"), exit(-1);
	}

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	_program->_framebufferId = fbo;
	_program->_renderId = renderedTexture;
}

void		ShaderChannel::loadSound(const std::string & file)
{
	if (chan->id)
		glDeleteTextures(1, (GLuint *)&chan->id);

	chan->id = load_wav_file(file);
	_type = ShaderChannelType::CHANNEL_SOUND;
}

bool		ShaderChannel::updateChannel(const std::string & file, int mode)
{
	_channelFile = file;

	const char *file_path = file.c_str();
	if (checkFileExtention(file, IMAGE_EXT))
		return (loadImage(file, mode));
	if (checkFileExtention(file, SHADER_EXT))
		return (loadShader(file, mode));
	if (checkFileExtention(file, SOUND_EXT))
		return (loadSound(file));
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
