#include "Timer.hpp"
#include <unistd.h>
#include <sys/time.h>

std::map< int, std::thread * >	Timer::_runningThreads;
int								Timer::_localThreadIndex;

Timer::~Timer(void)
{
	std::cout << "Destructor of Timer called" << std::endl;
}

void	Timer::Timeout(const Timeval *timeout, std::function< void(void) > callback)
{
	_runningThreads[_localThreadIndex] = new std::thread(
		[callback, timeout](void)
		{
			int			threadIndex = _localThreadIndex;
			Timeval		now;
			gettimeofday(&now, NULL);
			long		secDiff = (timeout->tv_sec - now.tv_sec) * 1000 * 1000;
			long		microSecDiff = timeout->tv_usec - now.tv_usec;
			long		total = secDiff + microSecDiff;
			if (total > 0)
				usleep(total);
			callback();
			_runningThreads.erase(threadIndex);
		}
	);
	_localThreadIndex++;
}
