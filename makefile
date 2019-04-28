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