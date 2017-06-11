#ifndef TIMEVAL_HPP
# define TIMEVAL_HPP
# include <iostream>
# include <string>
# include <sys/time.h>

typedef struct timeval	sTimeval;

struct	Timeval : sTimeval
{
	Timeval(void);

	Timeval(const Timeval & rhs);
	Timeval(const struct timeval & rhs);

	Timeval &	operator*(const int & mult);
	Timeval &	operator+(const Timeval & rhs);
	Timeval &	operator=(const Timeval & rhs);
	Timeval &	operator+=(const Timeval & rhs);

	virtual ~Timeval();
};

#endif
