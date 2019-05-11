FOLDER_BASE=/home/shiro
FOLDER_SRC="$FOLDER_BASE/src"
FOLDER_LIB="$FOLDER_BASE/lib"
FOLDER_BIN="$FOLDER_BASE/bin"
FOLDER_ASM="$FOLDER_SRC/boot/asm"
FOLDER_LINKER="$FOLDER_SRC/boot/linker"
FOLDER_CPP_LIBS="$FOLDER_SRC/libs"
FOLDER_CPP_CLASSES="$FOLDER_SRC/classes"
FOLDER_CPP_VENDOR="$FOLDER_SRC/vendor"

echo $FOLDER_CPP_LIBS

FILE_BOOT="$FOLDER_ASM/boot.asm"
FILE_LINKER="$FOLDER_LINKER/linker.ld"
FILE_KERNEL="$FOLDER_SRC/kernel.cpp"
FILE_KERNEL_BIN="$FOLDER_BIN/kernel.bin"

FILES="$FOLDER_SRC/*/*.cpp"

GCC="/docker-install/i686-elf-tools-linux/bin/i686-elf-g++"
#GCC="/docker-install/cross-compiler-libsupcxx/bin/i686-elf-g++"
#GCC="clang-7 -target i686-elf"
GCC_PARAMS="-ffreestanding -O2 -Wall -Wextra -fconcepts -fpermissive -Wno-write-strings -Wno-uninitialized -nostdlib -lgcc -I$FOLDER_CPP_LIBS -I$FOLDER_CPP_CLASSES -I$FOLDER_CPP_VENDOR"
GCC_LINKER_PARAMS="-ffreestanding -O2 -Wextra -fconcepts -fpermissive -Wno-write-strings -Wno-uninitialized -nostdlib -lgcc -I$FOLDER_CPP_LIBS -I$FOLDER_CPP_CLASSES -I$FOLDER_CPP_VENDOR"

SEPARATOR="----------------------------------------------------------"
LINEBREAK="\n\n"

# Remove pre-existing files
rm -f grub/boot/kernel.bin
rm -f grub/boot/grub/grub.cfg
rm -f lib/*.o
rm -f bin/*.iso

# Compile Boot
echo $SEPARATOR
echo "Compiling boot (NASM)"
echo $SEPARATOR
COMMAND=$(nasm -i "$FOLDER_ASM/" -f elf32 $FILE_BOOT -o $FOLDER_LIB/boot.o 2>&1)
if [ -z "$COMMAND" ]; then
    echo "OK"
else
    echo $COMMAND
    echo $LINEBREAK
    exit
fi
echo $SEPARATOR
echo $LINEBREAK

# Compile Kernel
echo $SEPARATOR
echo "Compiling kernel (C++)"
echo $SEPARATOR
$GCC -c -o $FOLDER_LIB/kernel.o $GCC_PARAMS $FILE_KERNEL
echo "OK"
echo $SEPARATOR
echo $LINEBREAK

# Compile other C++ files
# for PATH_FILE in $FILES
# do
#     FILENAME=$(basename $PATH_FILE| cut -f 1 -d '.')
    
#     echo $SEPARATOR
#     echo "Compiling $PATH_FILE (C++)"
#     echo $SEPARATOR
#     COMMAND=$($GCC -c -o $FOLDER_LIB/$FILENAME.o $GCC_PARAMS $PATH_FILE)
#     if [ -z "$COMMAND" ]; then
#         echo "OK"
#     else
#         echo $COMMAND
#         echo $LINEBREAK
#         exit
#     fi
#     echo $SEPARATOR
#     echo $LINEBREAK
# done

# Link files
echo $SEPARATOR
echo "Linking files (C++)"
echo $SEPARATOR
COMMAND=$($GCC -T $FILE_LINKER -o $FILE_KERNEL_BIN $GCC_LINKER_PARAMS $FOLDER_LIB/*.o 2>&1)
if [ -z "$COMMAND" ]; then
    echo "OK"
else
    echo $COMMAND
    echo $LINEBREAK
    exit
fi
echo $SEPARATOR
echo $LINEBREAK

# Copy our kernel to grub so it can be booted later
echo $SEPARATOR
echo "Copying files"
echo $SEPARATOR
COMMAND_1=$(cp bin/kernel.bin grub/boot/kernel.bin 2>&1)
COMMAND_2=$(cp etc/grub.cfg grub/boot/grub/grub.cfg 2>&1)
COMMAND_3=$(cp images/shiro.bmp grub/shiro.bmp 2>&1)
COMMAND_4=$(cp images/shiro.png grub/shiro.png 2>&1)
if [ ! -z "$COMMAND_1" ]; then
    echo $COMMAND_1
    echo $LINEBREAK
    exit
fi
if [ ! -z "$COMMAND_2" ]; then
    echo $COMMAND_2
    echo $LINEBREAK
    exit
fi
if [ ! -z "$COMMAND_3" ]; then
    echo $COMMAND_3
    echo $LINEBREAK
    exit
fi
if [ ! -z "$COMMAND_4" ]; then
    echo $COMMAND_4
    echo $LINEBREAK
    exit
fi
echo "OK"
echo $SEPARATOR
echo $LINEBREAK

# Generate an grub iso bootable file
echo $SEPARATOR
echo "Generating shiro iso"
echo $SEPARATOR
grub-mkrescue -o bin/shiro.iso grub
echo $SEPARATOR
echo $LINEBREAK

# Finish
echo $SEPARATOR
echo "DONE"
echo $SEPARATOR
echo $LINEBREAK