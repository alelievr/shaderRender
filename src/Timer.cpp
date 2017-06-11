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

Timeval		*Timer::TimeoutInSeconds(const int nSecs)
{
	static Timeval	t;

	gettimeofday(&t, NULL);
	t.tv_sec += nSecs;
	return &t;
}

Timeval		*Timer::TimeoutInMilliSeconds(const long nMillis)
{
	static Timeval	t;

	gettimeofday(&t, NULL);
	t.tv_usec += nMillis * 1000;
	return &t;
}

Timeval		*Timer::TimeoutInMicroSeconds(const long nMicro)
{
	static Timeval	t;

	gettimeofday(&t, NULL);
	t.tv_usec += nMicro;
	return &t;
}

Timeval		*Timer::Now(void)
{
	static Timeval	t;

	gettimeofday(&t, NULL);
	return &t;
}

char		*Timer::ReadableTime(const Timeval & tv)
{
	time_t			nowtime;
	struct tm *		nowtm;
	static char		tmbuf[64], buf[64];

	nowtime = tv.tv_sec;
	nowtm = localtime(&nowtime);
	strftime(tmbuf, sizeof tmbuf, "%Y-%m-%d %H:%M:%S", nowtm);
	snprintf(buf, sizeof buf, "%s.%06ld", tmbuf, tv.tv_usec);
	return buf;
}
