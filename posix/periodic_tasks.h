#ifndef PERIODIC_TASK
#define PERIODIC_TASK

struct periodic_task;
struct periodic_task *start_periodic_timer(uint64_t offs, int t);
void wait_next_activation(struct periodic_task *t);

#endif	/* PERIODIC_TASK */

