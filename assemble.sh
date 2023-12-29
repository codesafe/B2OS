
export PATH=$PATH:/usr/local/i386elfgcc/bin
# Compile assembly code
nasm "Asm/boot.asm" -f bin -o "Out/boot.bin"
nasm "Asm/zeroes.asm" -f bin -o "Out/zeroes.bin"
# Compile C entry code
nasm "Asm/kernel_entry.asm" -f elf -o "Out/kernel_entry.o"

# Compile C Kernel
i386-elf-gcc -ffreestanding -m32 -g -c "Src/kernel.c" -o "Out/kernel.o"
i386-elf-gcc -ffreestanding -m32 -g -c "Driver/vga.c" -o "Out/vga.o"

# Link
i386-elf-ld -o "Out/full_kernel.bin" -Ttext 0x1000 "Out/kernel_entry.o" "Out/kernel.o" --oformat binary

# 
cat "Out/boot.bin" "Out/full_kernel.bin" "Out/zeroes.bin"  > "B2OS.bin"

#qemu-system-x86_64 -drive format=raw,file="Binaries/OS.bin",index=0,if=floppy,  -m 128M

#qemu-virgil -drive format=raw,vfile=B2OS.bin, index=0, if=floppy, -m 128M