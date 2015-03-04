#ifndef TIME_H_
# define TIME_H_

struct timeval {
	time_t tv_sec; /* seconds */
	suseconds_t tv_usec; /* microseconds */
};

struct timezone {
	int tz_minuteswest; /* minutes west of Greenwich */
	int tz_dsttime;
};

struct itimerval {
	struct timeval it_interval;
	struct timeval it_value;
};

struct timespec {
	time_t tv_sec; /* seconds */
	long tv_nsec; /* nanoseconds */
};

# define ITIMER_REAL		01
# define ITIMER_VIRTUAL		02
# define ITIMER_PROF		04

struct itimerspec {
	struct timespec it_interval; /* Timer interval */
	struct timespec it_value; /* Initial expiration */
};

#endif /* !TIME_H_ */
