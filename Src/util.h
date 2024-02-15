#ifndef UTIL_H
#define UTIL_H

extern unsigned long int k_random_next;
void k_srand(unsigned int seed);
int k_rand(void);

char* reverse(const char* str, char* buffer, int len);

unsigned char safeDec(unsigned char src, char value);
unsigned char safeInc(unsigned char src, char value);


#endif