#ifndef TIMER_H
#define	TIMER_H

#include "config.h"
#include "scheduler.h"
#include "kernel.h"


void configTimer0(void);
void __interrupt() ISR_TIMER(void);
void release_task_delay(void);

#endif	/* TIMER_H */

