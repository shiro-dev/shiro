# Build boot using NASM
nasm -f elf32 src/boot.asm -o lib/boot.o

# Build Kernel in C++
/docker-install/i686-elf-tools-linux/bin/i686-elf-g++ -g -c -o lib/kernel.o -ffreestanding -O2 -Wall -Wextra src/kernel.c
/docker-install/i686-elf-tools-linux/bin/i686-elf-g++ -T src/linker.ld -o bin/kernel.bin -ffreestanding -O2 -nostdlib lib/boot.o lib/kernel.o -lgcc

# Remove pre-existing kernel.bin
rm -f grub/boot/kernel.bin
rm -f grub/boot/grub/grub.cfg

# Copy our kernel to grub so it can be booted later
cp bin/kernel.bin grub/boot/kernel.bin
cp etc/grub.cfg grub/boot/grub/grub.cfg
cp images/shiro.bmp grub/shiro.bmp
cp images/shiro.png grub/shiro.png

# Generate an grub iso bootable file
grub-mkrescue -o bin/shiro.iso grub