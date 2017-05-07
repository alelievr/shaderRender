#ifndef SHADERPROGRAM_HPP
# define SHADERPROGRAM_HPP
# include <iostream>
# include <string>

class		ShaderProgram
{
	private:
		std::string		_fragmentFile;
		std::string		_vertexFile;
		int				_id;
		int				_framebufferId;
		int				_render_id;
		ShaderChannel	_channels;
		int				_fd;
		long			_lastModified;


	public:
		ShaderProgram(void);
		ShaderProgram(const ShaderProgram&);
		virtual ~ShaderProgram(void);

		ShaderProgram &	operator=(ShaderProgram const & src);

		bool	compileAndLink(void);

		void	Bind(void);

		std::string	getFragmentFile(void) const;
		void	setFragmentFile(std::string tmp);
		
		std::string	getVertexFile(void) const;
		void	setVertexFile(std::string tmp);
		
		int	getId(void) const;
		void	setId(int tmp);
		
		int	getFramebufferId(void) const;
		void	setFramebufferId(int tmp);
		
		int	getRender_id(void) const;
		void	setRender_id(int tmp);
		
		ShaderChannel	getChannels(void) const;
		void	setChannels(ShaderChannel tmp);
		
		int	getFd(void) const;
		void	setFd(int tmp);
		
		long	getLastModified(void) const;
		void	setLastModified(long tmp);
};

std::ostream &	operator<<(std::ostream & o, ShaderProgram const & r);

#endif
