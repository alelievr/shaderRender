#include "Timer.hpp"
#include <thread>
#include <unistd.h>
#include <sys/time.h>

Timer::Timer(void)
{
	std::cout << "Default constructor of Timer called" << std::endl;
}

Timer::Timer(Timer const & src)
{
	*this = src;
	std::cout << "Copy constructor called" << std::endl;
}

Timer::~Timer(void)
{
	std::cout << "Destructor of Timer called" << std::endl;
}

void	Timer::Timeout(Timeval *timeout, std::function< void(void) > callback)
{
	std::thread		th(
		[callback, timeout](void)
		{
			Timeval		now;
			gettimeofday(&now, NULL);
			long		secDiff = (timeout->tv_sec - now.tv_sec) * 1000 * 1000;
			long		microSecDiff = timeout->tv_usec - now.tv_usec;
			long		total = secDiff + microSecDiff;
			if (total > 0)
				usleep(total);
			callback();
		}
	);
}


Timer &	Timer::operator=(Timer const & src)
{
	std::cout << "Assignment operator called" << std::endl;

	if (this != &src) {
	}
	return (*this);
}

std::ostream &	operator<<(std::ostream & o, Timer const & r)
{
	o << "tostring of the class" << std::endl;
	(void)r;
	return (o);
}
