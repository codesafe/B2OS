C_SOURCES = $(wildcard Src/*.c drivers/*.c cpu/*.c libc/*.c)
HEADERS = $(wildcard Src/*.h drivers/*.h cpu/*.h libc/*.h)
# Nice syntax for file extension replacement
OBJ = ${C_SOURCES:.c=.o cpu/interrupt.o} 

# Change this if your cross-compiler is somewhere else
CC = /usr/local/i386elfgcc/bin/i386-elf-gcc
GDB = /usr/local/i386elfgcc/bin/i386-elf-gdb
# -g: Use debugging symbols in gcc
CFLAGS = -g -ffreestanding -Wall -Wextra -fno-exceptions -m32

# First rule is run by default
B2OS.bin: Out/bootsect.bin kernel.bin
	cat $^ > B2OS.bin

# '--oformat binary' deletes all symbols as a collateral, so we don't need
# to 'strip' them manually on this case
kernel.bin: Out/kernel_entry.o ${OBJ}
	i386-elf-ld -o $@ -Ttext 0x1000 $^ --oformat binary

# Used for debugging purposes
kernel.elf: Out/kernel_entry.o ${OBJ}
	i386-elf-ld -o $@ -Ttext 0x1000 $^ 

run: B2OS.bin
	qemu-system-i386 -fda B2OS.bin

# Open the connection to qemu and load our kernel-object file with symbols
#debug: B2OS.bin kernel.elf
#	qemu-system-i386 -s -fda B2OS.bin -d guest_errors,int &
#	${GDB} -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"

# Generic rules for wildcards
# To make an object, always compile from its .c
%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -c $< -o $@

%.o: %.asm
	nasm $< -f elf -o $@

%.bin: %.asm
	nasm $< -f bin -o $@

clean:
	rm -rf *.bin *.dis *.o B2OS.bin *.elf
	rm -rf Out/*.o Out/*.bin