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
RUN apt-get install -y bsdmainutils
RUN apt-get install -y curl

RUN echo "COPY INSTALL FOLDER TO SERVER"
COPY ./install /docker-install

RUN echo "INSTALL i686 ELF CROSS-COMPILER"
RUN bash /docker-install/install.sh

#RUN bash /docker-install/i686-elf-tools.sh
#CMD cp /docker-install/i686-elf-tools-linux.zip /install/i686-elf-tools-linux.zip

RUN echo "INSTALL COMPOSER"
RUN curl -sS https://getcomposer.org/installer | php -- --install-dir=/usr/local/bin --filename=composer

RUN echo "INSTAL AST(PECL-PHP)"
RUN pecl install ast
RUN echo "extension=$(find /usr/local/lib/php/extensions/ -name ast.so)" > /usr/local/etc/php/conf.d/ast.ini

RUN echo "OTHER THINGS TO INSTALL"
RUN apt-get install -y build-essential libgmp-dev libmpc-dev libmpfr-dev cmake
RUN apt-get install -y libc++-dev

RUN echo "INSTALL LLVM"
RUN apt-get install -y software-properties-common
RUN wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add -
RUN apt-add-repository "deb http://apt.llvm.org/trusty/ llvm-toolchain-trusty main"
RUN apt-add-repository "deb http://apt.llvm.org/trusty/ llvm-toolchain-trusty-7 main"
RUN apt-add-repository "deb http://apt.llvm.org/trusty/ llvm-toolchain-trusty-8 main"
RUN apt-add-repository "deb http://ppa.launchpad.net/ubuntu-toolchain-r/test/ubuntu trusty main"
RUN apt-get update -y

# LLVM
RUN apt-get install -y libllvm-7-ocaml-dev libllvm7 llvm-7 llvm-7-dev llvm-7-doc llvm-7-examples llvm-7-runtime

# Clang and co
RUN apt-get install -y clang-7 clang-tools-7 clang-7-doc libclang-common-7-dev libclang-7-dev libclang1-7 clang-format-7 python-clang-7

# libfuzzer
RUN apt-get install -y libfuzzer-7-dev

# lldb
RUN apt-get install -y lldb-7

# lld (linker)
RUN apt-get install -y lld-7

# libc++
RUN apt-get install -y libc++-7-dev libc++abi-7-dev

# OpenMP
RUN apt-get install -y libomp-7-dev

RUN echo "DONE"
WORKDIR /home/shiro