docker-build:
	docker network ls | grep "shiro" || $(MAKE) -s docker-create-network; exit 0
	docker-compose build --no-cache
	docker-compose up -d

build:
	docker exec -it shiro bash -c "cd /home/shiro && ./build.sh"

run:
	docker exec -it shiro bash -c "cd /home/shiro && ./build.sh"
	qemu-system-i386 -boot order=d -cdrom src/bin/shiro.iso

debug:
	docker exec -it shiro bash -c "cd /home/shiro && ./build.sh"
	qemu-system-i386 -boot order=d -cdrom src/bin/shiro.iso -chardev stdio,id=char0,logfile=src/log/serial.log,signal=off -serial chardev:char0

docker-create-network:
	docker network create shiro-network

docker-up:
	docker-compose up -d

docker-enter:
	docker exec -it shiro bash