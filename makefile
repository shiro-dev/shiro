.PHONY: docker-build docker-up docker-down docker-enter \
        build run run-host stop-vm debug gdb clean

# Stop Git Bash / MSYS on Windows from rewriting Linux paths like /home/shiro
# into C:/Program Files/Git/home/shiro when they're passed to docker exec.
export MSYS_NO_PATHCONV := 1
export MSYS2_ARG_CONV_EXCL := *

docker-build:
	docker compose build
	docker compose up -d

docker-up:
	docker compose up -d

docker-down:
	docker compose down

docker-enter:
	docker exec -it shiro bash

build:
	docker exec -i shiro bash -c "cd /home/shiro && ./build.sh"

# Run inside the container, view in a browser (no host QEMU install needed).
# After this command finishes, open: http://localhost:6080/vnc.html?autoconnect=1&resize=scale
run: build stop-vm
	docker exec -d shiro qemu-system-i386 \
		-boot order=d -cdrom /home/shiro/bin/shiro.iso \
		-vnc :0 -no-shutdown
	docker exec -d shiro websockify --web=/usr/share/novnc 6080 localhost:5900
	@echo ""
	@echo "Shiro is running."
	@echo "Open: http://localhost:6080/vnc.html?autoconnect=1&resize=scale"
	@echo "Stop with: make stop-vm"

# Stop the in-container QEMU + websockify processes started by `make run`.
stop-vm:
	-docker exec shiro pkill -f qemu-system-i386 2>/dev/null || true
	-docker exec shiro pkill -f websockify 2>/dev/null || true

# Run using a host-installed QEMU (faster, no VNC overhead, but requires
# qemu-system-i386 on PATH).
run-host: build
	qemu-system-i386 -boot order=d -cdrom src/bin/shiro.iso

# Same as run-host but logs the serial port to src/log/serial.log.
debug: build
	qemu-system-i386 -boot order=d -cdrom src/bin/shiro.iso \
		-chardev stdio,id=char0,logfile=src/log/serial.log,signal=off \
		-serial chardev:char0

# Boot inside the container, halted, waiting for a gdb connection on :1234.
# In another terminal:
#   docker exec -it shiro i686-elf-gdb -ex "target remote :1234" /home/shiro/bin/kernel.bin
gdb: build stop-vm
	docker exec -d shiro qemu-system-i386 \
		-boot order=d -cdrom /home/shiro/bin/shiro.iso \
		-vnc :0 -gdb tcp:0.0.0.0:1234 -S
	docker exec -d shiro websockify --web=/usr/share/novnc 6080 localhost:5900
	@echo ""
	@echo "QEMU is paused, gdb listening on :1234, view at http://localhost:6080/vnc.html"

clean:
	rm -f src/bin/*.iso src/bin/*.bin src/lib/*.o
	rm -f src/grub/boot/kernel.bin src/grub/boot/grub/grub.cfg
	rm -f src/log/*.log
