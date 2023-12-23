#nasm boot.asm -f bin -o os.bin
#qemu-system-x86_64 -drive format=raw,file=os.bin


#export PATH=$PATH:/usr/local/i386elfgcc/bin

nasm boot.asm -f bin -o Binaries/boot.bin
nasm kernel_entry.asm -f elf -o Binaries/kernel_entry.o
nasm zeroes.asm -f bin -o Binaries/zeroes.bin

#i386-elf-gcc -ffreestanding -m32 -g -c kernel.c -o Binaries/kernel.o
#i386-elf-gcc -ffreestanding -m32 -g -c kernel.cpp -o Binaries/kernel.o

gcc -ffreestanding -fpie -m32 -g -c kernel.cpp -o Binaries/kernel.o
gcc -ffreestanding -fpie -m32 -g -c vga.cpp -o Binaries/vga.o
#gcc -std=gnu99 -ffreestanding -O2 -Wall -Wextra -v -m32 -g -c kernel.cpp -o Binaries/kernel.o
#gcc -std=gnu99 -ffreestanding -O2 -Wall -Wextra -v -m32 -g -c vga.cpp -o Binaries/vga.o
#i386-elf-ld -o Binaries/full_kernel.bin -Ttext 0x1000 Binaries/kernel_entry.o Binaries/kernel.o --oformat binary
#ld -melf_i386 -o Binaries/full_kernel.bin -Ttext 0x1000 Binaries/kernel_entry.o Binaries/kernel.o --oformat binary --ignore-unresolved-symbol _GLOBAL_OFFSET_TABLE_
ld -melf_i386 -o Binaries/full_kernel.bin -Ttext 0x1000 Binaries/kernel_entry.o Binaries/kernel.o Binaries/vga.o --oformat binary --ignore-unresolved-symbol _GLOBAL_OFFSET_TABLE_

cat "Binaries/boot.bin" "Binaries/full_kernel.bin" "Binaries/zeroes.bin"  > "Binaries/OS.bin"

#qemu-system-x86_64 -drive format=raw,file="Binaries/OS.bin",index=0,if=floppy,  -m 128M
qemu-virgil -drive format=raw,file="Binaries/OS.bin",index=0,if=floppy,  -m 128M