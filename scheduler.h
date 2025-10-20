#ifndef SCHEDULER_H
#define	SCHEDULER_H

#include "types.h"

tcb_t *rr_scheduler(void);
tcb_t *priority_scheduler(void);
void scheduler(void);

#endif	/* SCHEDULER_H */

