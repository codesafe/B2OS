//#define TIMER_LOG
#include "predef.h"
#include "timer.h"
#include "low_level.h"
#include "console.h"
#include "isr.h"
#include "util.h"

unsigned long global_tick = 0;
unsigned long global_seconds = 0;

static void timer_callback(registers_t regs) 
{
    global_tick++;
#ifdef TIMER_LOG
    char tick_ascii[20] = {0,};
    kint2ascii(global_tick, tick_ascii);
    kprint(tick_ascii, 5, 20);
#endif
	if (global_tick % 1000 == 0)
	{
		global_seconds++;
#ifdef TIMER_LOG        
        char sstr[20] = {0,};
        kint2ascii(global_seconds, sstr);
        kprint(sstr, 5, 21);
#endif            
	}
}

void kinitTimer(unsigned int freq)
{
    // Timer는 IRQ0 이용 
    // timer_callback IRQ0에 설치
    register_interrupt_handler(IRQ0, timer_callback);

/*
    // PIT reload value 설정
    // divisor가 0이 되면 reload 됨
    unsigned int divisor = 1193180 / freq;
    unsigned char low = (unsigned char)(divisor & 0xFF);
    unsigned char high = (unsigned char)((divisor >> 8) & 0xFF);

    // 명령 전송
    port_byte_out(0x43, 0x36); // Set our command byte 0x36
    // 0x40: Channel 0 data port
    // PIT reload value의 하위 byte
    port_byte_out(0x40, low);
    // PIT reload value의 상위 byte
    port_byte_out(0x40, high);
*/

    // oscillator는 약 1.193180 MHz로 작동
	int divisor = 1193180 / freq;	   /* Calculate our divisor */
    divisor = 110;
	port_byte_out(0x43, 0x36);			 /* Set our command byte 0x36 */
	port_byte_out(0x40, divisor & 0xFF);   /* Set low byte of divisor */
	port_byte_out(0x40, divisor >> 8);	 /* Set high byte of divisor */

}

void ksleep(int waitticks)
{
    int startTicks = global_tick;
    while(global_tick < startTicks + waitticks){}
    return;
}