#ifndef SHADERCHANNEL_HPP
# define SHADERCHANNEL_HPP
# include <iostream>
# include <string>

# include "shaderpixel.h"

class		ShaderProgram;

enum class ShaderChannelType {
	CHANNEL_NONE,
	CHANNEL_IMAGE,
	CHANNEL_SOUND,
	CHANNEL_PROGRAM,
};

class		ShaderChannel
{
	private:
		std::string			_channelFile;
		int					_index;
		bool				_loaded;
		ShaderChannelType	_type;
		ShaderProgram *		_program;
		int					_textureId;
		int					_mode;

		bool				loadImage(const std::string & file, int mode);
		bool				loadShader(const std::string & file, int mode);
		bool				loadSound(const std::string & file);

	public:
		ShaderChannel();
		ShaderChannel(const ShaderChannel&);
		virtual ~ShaderChannel(void);

		ShaderChannel &	operator=(ShaderChannel const & src);

		bool				updateChannel(const std::string & file, int mode);
		void				updateChannelMode(bool active, int value);

		std::string			getChannelFile(void) const;
		
		int					getIndex(void) const;
		void				setIndex(int tmp);
		
		ShaderChannelType	getType(void) const;
		ShaderProgram *		getProgram(void) const;
		int					getTextureId(void) const;
		bool				getLoaded(void) const;
};

std::ostream &	operator<<(std::ostream & o, ShaderChannel const & r);

#endif
