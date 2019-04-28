build:
	# Build boot using NASM
	nasm -f elf32 src/boot.asm -o lib/boot.o

	# Build Kernel in C++
	i686-elf-g++ -g -c -o lib/kernel.o -ffreestanding -O2 -Wall -Wextra src/kernel.c
	i686-elf-g++ -T src/linker.ld -o bin/kernel.bin -ffreestanding -O2 -nostdlib lib/boot.o lib/kernel.o -lgcc
	
	# Remove pre-existing files
	rm -f bin/shiro.iso
	rm -f grub/boot/kernel.bin
	rm -f grub/boot/grub/grub.cfg

	# Copy our kernel to grub so it can be booted later
	cp bin/kernel.bin grub/boot/kernel.bin
	cp etc/grub.cfg grub/boot/grub/grub.cfg
	cp images/shiro.bmp grub/shiro.bmp
	cp images/shiro.png grub/shiro.png

	# Generate an grub iso bootable file
	grub-mkrescue -o bin/shiro.iso grub

docker-build:
	docker exec -it shiro bash -c "cd /home/shiro && ./build.sh"

run:
	# Opens QEMU and Runs the OS
	qemu-system-i386 -boot order=d -cdrom bin/shiro.iso