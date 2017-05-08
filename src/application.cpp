#include <nanogui/opengl.h>
#include <nanogui/glutil.h>
#include <nanogui/screen.h>
#include <nanogui/window.h>
#include <nanogui/layout.h>
#include <nanogui/label.h>
#include <nanogui/checkbox.h>
#include <nanogui/button.h>
#include <nanogui/toolbutton.h>
#include <nanogui/popupbutton.h>
#include <nanogui/combobox.h>
#include <nanogui/progressbar.h>
#include <nanogui/entypo.h>
#include <nanogui/messagedialog.h>
#include <nanogui/textbox.h>
#include <nanogui/slider.h>
#include <nanogui/imagepanel.h>
#include <nanogui/imageview.h>
#include <nanogui/vscrollpanel.h>
#include <nanogui/colorwheel.h>
#include <nanogui/graph.h>
#include <nanogui/tabwidget.h>
#include <iostream>
#include <string>
#include "ShaderRender.hpp"

// Includes for the GLTexture class.
#include <cstdint>
#include <memory>
#include <utility>

#if defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
#if defined(_WIN32)
#  pragma warning(push)
#  pragma warning(disable: 4457 4456 4005 4312)
#endif

#if defined(_WIN32)
#  pragma warning(pop)
#endif
#if defined(_WIN32)
#  if defined(APIENTRY)
#    undef APIENTRY
#  endif
#  include <windows.h>
#endif

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::vector;
using std::pair;
using std::to_string;

nanogui::Screen		*screen;
ShaderRender		*renderShader;

class ShaderRenderApplication : public nanogui::Screen
{
	public:

    	ShaderRenderApplication(char *shaderFile) : nanogui::Screen(Eigen::Vector2i(1024, 768), "NanoGUI Test")
		{
        	using namespace nanogui;

			screen = this;
			renderShader = &this->mShaderRender;

			//channel view infos allocs:
			//channelImages = new (ImageView *[MAX_CHANNEL_COUNT]);
			channelTitle = new (Label *[MAX_CHANNEL_COUNT]);
			channelTexts = new (Label *[MAX_CHANNEL_COUNT]);

			GLFWwindow *window = glfwWindow();

			glfwSetFramebufferSizeCallback(window,
				[](GLFWwindow *, int width, int height) {
					screen->resizeCallbackEvent(width, height);
					renderShader->framebufferSizeCallback(width, height);
				}
			);

			glfwSetWindowSizeCallback(window,
				[](GLFWwindow *, int w, int h) {
					screen->resizeCallbackEvent(w, h);
					renderShader->windowSizeCallback(w, h);
				}
			);

			glfwSetScrollCallback(window,
				[](GLFWwindow *, double x, double y) {
					screen->scrollCallbackEvent(x, y);
					renderShader->scrollCallback(x, y);
				}
			);

			glfwSetKeyCallback(window,
				[](GLFWwindow *win, int key, int scancode, int action, int mods) {
					screen->keyCallbackEvent(key, scancode, action, mods);
					renderShader->keyCallback(win, key, scancode, action, mods);
				}
			);

			glfwSetMouseButtonCallback(window,
				[](GLFWwindow *, int button, int action, int modifiers) {
					screen->mouseButtonCallbackEvent(button, action, modifiers);
					renderShader->clickCallback(button, action, modifiers);
				}
			);

			glfwSetCursorPosCallback(window,
            	[](GLFWwindow *win, double x, double y) {
            		screen->cursorPosCallbackEvent(x, y);
					renderShader->mousePositionCallback(win, x, y);
        		}
    		);

			int		fw, fh;
			glfwGetFramebufferSize(window, &fw, &fh);
			framebuffer_size.x = fw;
			framebuffer_size.y = fh;


        	Window *mainGUI = new Window(this, "INFO");
        	mainGUI->setPosition(Vector2i(15, 15));
        	mainGUI->setLayout(new GroupLayout());

        	fpsLabel = new Label(mainGUI, string("FPS: 0"), "sans-bold");

			for (int i = 0; i < MAX_CHANNEL_COUNT; i++)
			{
				channelTitle[i] = new Label(mainGUI, "channel " + to_string(i));
				//channelImages[i] = new ImageView(mainGUI, -1);
				channelTexts[i] = new Label(mainGUI, "unused");
				ShaderChannel	*c = renderShader->getChannel(i);
				if (c != NULL)
				{
					//file button
					auto b = new Button(mainGUI, "Open");
					b->setCallback([&] {
							string file = file_dialog({
								{"png", "Portable Network Graphics"},
								{"jpg", "Joint Photographic Experts Group"},
								{"tga", "Truevision Targa"}
							}, false);
							if (!file.empty())
							{
								//TODO: reload the channel
								/*c = renderShader->getChannel(i);
								renderShader->updateChannel(c, file.c_str(), 0);
								channelTexts[i]->setCaption(c->file_path);*/
							}
						}
					);
				}
			}
			updateChannelGUI(mainGUI);

			renderShader->attachShader(shaderFile);

			performLayout();
		}

