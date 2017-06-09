#ifndef SHADER_APPLICATION
# define SHADER_APPLICATION
# include "shaderpixel.h"
# include "ShaderRender.hpp"

class ShaderApplication
{
	public:
		ShaderApplication(bool server = false);
		virtual ~ShaderApplication(void);

		bool	LoadShader(const std::string & shaderFile);
		void	SwapShaderRender(void);
		void	RenderLoop(void);

	private:
		ShaderRender		*currentShaderRender;
		ShaderRender		*bufferedShaderRender;
		GLFWwindow			*window;
};

#endif
