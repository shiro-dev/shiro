# syntax=docker/dockerfile:1.7

# ---------- Stage 1: build the i686-elf cross toolchain from source ----------
FROM debian:bookworm-slim AS toolchain

ARG BINUTILS_VERSION=2.42
ARG GCC_VERSION=13.2.0
ARG GDB_VERSION=14.2
ARG PREFIX=/opt/cross
ARG TARGET=i686-elf
ARG JOBS=4

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential \
        bison \
        flex \
        libgmp-dev \
        libmpc-dev \
        libmpfr-dev \
        libisl-dev \
        libexpat1-dev \
        texinfo \
        wget \
        ca-certificates \
        xz-utils \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /tmp/build

RUN wget -q https://ftp.gnu.org/gnu/binutils/binutils-${BINUTILS_VERSION}.tar.xz \
 && wget -q https://ftp.gnu.org/gnu/gcc/gcc-${GCC_VERSION}/gcc-${GCC_VERSION}.tar.xz \
 && tar -xf binutils-${BINUTILS_VERSION}.tar.xz \
 && tar -xf gcc-${GCC_VERSION}.tar.xz

RUN mkdir build-binutils && cd build-binutils \
 && ../binutils-${BINUTILS_VERSION}/configure \
        --target=${TARGET} \
        --prefix=${PREFIX} \
        --with-sysroot \
        --disable-nls \
        --disable-werror \
 && make -j${JOBS} \
 && make install

ENV PATH=${PREFIX}/bin:${PATH}

RUN cd gcc-${GCC_VERSION} && ./contrib/download_prerequisites \
 && cd .. \
 && mkdir build-gcc && cd build-gcc \
 && ../gcc-${GCC_VERSION}/configure \
        --target=${TARGET} \
        --prefix=${PREFIX} \
        --disable-nls \
        --enable-languages=c,c++ \
        --without-headers \
 && make -j${JOBS} all-gcc \
 && make -j${JOBS} all-target-libgcc \
 && make install-gcc \
 && make install-target-libgcc

RUN cd /tmp/build \
 && wget -q https://ftp.gnu.org/gnu/gdb/gdb-${GDB_VERSION}.tar.xz \
 && tar -xf gdb-${GDB_VERSION}.tar.xz \
 && mkdir build-gdb && cd build-gdb \
 && ../gdb-${GDB_VERSION}/configure \
        --target=${TARGET} \
        --prefix=${PREFIX} \
        --disable-nls \
        --disable-werror \
 && make -j${JOBS} \
 && make install

# ---------- Stage 2: runtime image used to build the OS ----------
FROM debian:bookworm-slim

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
        nasm \
        xorriso \
        mtools \
        grub-common \
        grub-pc-bin \
        qemu-system-x86 \
        novnc \
        websockify \
        procps \
        make \
        ca-certificates \
    && rm -rf /var/lib/apt/lists/*

COPY --from=toolchain /opt/cross /opt/cross
ENV PATH=/opt/cross/bin:${PATH}

WORKDIR /home/shiro
