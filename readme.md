# Shiro - Operating System (OS)

![](https://img.shields.io/badge/Version-v0.1-green.svg)

![Shiro - Operating System (OS)](http://nimbleking.com/shiro/images/shiro.bmp)

## How to start?
```shell
$ git clone https://github.com/shiro-dev/shiro.git
```

## Requirements

### Windows
[QEMU emulator version 3.1.50+](https://qemu.weilnetz.de/w64/qemu-w64-setup-20190218.exe)

[Docker version 18.03.0-ce+](https://hub.docker.com/editions/community/docker-ce-desktop-windows)

Docker Compose version 1.20.1+

### Ubuntu (not tested)
QEMU emulator version 2.1.2+
```shell
$ apt-get update -y
$ apt-get install -y qemu
```
Docker
```shell
$ sudo apt-get update -y
$ sudo apt-get install docker-ce
$ sudo curl -L "https://github.com/docker/compose/releases/download/1.23.1/docker-compose-$(uname -s)-$(uname -m)" -o /usr/local/bin/docker-compose
$ sudo chmod +x /usr/local/bin/docker-compose
```
If you need to set-up docker repository, please refer to [docker documentation](https://docs.docker.com/v17.09/engine/installation/linux/docker-ce/ubuntu/#install-docker-ce-1).

Docker Compose
```shell
$ sudo apt-get update -y
$ sudo curl -L "https://github.com/docker/compose/releases/download/1.23.1/docker-compose-$(uname -s)-$(uname -m)" -o /usr/local/bin/docker-compose
$ sudo chmod +x /usr/local/bin/docker-compose
```

## Build Environment
This command will build the development environment on docker.
```shell
$ make build
```

## Run Shiro
This command will build/rebuild the OS on docker and will run it using QEMU.
```shell
$ make run
```