		void			updateChannelGUI(nanogui::Window *mainGUI)
		{
        	using namespace nanogui;

			for (int i = 0; i < MAX_CHANNEL_COUNT; i++)
			{
				ShaderChannel *c = renderShader->getChannel(i);
				if (c == NULL)
					continue ;
				switch (c->getType())
				{
					case ShaderChannelType::CHANNEL_IMAGE:
						channelTexts[i]->setCaption(string("file: ") + c->getChannelFile());
						//channelImages[i]->bindImage(c->id);
						break ;
					case ShaderChannelType::CHANNEL_SOUND:
						channelTexts[i]->setCaption(string("file: ") + c->getChannelFile());
						break ;
					case ShaderChannelType::CHANNEL_PROGRAM:
						channelTexts[i]->setCaption(string("file: ") + c->getChannelFile());
						break ;
					default:
						break ;
				}
				//file button
			}
		}

    	~ShaderRenderApplication() {}

    	virtual void	draw(NVGcontext *ctx)
		{
        	Screen::draw(ctx);
    	}

		virtual void	drawContents()
		{
			if (mOldTime != time(NULL))
				fpsLabel->setCaption("FPS: " + to_string(mFrames)), mFrames = 0;
			renderShader->render(glfwWindow());
			mOldTime = time(NULL);
			mFrames++;
		}

		/*virtual bool	dropEvent(const std::vector<std::string> & filenames) {
			//TODO
			return false;
		}*/

	private:
    	nanogui::ProgressBar	*mProgress;
		ShaderRender			mShaderRender;
		nanogui::Label			*fpsLabel;
		//nanogui::ImageView		**channelImages;
		nanogui::Label			**channelTexts;
		nanogui::Label			**channelTitle;
		long					mOldTime = 0;
		int						mFrames = 0;
};

static void	usage(char *prog) __attribute__((noreturn));
static void	usage(char *prog)
{
	cout << "usage: " << prog << " <shader file>" << endl;
	exit(0);
}

int		main(int ac, char ** av)
{
	if (ac != 2)
		usage(av[0]);

    try {
        nanogui::init();

        nanogui::ref<ShaderRenderApplication> app = new ShaderRenderApplication(av[1]);
        app->drawAll();
        app->setVisible(true);
        nanogui::mainloop(-1);

    	nanogui::shutdown();
	} catch (const std::runtime_error &e) {
    	std::string error_msg = std::string("Caught a fatal error: ") + std::string(e.what());
#if defined(_WIN32)
    	MessageBoxA(nullptr, error_msg.c_str(), NULL, MB_ICONERROR | MB_OK);
#else
    	std::cerr << error_msg << endl;
#endif
    	return -1;
	}

	return 0;
}
