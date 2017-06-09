#ifndef TIMER_HPP
# define TIMER_HPP
# include <iostream>
# include <string>
# include <map>
# include <thread>
# include "shaderpixel.h"

class		Timer
{
	private:
		static std::map< int, std::thread * >	_runningThreads;
		static int								_localThreadIndex;

	public:
		Timer(void) = delete;
		Timer(const Timer&) = delete;
		virtual ~Timer(void);

		Timer &	operator=(Timer const & src) = delete;

		static void	Timeout(const Timeval *timeout, std::function< void(void) > callback);
};

std::ostream &	operator<<(std::ostream & o, Timer const & r);

#endif
