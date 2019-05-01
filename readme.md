# Shiro - Operating System (OS)

<p align="center">

<img src="http://nimbleking.com/shiro/images/shiro.bmp" alt="Shiro - Operating System (OS)"/>

<br/>

<img src="https://img.shields.io/badge/Version-v0.2-green.svg" alt="Version">
&nbsp;
<img src="https://img.shields.io/badge/Date-2019/05/01-green.svg" alt="Version">

</p>

## How to start?
If you are using windows, make sure all files are LF instead of CRLF. 

You can disable CRLF on Git by doing **(optional)**:
```shell
$ git config --global core.safecrlf false
$ git config --global core.autocrlf false
```

To clone the project:
```shell
$ git clone git@github.com:shiro-dev/shiro.git
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
$ make docker-build
```

## Run Shiro
This command will build/rebuild the OS on docker and will run it using QEMU.
```shell
$ make run
```

## Special Thanks
- [OSDev.org](https://osdev.org/)
- [OSDev Forum](https://forum.osdev.org/)
- [GNU](https://www.gnu.org/)
- [Free Software Foundation](https://www.fsf.org/pt-br)
- [Philipp Oppermann](https://os.phil-opp.com/)
- [Leo Whitehead](https://medium.com/@lduck11007)
- [Hacker Pulp](https://hackerpulp.com/)
- [Eugene Obrezkov](https://blog.ghaiklor.com/how-to-implement-your-own-hello-world-boot-loader-c0210ef5e74b)
- [Mike Saunders](http://mikeos.sourceforge.net/)
- [Computer Tyme](http://www.ctyme.com/)
- [Napalm](http://www.rohitab.com/discuss/user/3860-napalm/)
- [Yoshinori K. Okuji](http://download-mirror.savannah.gnu.org/releases/grub/phcoder/multiboot.pdf)
- [Bryan Ford](http://download-mirror.savannah.gnu.org/releases/grub/phcoder/multiboot.pdf)
- [Erich Stefan Boleyn](http://download-mirror.savannah.gnu.org/releases/grub/phcoder/multiboot.pdf)
- [Kunihiro Ishiguro](http://download-mirror.savannah.gnu.org/releases/grub/phcoder/multiboot.pdf)
