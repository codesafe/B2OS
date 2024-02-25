#include "console.h"
#include "util.h"
#include "memory.h"
#include "low_level.h"

uint16 vga_item_entry(uint8 ch, VGA_COLOR_TYPE fore_color, VGA_COLOR_TYPE back_color) 
{
    uint16 ax = 0;
    uint8 ah = 0, al = 0;

    ah = back_color;
    ah <<= 4;
    ah |= fore_color;
    ax = ah;
    ax <<= 8;
    al = ch;
    ax |= al;

    return ax;
}

void vga_set_cursor_pos(uint8 x, uint8 y) 
{
    uint16 cursorLocation = y * 80 + x;
    port_byte_out(0x3D4, 14);
    port_byte_out(0x3D5, cursorLocation >> 8);
    port_byte_out(0x3D4, 15);
    port_byte_out(0x3D5, cursorLocation);
}

void vga_disable_cursor() 
{
    port_byte_out(0x3D4, 10);
    port_byte_out(0x3D5, 32);
}

/////////////////////////////////////////////////////////////////////////////////

static uint16 *g_vga_buffer;
static uint32 g_vga_index;
static uint8 cursor_pos_x = 0, cursor_pos_y = 0;
uint8 g_fore_color = COLOR_WHITE, g_back_color = COLOR_BLACK;

void console_init(VGA_COLOR_TYPE fore_color, VGA_COLOR_TYPE back_color) 
{
    g_vga_buffer = (uint16 *)VGA_ADDRESS;
    g_fore_color = fore_color;
    g_back_color = back_color;
    cursor_pos_x = 0;
    cursor_pos_y = 0;
    console_clear(fore_color, back_color);
}

void console_clear(VGA_COLOR_TYPE fore_color, VGA_COLOR_TYPE back_color) 
{
    uint32 i;
    for (i = 0; i < VGA_TOTAL_ITEMS; i++) 
        g_vga_buffer[i] = vga_item_entry(0, fore_color, back_color);

    g_vga_index = 0;
    cursor_pos_x = 0;
    cursor_pos_y = 0;
    vga_set_cursor_pos(cursor_pos_x, cursor_pos_y);
}

static void console_newline() 
{
    if (cursor_pos_y >= VGA_HEIGHT) {
        cursor_pos_x = 0;
        cursor_pos_y = 0;
        console_clear(g_fore_color, g_back_color);
    } else {
        g_vga_index += VGA_WIDTH - (g_vga_index % VGA_WIDTH);
        cursor_pos_x = 0;
        ++cursor_pos_y;
        vga_set_cursor_pos(cursor_pos_x, cursor_pos_y);
    }
}

void console_putchar(char ch) 
{
    if (ch == ' ') 
    {
        g_vga_buffer[g_vga_index++] = vga_item_entry(' ', g_fore_color, g_back_color);
        vga_set_cursor_pos(cursor_pos_x++, cursor_pos_y);
    }
    if (ch == '\t') 
    {
        for(int i = 0; i < 4; i++) 
        {
            g_vga_buffer[g_vga_index++] = vga_item_entry(' ', g_fore_color, g_back_color);
            vga_set_cursor_pos(cursor_pos_x++, cursor_pos_y);
        }
    } else if (ch == '\n') 
    {
        console_newline();
    } 
    else 
    {
        if (ch > 0) 
        {
            g_vga_buffer[g_vga_index++] = vga_item_entry(ch, g_fore_color, g_back_color);
            vga_set_cursor_pos(++cursor_pos_x, cursor_pos_y);
        }
    }
}

void console_ungetchar() 
{
    if(g_vga_index > 0) {
        g_vga_buffer[g_vga_index--] = vga_item_entry(0, g_fore_color, g_back_color);
        if(cursor_pos_x > 0) {
            vga_set_cursor_pos(cursor_pos_x--, cursor_pos_y);
        } else {
            cursor_pos_x = VGA_WIDTH;
            if (cursor_pos_y > 0)
                vga_set_cursor_pos(cursor_pos_x--, --cursor_pos_y);
            else
                cursor_pos_y = 0;
        }
    }
    g_vga_buffer[g_vga_index] = vga_item_entry(0, g_fore_color, g_back_color);
}

void console_ungetchar_bound(uint8 n) 
{
    if(((g_vga_index % VGA_WIDTH) > n) && (n > 0)) 
    {
        g_vga_buffer[g_vga_index--] = vga_item_entry(0, g_fore_color, g_back_color);
        if(cursor_pos_x >= n) 
        {
            vga_set_cursor_pos(cursor_pos_x--, cursor_pos_y);
        } 
        else 
        {
            cursor_pos_x = VGA_WIDTH;
            if (cursor_pos_y > 0)
                vga_set_cursor_pos(cursor_pos_x--, --cursor_pos_y);
            else
                cursor_pos_y = 0;
        }
    }
    g_vga_buffer[g_vga_index] = vga_item_entry(0, g_fore_color, g_back_color);
}

void console_gotoxy(uint16 x, uint16 y) 
{
    g_vga_index = (80 * y) + x;
    cursor_pos_x = x;
    cursor_pos_y = y;
    vga_set_cursor_pos(cursor_pos_x, cursor_pos_y);
}

void console_putstr(const char *str) 
{
    uint32 index = 0;
    while (str[index]) {
        if (str[index] == '\n')
            console_newline();
        else
            console_putchar(str[index]);
        index++;
    }
}

uint32 digit_count(int num)
{
  uint32 count = 0;
  if(num == 0)
    return 1;
  while(num > 0){
    count++;
    num = num/10;
  }
  return count;
}

void itoa(char *buf, int base, int d) 
{
    char *p = buf;
    char *p1, *p2;
    unsigned long ud = d;
    int divisor = 10;

    if (base == 'd' && d < 0) 
    {
        *p++ = '-';
        buf++;
        ud = -d;
    } 
    else if (base == 'x')
        divisor = 16;

    do 
    {
        int remainder = ud % divisor;
        *p++ = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
    } while (ud /= divisor);

    *p = 0;
    p1 = buf;
    p2 = p - 1;
    while (p1 < p2) 
    {
        char tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
        p1++;
        p2--;
    }
}

void printf(const char *format, ...) 
{
    char **arg = (char **)&format;
    int c;
    char buf[32];

    arg++;
    memset(buf, 0, sizeof(buf));
    while ((c = *format++) != 0) 
    {
        if (c != '%')
            console_putchar(c);
        else 
        {
            char *p, *p2;
            int pad0 = 0, pad = 0;

            c = *format++;
            if (c == '0') 
            {
                pad0 = 1;
                c = *format++;
            }

            if (c >= '0' && c <= '9') 
            {
                pad = c - '0';
                c = *format++;
            }

            switch (c) 
            {
                case 'd':
                case 'u':
                case 'x':
                    itoa(buf, c, *((int *)arg++));
                    p = buf;
                    goto string;
                    break;

                case 's':
                    p = *arg++;
                    if (!p)
                        p = "(null)";

                string:
                    for (p2 = p; *p2; p2++)
                        ;
                    for (; p2 < p + pad; p2++)
                        console_putchar(pad0 ? '0' : ' ');
                    while (*p)
                        console_putchar(*p++);
                    break;

                default:
                    console_putchar(*((int *)arg++));
                    break;
            }
        }
    }
}

