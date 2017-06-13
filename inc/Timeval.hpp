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
	Timeval(const time_t sec, const suseconds_t usec);

	friend Timeval		operator*(const Timeval & rls, const int & mult);
	friend Timeval		operator+(const Timeval & rls, const Timeval & rhs);
	Timeval &	operator=(const Timeval & rhs);
	Timeval &	operator+=(const Timeval & rhs);

	virtual ~Timeval();
};

#endif
