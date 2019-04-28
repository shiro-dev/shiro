FROM php:7.1.27-apache-jessie

RUN echo "APT-GET UPDATE"
RUN apt-get update -y

RUN echo "APT-GET INSTALL"
RUN apt-get install -y build-essential bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo libcloog-isl-dev libisl-dev qemu grub-common xorriso nasm grub-pc-bin
RUN apt-get install -y wget
RUN apt-get install -y nano
RUN apt-get install -y git 
RUN apt-get install -y zip 
RUN apt-get install -y unzip
RUN apt-get install -y tar
RUN apt-get install -y gcc-multilib

RUN echo "COPY INSTALL FOLDER TO SERVER"
COPY ./install /docker-install

RUN echo "INSTALL i686 ELF CROSS-COMPILER"
RUN bash /docker-install/install.sh

RUN echo "DONE"
WORKDIR /home/shiro