clear
rm -fr *.img
rm -fr *.bin
rm -fr  Out/*.o Out/*.bin

export PATH=$PATH:/usr/local/i386elfgcc/bin
# Compile assembly code
# bin -> bin
nasm "Asm/boot_first.asm" -f bin -o "Out/boot_first.bin"
nasm "Asm/boot_second.asm" -f bin -o "Out/boot_second.bin"

# Compile C entry code
# o -> elf format
nasm "Asm/kernel_entry.asm" -f elf -o "Out/kernel_entry.o"
nasm "Asm/interrupt.asm" -f elf -o "Out/interrupt.o"


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
i386-elf-gcc -ffreestanding -m32 -c "Src/memory.c" -o "Out/memory.o"

# apple2 emulator
#i386-elf-gcc -ffreestanding -m32 -c "Apple2/AppleMachine.c" -o "Out/AppleMachine.o"
#i386-elf-gcc -ffreestanding -m32 -c "Apple2/AppleDevice.c" -o "Out/AppleDevice.o"
#i386-elf-gcc -ffreestanding -m32 -c "Apple2/AppleMem.c" -o "Out/AppleMem.o"
#i386-elf-gcc -ffreestanding -m32 -c "Apple2/AppleCpu.c" -o "Out/AppleCpu.o"

# Link
i386-elf-ld -o "Out/full_kernel.bin" \
    -Ttext 0xF000 \
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
    Out/memory.o \
    --oformat binary

#    Out/AppleMem.o \
#    Out/AppleCpu.o \
#    Out/AppleDevice.o \
 #   Out/AppleMachine.o \



#i386-elf-ld -o "Out/full_kernel.bin" -Ttext 0x1000 "Out/kernel_entry.o" "Out/kernel.o" "Out/console.o" "Out/low_level.o" --oformat binary


cat Out/boot_first.bin > boot.bin
cat Out/boot_second.bin > boot2.bin
cat Out/full_kernel.bin > kernel.bin

#cat Out/boot_first.bin Out/boot_second.bin > B2OS.bin
#cat Out/boot_first.bin > B2OS.bin



#!/bin/bash
set -e
COLOR_GREEN=$(tput setaf 2)		# green
COLOR_RED=$(tput setaf 1)		# red
COLOR_RESET=$(tput sgr0)		# reset

echo "${COLOR_GREEN}Creating bootable floppy image...${COLOR_RESET}"
sudo dd if=/dev/zero of=floppy.img bs=512 count=2880
sudo dd if=boot.bin of=floppy.img bs=512 conv=notrunc
sudo losetup /dev/loop0 floppy.img
sudo mount /dev/loop0 /mnt
sudo cp boot2.bin /mnt
sudo cp kernel.bin /mnt
sudo umount /mnt
sudo losetup -d /dev/loop0

# mkfs.msdos -C floppy.img 1440
# sudo dd if=boot.bin of=floppy.img bs=512 conv=notrunc
# sudo losetup /dev/loop0 floppy.img
# sudo mount /dev/loop0 /mnt
# sudo cp kernel.bin /mnt
# sudo umount /mnt
# sudo losetup -d /dev/loop0


# int13 extended mode는 hdd만됨
#qemu-system-x86_64 -m 1024 -machine type=pc -drive format=raw,file=B2OS.bin
#qemu-system-x86_64 -drive format=raw,file=B2OS.bin,index=0,if=ide, -m 128M
#qemu-system-x86_64 -drive format=raw,file=floppy.img,index=0,if=ide, -m 128M

#qemu-system-x86_64 -blockdev driver=file,node-name=fda,filename=floppy.img -device floppy,drive=fda,drive-type=144