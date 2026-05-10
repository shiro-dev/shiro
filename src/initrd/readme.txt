Welcome to Shiro!
=================

This is a file living inside the kernel's ramdisk. It was packed into
/boot/initrd by the build script, loaded by GRUB at boot time as a
multiboot2 module, and exposed to userland by the kernel.

Try the shell commands:
   ls          list files in the ramdisk
   cat <name>  print a file's contents
   echo hello  sanity-check the shell

The ramdisk format is a bespoke baby-tar: a magic number + count, an
array of fixed-size headers (32-byte name + 4-byte size), then file
data concatenated. See classes/initrd.class.h for the parser.
