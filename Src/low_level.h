#ifndef LOW_LEVEL_H
#define LOW_LEVEL_H


#define OUTB(port,val)                                                  \
do {                                                                    \
    asm volatile ("                                                     \
        outb %b1,(%w0)                                                  \
    " : /* no outputs */                                                \
      : "d" ((port)), "a" ((val))                                       \
      : "memory", "cc");                                                \
} while (0)

#define REP_OUTSB(port,source,count)                                    \
do {                                                                    \
    asm volatile ("                                                     \
     1: movb 0(%1),%%al                                                ;\
  outb %%al,(%w2)                                                ;\
  incl %1                                                        ;\
  decl %0                                                        ;\
  jne 1b                                                          \
    " : /* no outputs */                                                \
      : "c" ((count)), "S" ((source)), "d" ((port))                     \
      : "eax", "memory", "cc");                                         \
} while (0)

unsigned char port_byte_in(unsigned short port);
void port_byte_out(unsigned short port, unsigned char data);

unsigned short port_word_in(unsigned short port);
void port_word_out(unsigned short port, unsigned char data);


#endif 