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
#include "RenderShader.hpp"

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
RenderShader		*renderShader;

class ShaderRenderApplication : public nanogui::Screen
{
	public:

    	ShaderRenderApplication() : nanogui::Screen(Eigen::Vector2i(1024, 768), "NanoGUI Test")
		{
        	using namespace nanogui;

			screen = this;
			renderShader = &this->mRenderShader;
        	Window *mainGUI = new Window(this, "Button demo");
        	mainGUI->setPosition(Vector2i(15, 15));
        	mainGUI->setLayout(new GroupLayout());

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


        	/* No need to store a pointer, the data structure will be automatically
           	   freed when the parent window is deleted */
        	new Label(mainGUI, "Push buttons", "sans-bold");

        	Button *b = new Button(mainGUI, "Plain button");
        	b->setCallback([] { cout << "pushed!" << endl; });
        	b->setTooltip("short tooltip");

        	/* Alternative construction notation using variadic template */
        	b = mainGUI->add<Button>("Styled", ENTYPO_ICON_ROCKET);
        	b->setBackgroundColor(Color(0, 0, 255, 25));
        	b->setCallback([] { cout << "pushed!" << endl; });
        	b->setTooltip("This button has a fairly long tooltip. It is so long, in "
                	"fact, that the shown text will span several lines.");

			performLayout();
    	}

    	~ShaderRenderApplication() {}

    	virtual void	draw(NVGcontext *ctx)
		{
        	/* Draw the user interface */
        	Screen::draw(ctx);
    	}

    	virtual void	drawContents()
		{
			mRenderShader.render();
    	}

		virtual bool	dropEvent(const std::vector<std::string> & /* filenames */) {
			//TODO
			return false;
		}

	private:
    	nanogui::ProgressBar	*mProgress;
    	nanogui::GLShader		mShader;
		RenderShader			mRenderShader;

    	int mCurrentImage;
};

int		main(int /* argc */, char ** /* argv */)
{
    try {
        nanogui::init();

        nanogui::ref<ShaderRenderApplication> app = new ShaderRenderApplication();
        app->drawAll();
        app->setVisible(true);
        nanogui::mainloop();

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
