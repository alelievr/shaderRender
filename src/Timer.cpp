#include "Timer.hpp"
#include <unistd.h>
#include <sys/time.h>
#include <condition_variable>

std::map< int, std::thread * >	Timer::_runningThreads;
int								Timer::_localThreadIndex;
bool							Timer::_threadsShouldStop = false;

Timer::~Timer(void)
{
	std::cout << "Destructor of Timer called" << std::endl;
}

void	Timer::Timeout(const Timeval *timeout, std::function< void(void) > callback)
{
	_runningThreads[_localThreadIndex] = new std::thread(
		[timeout, callback](void)
		{
			int			threadIndex = _localThreadIndex;
			Timeval		now;
			gettimeofday(&now, NULL);
			long		secDiff = (timeout->tv_sec - now.tv_sec) * 1000 * 1000;
			long		microSecDiff = timeout->tv_usec - now.tv_usec;
			long		total = secDiff + microSecDiff;
			if (total > 0)
				usleep(total);
			if (!_threadsShouldStop)
				callback();
			printf("HERE !!!\n");
			_runningThreads.erase(threadIndex);
			printf("ool\n");
		}
	);
	_localThreadIndex++;
}

void	Timer::Interval(std::function< void(void) > callback, const int milliSecs, const int blockingUntilLoopCount)
{
	_runningThreads[_localThreadIndex] = new std::thread(
		[callback, milliSecs](void) mutable
		{
			int			threadIndex = _localThreadIndex;
			int			microSecs = milliSecs * 1000;

			std::unique_lock<std::mutex> lck(mtx);

			while (!_threadsShouldStop)
			{
				callback();
				cv.notify_all();
				usleep(microSecs);
			}
			_runningThreads.erase(threadIndex);
		}
	);
	//TODO: implemnt blocking count !!!
	sleep(1);
	_localThreadIndex++;
}

void	Timer::ExitAllThreads(void)
{
	_threadsShouldStop = true;
	for (const auto & threadKP : _runningThreads)
		threadKP.second->join();
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
	snprintf(buf, sizeof buf, "%s.%06d", tmbuf, tv.tv_usec);
	return buf;
}
