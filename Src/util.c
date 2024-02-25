#include "util.h"
#include "console.h"

////////////////////////////////////////////////////////////////////////////////

unsigned long int k_random_next = 1;

void k_srand(unsigned int seed)
{
    k_random_next = seed;
	printf("[INIT RANDOM]\n");
}

int k_rand(void)
{
    k_random_next = k_random_next * 1103515245 + 12345;
    return (unsigned int)(k_random_next/65536) % 32768;
}

////////////////////////////////////////////////////////////////////////////////


char* reverse(const char* str, char* buffer, int len)
{
	int i;
	for (i = 0; i <= len; i++)
		buffer[i] = str[len - 1 - i];
	//buffer[i] = '\0';
	return buffer;
}

// wrap-around를 막자
unsigned char safeDec(unsigned char src, char value)
{
	unsigned char result = src - value;
	//result = result > src ? 0 : result;
	return result;
}

unsigned char safeInc(unsigned char src, char value)
{
	unsigned char result = src + value;
	//result = result < src ? 0xFF : result;
	return result;
}
