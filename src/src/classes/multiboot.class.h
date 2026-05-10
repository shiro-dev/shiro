#ifndef MULTIBOOT_CLASS_H
#define MULTIBOOT_CLASS_H

#include <stdint.h>
#include <multiboot2.h>

namespace System
{
class Multiboot
{
public:
    bool valid;
    uint32_t mem_lower;            // KiB below 1MiB
    uint32_t mem_upper;            // KiB above 1MiB up to first hole
    const char *cmdline;           // kernel command line passed by GRUB (may be 0)
    uint32_t mmap_count;           // number of memory-map entries discovered
    uint64_t mmap_total_available; // bytes flagged as available across all entries
    uint64_t fb_addr;              // framebuffer base, if GRUB provided one
    uint32_t fb_width;
    uint32_t fb_height;
    uint8_t  fb_bpp;
    uint8_t  fb_type;

    void Reset()
    {
        valid = false;
        mem_lower = 0;
        mem_upper = 0;
        cmdline = 0;
        mmap_count = 0;
        mmap_total_available = 0;
        fb_addr = 0;
        fb_width = 0;
        fb_height = 0;
        fb_bpp = 0;
        fb_type = 0;
    }

    /**
     * System::Multiboot.Start(magic, mbi)
     *
     * Parses the multiboot2 info structure GRUB hands us.
     * Boot.asm pushes ebx (mbi pointer) and eax (magic) so kernel.cpp's
     * shiro_main(magic, mbi) gets them as the first two arguments.
     */
    void Start(uint32_t magic, void *mbi)
    {
        log("> Parsing multiboot2 info");
        Reset();

        if (magic != MULTIBOOT2_BOOTLOADER_MAGIC)
        {
            log("> [WARN] Multiboot2 magic mismatch — boot info unavailable");
            return;
        }
        if (mbi == 0)
        {
            log("> [WARN] Multiboot2 info pointer is null");
            return;
        }

        valid = true;

        // The first 8 bytes are total_size + reserved; tags follow, 8-byte aligned,
        // and terminate with a zero-type tag.
        uint8_t *cursor = (uint8_t *)mbi + 8;
        for (;;)
        {
            mb2_tag *tag = (mb2_tag *)cursor;
            if (tag->type == MB2_TAG_END)
                break;

            switch (tag->type)
            {
                case MB2_TAG_CMDLINE:
                {
                    mb2_tag_string *t = (mb2_tag_string *)tag;
                    cmdline = t->string;
                    break;
                }
                case MB2_TAG_BASIC_MEMINFO:
                {
                    mb2_tag_basic_meminfo *t = (mb2_tag_basic_meminfo *)tag;
                    mem_lower = t->mem_lower;
                    mem_upper = t->mem_upper;
                    break;
                }
                case MB2_TAG_MMAP:
                {
                    mb2_tag_mmap *t = (mb2_tag_mmap *)tag;
                    uint32_t entries_size = t->size - 16; // subtract type+size+entry_size+entry_version
                    uint32_t count = entries_size / t->entry_size;
                    mmap_count = count;

                    uint8_t *e = (uint8_t *)t->entries;
                    for (uint32_t i = 0; i < count; i++)
                    {
                        mb2_mmap_entry *entry = (mb2_mmap_entry *)(e + i * t->entry_size);
                        if (entry->type == MB2_MEMORY_AVAILABLE)
                        {
                            mmap_total_available += entry->length;
                        }
                    }
                    break;
                }
                case MB2_TAG_FRAMEBUFFER:
                {
                    mb2_tag_framebuffer_common *t = (mb2_tag_framebuffer_common *)tag;
                    fb_addr = t->framebuffer_addr;
                    fb_width = t->framebuffer_width;
                    fb_height = t->framebuffer_height;
                    fb_bpp = t->framebuffer_bpp;
                    fb_type = t->framebuffer_type;
                    break;
                }
                default:
                    break;
            }

            // Advance to the next 8-byte aligned tag.
            uint32_t step = (tag->size + 7) & ~7u;
            cursor += step;
        }

        log("> Multiboot: mem_lower(KiB)=", (int)mem_lower, " mem_upper(KiB)=", (int)mem_upper);
        log("> Multiboot: mmap entries=", (int)mmap_count, " available(MiB)~",
            (int)(mmap_total_available / (1024 * 1024)));
        if (cmdline != 0)
        {
            log("> Multiboot: cmdline=", cmdline);
        }
        if (fb_addr != 0)
        {
            log("> Multiboot: framebuffer ", (int)fb_width, "x", (int)fb_height,
                " bpp=", (int)fb_bpp, " type=", (int)fb_type);
        }
    }
};
} // namespace System

#endif
