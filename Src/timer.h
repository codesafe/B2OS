#ifndef TIMER_H
#define TIMER_H

extern unsigned long global_tick;
extern unsigned long global_seconds;

void kinitTimer(unsigned int freq);
void ksleep(int ticks);

#endif