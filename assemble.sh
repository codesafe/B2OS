clear
rm -fr *.bin
rm -fr  Out/*.o Out/*.bin

export PATH=$PATH:/usr/local/i386elfgcc/bin
# Compile assembly code
# bin -> bin
nasm "Asm/boot.asm" -f bin -o "Out/boot.bin"

# Compile C entry code
# o -> elf format
nasm "Asm/interrupt.asm" -f elf -o "Out/interrupt.o"
nasm "Asm/kernel_entry.asm" -f elf -o "Out/kernel_entry.o"
#nasm "Asm/zeroes.asm" -f bin -o "Out/zeroes.bin"


# Compile C Kernel

#i386-elf-gcc -ffreestanding -m32 -g -c "Src/screen.c" -o "Out/screen.o"
i386-elf-gcc -ffreestanding -m32 -c "Src/console.c" -o "Out/console.o"
i386-elf-gcc -ffreestanding -m32 -c "Src/low_level.c" -o "Out/low_level.o"
i386-elf-gcc -ffreestanding -m32 -c "Src/vga.c" -o "Out/vga.o"
i386-elf-gcc -ffreestanding -m32 -c "Src/kernel.c" -o "Out/kernel.o"
i386-elf-gcc -ffreestanding -m32 -c "Src/timer.c" -o "Out/timer.o"
i386-elf-gcc -ffreestanding -m32 -c "Src/isr.c" -o "Out/isr.o"
i386-elf-gcc -ffreestanding -m32 -c "Src/idt.c" -o "Out/idt.o"
i386-elf-gcc -ffreestanding -m32 -c "Src/util.c" -o "Out/util.o"

# Link
i386-elf-ld -o "Out/full_kernel.bin" \
    -Ttext 0x1000 \
    Out/kernel_entry.o \
    Out/interrupt.o \
    Out/kernel.o \
    Out/console.o \
    Out/low_level.o \
    Out/vga.o \
    Out/timer.o \
    Out/isr.o \
    Out/idt.o \
    Out/util.o \
    --oformat binary

#i386-elf-ld -o "Out/full_kernel.bin" -Ttext 0x1000 "Out/kernel_entry.o" "Out/kernel.o" "Out/console.o" "Out/low_level.o" --oformat binary

# 
#cat "Out/boot.bin" "Out/full_kernel.bin" "Out/zeroes.bin"  > "B2OS.bin"
cat Out/boot.bin Out/full_kernel.bin > "B2OS.bin"

#qemu-system-x86_64 -drive format=raw,file="Binaries/OS.bin",index=0,if=floppy,  -m 128M
#qemu-virgil -drive format=raw,file=B2OS.bin,index=0,if=floppy, -m 128M
#qemu-system-x86_64 -drive format=raw,file=B2OS.bin,index=0,if=floppy, -m 128M

