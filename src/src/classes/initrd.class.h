#ifndef INITRD_CLASS_H
#define INITRD_CLASS_H

#include <stdint.h>
#include <stddef.h>
#include <multiboot2.h>

namespace System
{
/**
 * System::Initrd
 *
 * In-memory ramdisk loaded by GRUB as a multiboot2 module. Read-only,
 * flat structure — basically a baby tar:
 *
 *   [u32 magic = 'SHIR'] [u32 num_files]
 *   [ {char name[32], u32 size, u32 offset} * num_files ]
 *   [ payloads concatenated ]
 *
 * `offset` is from the start of the binary, so a single base+size
 * pair from multiboot is enough to navigate everything.
 *
 * The packer that produces this format is at tools/mkinitrd.py and
 * runs as part of build.sh.
 */
class Initrd
{
public:
    static const uint32_t MAGIC = 0x53484952u; // 'SHIR' little-endian when stored u32
    static const uint32_t NAME_LEN = 32;

    struct file_entry_t
    {
        char     name[NAME_LEN];
        uint32_t size;
        uint32_t offset;
    } __attribute__((packed));

    static uint32_t      base;
    static uint32_t      length;
    static uint32_t      file_count;
    static file_entry_t *entries;
    static bool          ready;

    /**
     * System::Initrd::Start(mbi)
     *
     * Walks the multiboot2 tag chain looking for the first MODULE tag,
     * checks the SHIR magic, and remembers the file table location.
     */
    static void Start(void *mbi)
    {
        ready = false;
        base = 0;
        length = 0;
        file_count = 0;
        entries = 0;

        if (mbi == 0)
        {
            log("> [Initrd] no multiboot info — initrd disabled");
            return;
        }

        uint8_t *cursor = (uint8_t *)mbi + 8;
        for (;;)
        {
            mb2_tag *tag = (mb2_tag *)cursor;
            if (tag->type == MB2_TAG_END) break;

            if (tag->type == MB2_TAG_MODULE)
            {
                // Layout: type, size, mod_start, mod_end, [string...]
                uint32_t *u = (uint32_t *)tag;
                uint32_t mod_start = u[2];
                uint32_t mod_end   = u[3];

                // Check magic.
                uint32_t *header = (uint32_t *)mod_start;
                if (header[0] != MAGIC)
                {
                    log("> [Initrd] module at ", (int)mod_start, " has bad magic");
                }
                else
                {
                    base       = mod_start;
                    length     = mod_end - mod_start;
                    file_count = header[1];
                    entries    = (file_entry_t *)(mod_start + 8);
                    ready      = true;
                    log("> Initrd online: base=", (int)base, " files=", (int)file_count,
                        " size=", (int)length);
                    return;
                }
            }

            uint32_t step = (tag->size + 7) & ~7u;
            cursor += step;
        }

        if (!ready) log("> [Initrd] no MODULE tag found");
    }

    static int strncmp_(const char *a, const char *b, uint32_t n)
    {
        for (uint32_t i = 0; i < n; i++)
        {
            uint8_t ca = (uint8_t)a[i];
            uint8_t cb = (uint8_t)b[i];
            if (ca != cb) return (int)ca - (int)cb;
            if (ca == 0) return 0;
        }
        return 0;
    }

    /**
     * Find a file by exact name. Returns NULL if not found or not ready.
     */
    static file_entry_t *Find(const char *name)
    {
        if (!ready) return 0;
        for (uint32_t i = 0; i < file_count; i++)
        {
            if (strncmp_(entries[i].name, name, NAME_LEN) == 0)
                return &entries[i];
        }
        return 0;
    }

    /**
     * Returns a pointer to the in-memory bytes of a file. The caller must
     * not write through it (the ramdisk is conceptually read-only).
     */
    static const uint8_t *Data(file_entry_t *e)
    {
        if (!ready || e == 0) return 0;
        return (const uint8_t *)(base + e->offset);
    }

    static uint32_t Count()  { return ready ? file_count : 0; }
    static bool     Ready()  { return ready; }
    static uint32_t Length() { return length; }
    static file_entry_t *EntryAt(uint32_t i)
    {
        if (!ready || i >= file_count) return 0;
        return &entries[i];
    }
};

uint32_t              Initrd::base       = 0;
uint32_t              Initrd::length     = 0;
uint32_t              Initrd::file_count = 0;
Initrd::file_entry_t *Initrd::entries    = 0;
bool                  Initrd::ready      = false;

} // namespace System

#endif
