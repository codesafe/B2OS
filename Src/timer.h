#ifndef TIMER_H
#define TIMER_H

extern unsigned long global_tick;
extern unsigned long global_seconds;

void k_init_timer(unsigned int freq);
void k_sleep(int ticks);

#endif