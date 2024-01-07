#include "console.h"
#include "util.h"
#include "memory.h"

unsigned char *consolemem = 0;
unsigned char cursorpos = 0;

void k_init_console()
{
  consolemem = (unsigned char *) VIDEO_ADDRESS;
}

void k_clear_console()
{
  for(int x=0; x<MAX_X; x++)
    for(int y=0; y<MAX_Y; y++)
      k_putc(' ', x, y);

  cursorpos = 0;
}

void k_putc(char c, int x, int y)
{
  if( *consolemem == 0 ) return;
  //unsigned char *consolemem = (unsigned char *) VIDEO_ADDRESS;
  
  int offset = k_get_offset(x, y);
  consolemem[offset] = c;
  consolemem[offset+1] = CONSOLE_WHITE;
}

void k_scrollup()
{
    for(int i=0; i<MAX_Y-1; i++)
    {
      // line(N+1) -> line(N)
      memcpy(consolemem + (i*MAX_X*2), consolemem + ((i+1)*MAX_X*2) , MAX_X *2);
    }
} 

void k_print(char *str)
{
  if( cursorpos >= MAX_Y-1)
  {
      k_scrollup();
      cursorpos = MAX_Y-1;
      k_prinxy(str, 0, cursorpos);
      return;
  }

  k_prinxy(str, 0, cursorpos);
  cursorpos++;
}

void k_prinxy(char *str, int x, int y)
{
    int len = 0;
    while(1)
    {
      char c = str[len];
      if(c == '\n' || c == '\0')
        break;
      k_putc(c, x+len, y);
      len++;
    }
}

int k_get_offset(int x, int y)
{
  return 2 * (y * MAX_X + x);
}

int k_num2ascii(unsigned long n, char str[]) 
{
    char temp[64] = {0,};
    int i = 0;
    do 
    {
        temp[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);

    temp[i] = '\0';

    reverse(temp, str, i);
    return i;
}
