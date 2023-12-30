#include "console.h"

unsigned char *consolemem = 0;

void kinit_console()
{
  consolemem = (unsigned char *) VIDEO_ADDRESS;
}

void kputc(char c, int x, int y)
{
  if( *consolemem == 0 ) return;
  
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
      if( c == '\n')
        break;
       kputc(c, x+len, y);
       len ++;
    }
}

int kget_offset(int x, int y)
{
  return 2 * (y * MAX_X + x);
}


