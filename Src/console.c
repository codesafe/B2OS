#include "console.h"

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


