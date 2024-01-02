#include "console.h"
#include "util.h"

//unsigned char *consolemem = 0;

void kinit_console()
{
  //consolemem = (unsigned char *) VIDEO_ADDRESS;
}

void kclear_console()
{
  for(int x=0; x<MAX_X; x++)
    for(int y=0; y<MAX_Y; y++)
      kputc(' ', x, y);
}

void kputc(char c, int x, int y)
{
  //if( *consolemem == 0 ) return;
  unsigned char *consolemem = (unsigned char *) VIDEO_ADDRESS;
  
  int offset = kget_offset(x, y);
  consolemem[offset] = c;
  consolemem[offset+1] = CONSOLE_WHITE;
}

void kprint(char *str, int x, int y)
{
    int len = 0;
    while(1)
    {
      char c = str[len];
      if( c == '\n' || c == '\0')
        break;
       kputc(c, x+len, y);
       len ++;
    }
}

int kget_offset(int x, int y)
{
  return 2 * (y * MAX_X + x);
}

int kint2ascii(unsigned long n, char str[]) 
{
    char temp[64] = {0,};
    int i = 0;
    do 
    {
        temp[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);

    temp[i] = '\n';

    reverse(temp, str, i);
    return i;
}
