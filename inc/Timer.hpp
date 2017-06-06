#ifndef TIMER_HPP
# define TIMER_HPP
# include <iostream>
# include <string>
# include "shaderpixel.h"

class		Timer
{
	private:


	public:
		Timer();
		Timer(const Timer&);
		virtual ~Timer(void);

		Timer &	operator=(Timer const & src);

		static void	Timeout(Timeval *timeout, std::function< void(void) > callback);
};

std::ostream &	operator<<(std::ostream & o, Timer const & r);

#endif
