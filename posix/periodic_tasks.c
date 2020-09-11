#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>

#include "periodic_tasks.h"

struct periodic_task {
  struct timespec r;
  int period;
};

#define NSEC_PER_SEC 1000000000ULL
static inline void timespec_add_us(struct timespec *t, uint64_t d)
{
    d *= 1000;
    d += t->tv_nsec;
    while (d >= NSEC_PER_SEC) {
        d -= NSEC_PER_SEC;
	t->tv_sec += 1;
    }
    t->tv_nsec = d;
}

void wait_next_activation(struct periodic_task *t)
{
    clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &t->r, NULL);
    timespec_add_us(&t->r, t->period);
}

struct periodic_task *start_periodic_timer(uint64_t offs, int t)
{
    struct periodic_task *p;

    p = malloc(sizeof(struct periodic_task));
    if (p == NULL) {
        return NULL;
    }

    clock_gettime(CLOCK_REALTIME, &p->r);
    timespec_add_us(&p->r, offs);
    p->period = t;

    return p;
}

