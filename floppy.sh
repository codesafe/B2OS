dd if=/dev/zero of=floppy.img bs=512 count=2880
mkfs.fat -F 12 -n "B2OS" floppy.img
#dd if=B2OS.bin of=floppy.img conv=notrunc
mcopy -i floppy.img run.bat "::run.bat"
mcopy -i floppy.img B2OS.bin "::B2OS.bin"

#sudo mount -o loop imagefile.img /media/floppy1/ --options rw