build:
	docker network ls | grep "shiro" || $(MAKE) -s create-network; exit 0
	docker-compose build --no-cache
	docker-compose up -d

run:
	docker exec -it shiro bash -c "cd /home/shiro && ./build.sh"
	qemu-system-i386 -boot order=d -cdrom src/bin/shiro.iso

create-network:
	docker network create shiro-network

dbuild:
	docker-compose build

dup:
	docker-compose up -d

dub:
	docker-compose build && docker-compose up -d

dubr:
	docker-compose build && docker-compose up -d && docker exec -it shiro bash

dbash:
	docker exec -it shiro bash