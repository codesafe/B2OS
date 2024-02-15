cls
qemu-system-x86_64 -blockdev driver=file,node-name=fda,filename=floppy.img -device floppy,drive=fda,drive-type=144 -display sdl
