#include "Timeval.hpp"

Timeval::Timeval(void)
{
	tv_sec = 0;
	tv_usec = 0;
}

Timeval::Timeval(const Timeval & rhs)
{
	*this = rhs;
}

Timeval::Timeval(const struct timeval & rhs)
{
	tv_sec = rhs.tv_sec;
	tv_usec = rhs.tv_usec;
}

Timeval::Timeval(const time_t sec, const suseconds_t usec)
{
	tv_sec = sec;
	tv_usec = usec;
	tv_sec += tv_usec / 1000000;
	tv_usec %= 1000000;
}

Timeval		operator*(const Timeval & t, const int & mult)
{
	return Timeval(t.tv_sec * mult, t.tv_usec * mult);
}

Timeval		operator+(const Timeval & rls, const Timeval & rhs)
{
	return Timeval(rls.tv_sec + rhs.tv_sec, rls.tv_usec + rhs.tv_usec);
}

Timeval &	Timeval::operator=(const Timeval & rhs)
{
	if (this != &rhs)
	{
		tv_sec = rhs.tv_sec;
		tv_usec = rhs.tv_usec;
	}
	return *this;
}

Timeval &	Timeval::operator+=(const Timeval & rhs)
{
	Timeval	tv = *this + rhs;
	tv_sec = tv.tv_sec;
	tv_usec = tv.tv_usec;
	return *this;
}

Timeval::~Timeval() {}
