#ifndef TIMER_H
#define TIMER_H

#include <sys/time.h>

#ifdef __cplusplus
extern "C" {
#endif

double getMsDifference(struct timeval t);


#ifdef __cplusplus
}
#endif

#endif /* TIMER_H */

