#include <stdlib.h>
#include <sys/time.h>


double getMsDifference(struct timeval t) {
	struct timeval current;
	gettimeofday(&current, NULL);
	double diff = (current.tv_sec  - t.tv_sec) * 1000.0;
	return diff + (current.tv_usec - t.tv_usec) / 1000.0;
}

