#include "util.h"


char* reverse(const char* str, char* buffer, int len)
{
	int i;
	for (i = 0; i <= len; i++)
		buffer[i] = str[len - 1 - i];
	//buffer[i] = '\0';
	return buffer;
}
