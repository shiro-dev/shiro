<h1 align="center">Shiro - Operating System (OS)</h3>

<p align="center">

<img src="http://nimbleking.com/shiro/images/shiro.bmp" alt="Shiro - Operating System (OS)"/>

<br/>

<img src="https://img.shields.io/badge/Version-v0.5-green.svg" alt="Version">
&nbsp;
<img src="https://img.shields.io/badge/Date-2026/05/10-green.svg" alt="Version">

</p>

## How to get started?

Line endings are enforced by `.gitattributes`, so a fresh clone on any
platform will check out with LF endings — no manual git config needed.

```shell
$ git clone git@github.com:shiro-dev/shiro.git
```

## Requirements

- [Docker](https://docs.docker.com/get-docker/) (with the `docker compose` plugin)
- A web browser (for the default `make run` — Shiro renders inside the
  container and you view it at `http://localhost:6080/vnc.html`).

Everything else — the cross-compiler (binutils + gcc + gdb for `i686-elf`),
`nasm`, `xorriso`, `grub-pc-bin`, QEMU, and noVNC — lives inside the Docker
image. No host-side installs.

> Optional: if you already have `qemu-system-i386` on your host, `make run-host`
> skips the VNC layer for a snappier display.

## Build & Run

```shell
$ make docker-build   # one-time, ~20-40 min (toolchain compile, then cached)
$ make run            # builds the kernel and boots Shiro in the container
```

Then open <http://localhost:6080/vnc.html?autoconnect=1&resize=scale> to see
the running OS. Stop the VM with `make stop-vm`.

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
- [Erik Helin](https://littleosbook.github.io/)
- [Adam Renberg](https://littleosbook.github.io/)
- [James Molloy](http://www.jamesmolloy.co.uk/)
- [Samy Pessé](https://samypesse.gitbook.io/)
- [Klaykap](https://github.com/Klaykap)
- [Tu, Do Hoang](https://github.com/tuhdo/)
- [Brandon F.](http://www.osdever.net/bkerndev/Docs/isrs.htm)