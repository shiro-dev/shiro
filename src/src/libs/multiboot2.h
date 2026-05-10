#ifndef MULTIBOOT2_H
#define MULTIBOOT2_H

#include <stdint.h>

#define MULTIBOOT2_BOOTLOADER_MAGIC 0x36d76289

#define MB2_TAG_END               0
#define MB2_TAG_CMDLINE           1
#define MB2_TAG_BOOTLOADER_NAME   2
#define MB2_TAG_MODULE            3
#define MB2_TAG_BASIC_MEMINFO     4
#define MB2_TAG_MMAP              6
#define MB2_TAG_FRAMEBUFFER       8

#define MB2_MEMORY_AVAILABLE       1
#define MB2_MEMORY_RESERVED        2
#define MB2_MEMORY_ACPI_RECLAIM    3
#define MB2_MEMORY_NVS             4
#define MB2_MEMORY_BADRAM          5

struct mb2_info
{
    uint32_t total_size;
    uint32_t reserved;
} __attribute__((packed));

struct mb2_tag
{
    uint32_t type;
    uint32_t size;
} __attribute__((packed));

struct mb2_tag_string
{
    uint32_t type;
    uint32_t size;
    char string[0];
} __attribute__((packed));

struct mb2_tag_basic_meminfo
{
    uint32_t type;
    uint32_t size;
    uint32_t mem_lower; // KiB below 1MiB
    uint32_t mem_upper; // KiB above 1MiB up to first hole
} __attribute__((packed));

struct mb2_mmap_entry
{
    uint64_t base_addr;
    uint64_t length;
    uint32_t type;     // MB2_MEMORY_*
    uint32_t reserved;
} __attribute__((packed));

struct mb2_tag_mmap
{
    uint32_t type;
    uint32_t size;
    uint32_t entry_size;
    uint32_t entry_version;
    struct mb2_mmap_entry entries[0];
} __attribute__((packed));

struct mb2_tag_framebuffer_common
{
    uint32_t type;
    uint32_t size;
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t  framebuffer_bpp;
    uint8_t  framebuffer_type; // 0=indexed, 1=rgb, 2=ega text
    uint8_t  reserved;
} __attribute__((packed));

#endif
