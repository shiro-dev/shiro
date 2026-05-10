ENTRY(shiro.start)
SECTIONS
{
	. = 1M;
	_kernel_start = .;

	/* Multiboot2 header must live in the first 32 KiB of the ELF file
	 * for GRUB to find it. With the kernel growing past trivial size,
	 * we must list .grub first AND list .text explicitly so GCC's
	 * per-function .text.* sections don't end up before .grub. */
	.grub BLOCK(4K) : ALIGN(4K)
	{
		*(.grub)
		*(.shiro)
	}
	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.text)
		*(.text.*)
	}
	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
		*(.rodata.*)
	}
	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
		*(.data.*)
	}
	.bss BLOCK(16K) : ALIGN(16K)
	{
		*(COMMON)
		*(.bss)
		*(.bss.*)
	}
	. = ALIGN(4K);
	_kernel_end = .;
}
