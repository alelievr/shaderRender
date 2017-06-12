#ifndef TIMER_HPP
# define TIMER_HPP
# include "shaderpixel.h"
# include "Timeval.hpp"
# include <iostream>
# include <string>
# include <map>
# include <thread>

class		Timer
{
	private:
		static std::map< int, std::thread * >	_runningThreads;
		static int								_localThreadIndex;
		static bool								_threadsShouldStop;

	public:
		Timer(void) = delete;
		Timer(const Timer&) = delete;
		virtual ~Timer(void);

		Timer &	operator=(Timer const & src) = delete;

		static void			Timeout(const Timeval *timeout, std::function< void(void) > callback);
		static void			Interval(std::function< void(void) > callback, const int milliSecs, const int blockingUntilLoopCount = 0);
		static void			ExitAllThreads(void);

		static Timeval		*TimeoutInSeconds(const int nSecs);
		static Timeval		*TimeoutInMilliSeconds(const long nMillis);
		static Timeval		*TimeoutInMicroSeconds(const long nMicro);
		static Timeval		*Now(void);
		static char			*ReadableTime(const Timeval & t);
};

std::ostream &	operator<<(std::ostream & o, Timer const & r);

#endif
