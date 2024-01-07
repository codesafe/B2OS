#ifndef CONSOLE_H
#define CONSOLE_H


#define CONSOLE_BLACK    0
#define CONSOLE_BLUE     1
#define CONSOLE_GREEN    2
#define CONSOLE_CYAN     3
#define CONSOLE_RED      4
#define CONSOLE_MAGENTA  5
#define CONSOLE_BROWN    6
#define CONSOLE_LIGHT_GREY   7
#define CONSOLE_DARK_GREY    8
#define CONSOLE_LIGHT_BLUE   9
#define CONSOLE_LIGHT_GREEN  10
#define CONSOLE_LIGHT_CYAN   11
#define CONSOLE_LIGHT_RED    12
#define CONSOLE_LIGHT_MAGENTA  13
#define CONSOLE_LIGHT_BROWN  14
#define CONSOLE_WHITE    15

#define VIDEO_ADDRESS 0xb8000
#define MAX_X 80
#define MAX_Y 25

void k_init_console();
void k_clear_console();
void k_putc(char c, int x, int y);
void k_print(char *str);
void k_prinxy(char *str, int x, int y);
int k_get_offset(int row, int col);
int k_num2ascii(unsigned long n, char str[]);

#endif