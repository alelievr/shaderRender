#include "ShaderProgram.hpp"


ShaderProgram::ShaderProgram(void)
{
	std::cout << "Default constructor of ShaderProgram called" << std::endl;
	this->_fragmentFile = "";
	this->_vertexFile = "";
	this->_id = 0;
	this->_framebufferId = 0;
	this->_render_id = 0;
	this->_channels = ;
	this->_fd = 0;
	this->_lastModified = 0;
}

ShaderProgram::ShaderProgram(ShaderProgram const & src)
{
	*this = src;
	std::cout << "Copy constructor called" << std::endl;
}

ShaderProgram::~ShaderProgram(void)
{
	std::cout << "Destructor of ShaderProgram called" << std::endl;
}

bool		ShaderProgram::compileAndLink(void)
{
	
}

void		ShaderProgram::Bind(void)
{
	
}


ShaderProgram &	ShaderProgram::operator=(ShaderProgram const & src)
{
	std::cout << "Assignment operator called" << std::endl;

	if (this != &src) {
		this->_fragmentFile = src.getFragmentFile();
		this->_vertexFile = src.getVertexFile();
		this->_id = src.getId();
		this->_framebufferId = src.getFramebufferId();
		this->_render_id = src.getRender_id();
		this->_channels = src.getChannels();
		this->_fd = src.getFd();
		this->_lastModified = src.getLastModified();
	}
	return (*this);
}

std::string		ShaderProgram::getFragmentFile(void) const { return (this->_fragmentFile); }
void		ShaderProgram::setFragmentFile(std::string tmp) { this->_fragmentFile = tmp; }

std::string		ShaderProgram::getVertexFile(void) const { return (this->_vertexFile); }
void		ShaderProgram::setVertexFile(std::string tmp) { this->_vertexFile = tmp; }

int		ShaderProgram::getId(void) const { return (this->_id); }
void		ShaderProgram::setId(int tmp) { this->_id = tmp; }

int		ShaderProgram::getFramebufferId(void) const { return (this->_framebufferId); }
void		ShaderProgram::setFramebufferId(int tmp) { this->_framebufferId = tmp; }

int		ShaderProgram::getRender_id(void) const { return (this->_render_id); }
void		ShaderProgram::setRender_id(int tmp) { this->_render_id = tmp; }

ShaderChannel		ShaderProgram::getChannels(void) const { return (this->_channels); }
void		ShaderProgram::setChannels(ShaderChannel tmp) { this->_channels = tmp; }

int		ShaderProgram::getFd(void) const { return (this->_fd); }
void		ShaderProgram::setFd(int tmp) { this->_fd = tmp; }

long		ShaderProgram::getLastModified(void) const { return (this->_lastModified); }
void		ShaderProgram::setLastModified(long tmp) { this->_lastModified = tmp; }

std::ostream &	operator<<(std::ostream & o, ShaderProgram const & r)
{
	o << "tostring of the class" << std::endl;
	(void)r;
	return (o);
}
