#!/usr/bin/env bash
set -euo pipefail

FOLDER_BASE=/home/shiro
FOLDER_SRC="$FOLDER_BASE/src"
FOLDER_LIB="$FOLDER_BASE/lib"
FOLDER_BIN="$FOLDER_BASE/bin"
FOLDER_ASM="$FOLDER_SRC/boot/asm"
FOLDER_LINKER="$FOLDER_SRC/boot/linker"
FOLDER_CPP_LIBS="$FOLDER_SRC/libs"
FOLDER_CPP_CLASSES="$FOLDER_SRC/classes"
FOLDER_CPP_VENDOR="$FOLDER_SRC/vendor"

FILE_BOOT="$FOLDER_ASM/boot.asm"
FILE_LINKER="$FOLDER_LINKER/linker.ld"
FILE_KERNEL="$FOLDER_SRC/kernel.cpp"
FILE_KERNEL_BIN="$FOLDER_BIN/kernel.bin"

GCC="i686-elf-g++"
# -O0 chosen deliberately: this v0.4 codebase has many UB / -fpermissive paths
# that the GCC 13 -O2 optimizer mangles (eliding writes through aliased member
# pointers, hoisting strcpy from non-null-terminated sources into infinite
# loops, etc.). A modern rewrite should restore -O2.
GCC="i686-elf-g++"
GCC_PARAMS="-ffreestanding -O0 -Wall -Wextra -fpermissive -Wno-write-strings -Wno-uninitialized -nostdlib -lgcc \
    -I$FOLDER_CPP_LIBS -I$FOLDER_CPP_CLASSES -I$FOLDER_CPP_VENDOR"
GCC_LINKER_PARAMS="-ffreestanding -O0 -Wextra -fpermissive -Wno-write-strings -Wno-uninitialized -nostdlib -lgcc \
    -I$FOLDER_CPP_LIBS -I$FOLDER_CPP_CLASSES -I$FOLDER_CPP_VENDOR"

SEPARATOR="----------------------------------------------------------"

step() {
    echo "$SEPARATOR"
    echo "$1"
    echo "$SEPARATOR"
}

# Ensure output directories exist (fresh checkout won't have them).
mkdir -p "$FOLDER_LIB" "$FOLDER_BIN" "$FOLDER_BASE/grub/boot/grub" "$FOLDER_BASE/log"

# Remove pre-existing files
rm -f "$FOLDER_BASE/grub/boot/kernel.bin"
rm -f "$FOLDER_BASE/grub/boot/grub/grub.cfg"
rm -f "$FOLDER_LIB"/*.o
rm -f "$FOLDER_BIN"/*.iso

step "Compiling boot (NASM)"
nasm -i "$FOLDER_ASM/" -f elf32 "$FILE_BOOT" -o "$FOLDER_LIB/boot.o"

step "Compiling kernel (C++)"
$GCC -c -o "$FOLDER_LIB/kernel.o" $GCC_PARAMS "$FILE_KERNEL"

step "Linking files (C++)"
$GCC -T "$FILE_LINKER" -o "$FILE_KERNEL_BIN" $GCC_LINKER_PARAMS "$FOLDER_LIB"/*.o

step "Building initrd from src/initrd/"
python3 "$FOLDER_BASE/tools/mkinitrd.py" "$FOLDER_BASE/initrd" "$FOLDER_BIN/initrd"

step "Copying files"
cp "$FOLDER_BIN/kernel.bin"   "$FOLDER_BASE/grub/boot/kernel.bin"
cp "$FOLDER_BIN/initrd"        "$FOLDER_BASE/grub/boot/initrd"
cp "$FOLDER_BASE/etc/grub.cfg" "$FOLDER_BASE/grub/boot/grub/grub.cfg"
cp "$FOLDER_BASE/images/shiro.bmp" "$FOLDER_BASE/grub/shiro.bmp"
cp "$FOLDER_BASE/images/shiro.png" "$FOLDER_BASE/grub/shiro.png"

step "Generating shiro iso"
grub-mkrescue -o "$FOLDER_BIN/shiro.iso" "$FOLDER_BASE/grub"

step "DONE"
