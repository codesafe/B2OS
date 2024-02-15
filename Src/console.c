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
    memset(consolemem + ((MAX_Y-1)*MAX_X*2), 0 ,MAX_X);
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

void k_printn(char *str1, char *str2, char *str3 )
{
  int pos = 0;
  char temp[80]={0,};

  if( str1 != NULL )
  {
      int len = k_strlen(str1);
      memcpy(temp, str1, len);
      temp[len] = '|';
      pos+=len + 1;
  }

  if( str2 != NULL )
  {
      int len = k_strlen(str2);
      memcpy(temp + pos, str2, len);
      temp[len+pos] = '|';
      pos+=len + 1;
  }

  if( str3 != NULL )
  {
      int len = k_strlen(str3);
      memcpy(temp + pos, str3, len);
  }
  k_print(temp);
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

void k_printnum(unsigned long n)
{
  char temp[64] = {0,};
  k_num2ascii(n, temp);
  k_print(temp);
}

int k_strlen(char *str)
{
  int len = 0;
  while(1)
  {
    if( str[len] == 0 || str[len] == '\0' || str[len] == '\n')
      break;
    len++;
  }

  return len;
}