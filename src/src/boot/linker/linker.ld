ENTRY(shiro.start)
SECTIONS
{
	. = 1M;
	.grub BLOCK(4K) : ALIGN(4K)
	{
		*(.grub)
		*(.shiro)
	}
 	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
	}
 	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}
 	.bss BLOCK(16K) : ALIGN(16K)
	{
		*(COMMON)
		*(.bss)
	}
}