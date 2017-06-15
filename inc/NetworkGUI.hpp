#ifndef NETWORKGUI_HPP
# define NETWORKGUI_HPP
# include "NetworkManager.hpp"
# include "SFML/Window.hpp"
# include "SFML/Graphics.hpp"
# include <iostream>
# include <string>

# define WINDOW_WIDTH	1920
# define WINDOW_HEIGHT	1080

class		NetworkGUI
{
	private:
		sf::RenderWindow	*_win;
		sf::Sprite			*_placeSprite;
		sf::Vector2< int >	_mousePosition;

	public:
		NetworkGUI(void) = delete;
		NetworkGUI(NetworkManager *nm);
		NetworkGUI(const NetworkGUI &) = delete;
		virtual ~NetworkGUI(void);

		NetworkGUI &	operator=(NetworkGUI const & src) = delete;

		void	RenderLoop(void);
};

#endif
