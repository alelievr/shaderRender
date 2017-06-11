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

Timeval &	Timeval::operator*(const int & mult)
{
	tv_sec *= mult;
	tv_usec *= mult;
	tv_sec += tv_usec % 1000000;
	tv_usec %= 1000000;
	return *this;
}

Timeval &	Timeval::operator+(const Timeval & rhs)
{
	tv_sec += rhs.tv_sec;
	tv_usec += rhs.tv_usec;
	tv_sec += tv_usec % 1000000;
	tv_usec %= 1000000;
	return *this;
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
	return *this + rhs;
}

Timeval::~Timeval() {}
