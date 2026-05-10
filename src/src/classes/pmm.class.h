#ifndef PMM_CLASS_H
#define PMM_CLASS_H

#include <stdint.h>
#include <stddef.h>
#include <multiboot2.h>

extern "C" uint8_t _kernel_start[];
extern "C" uint8_t _kernel_end[];

namespace System
{
/**
 * System::PMM (Physical Memory Manager)
 *
 * Owns the bookkeeping for physical RAM at page granularity. One bit in
 * `bitmap[]` represents one 4 KiB page:
 *      0 = free, 1 = allocated/reserved.
 *
 * The bitmap is sized for 4 GiB of address space (1 << 20 pages = 1 Mbit
 * = 128 KiB). Most of it stays in the "reserved" state forever on a
 * machine with less RAM, which is the safe default.
 *
 * PMM::Start() does the bring-up:
 *   1. Mark every page as reserved.
 *   2. Walk the multiboot mmap; for each AVAILABLE region, clear those
 *      pages in the bitmap (i.e. mark them free).
 *   3. Re-reserve the range the kernel + heap occupies so we don't hand
 *      it out to drivers later.
 *
 * After Start(), AllocPage() returns the next free page in O(N) by linear
 * scan. Plenty good for a hobby kernel.
 */
class PMM
{
public:
    static const uint32_t PAGE_SIZE = 4096;
    static const uint32_t MAX_PAGES = 1u << 20; // 4 GiB / 4 KiB
    static const uint32_t BITMAP_BYTES = MAX_PAGES / 8;

    static uint8_t  bitmap[BITMAP_BYTES];
    static uint32_t total_pages;
    static uint32_t free_pages;
    static uint32_t used_pages;
    static uint32_t reserved_pages;
    static bool     ready;

    static inline void SetUsed(uint32_t page)
    {
        bitmap[page >> 3] |= (uint8_t)(1u << (page & 7));
    }

    static inline void SetFree(uint32_t page)
    {
        bitmap[page >> 3] &= (uint8_t)~(1u << (page & 7));
    }

    static inline bool IsUsed(uint32_t page)
    {
        return (bitmap[page >> 3] >> (page & 7)) & 1u;
    }

    /**
     * Mark a contiguous physical range as available.
     * `addr`/`size` are in bytes; rounded inwards so partial pages stay
     * reserved (we never want to hand out a page that isn't fully usable).
     */
    static void MarkRangeFree(uint64_t addr, uint64_t size)
    {
        uint64_t start = (addr + PAGE_SIZE - 1) & ~(uint64_t)(PAGE_SIZE - 1);
        uint64_t end   = (addr + size) & ~(uint64_t)(PAGE_SIZE - 1);
        if (end <= start) return;

        uint32_t first = (uint32_t)(start / PAGE_SIZE);
        uint32_t last  = (uint32_t)(end   / PAGE_SIZE); // exclusive
        if (last > MAX_PAGES) last = MAX_PAGES;

        for (uint32_t p = first; p < last; p++)
        {
            if (IsUsed(p))
            {
                SetFree(p);
                free_pages++;
                if (reserved_pages > 0) reserved_pages--;
            }
        }
    }

    /**
     * Mark a contiguous physical range as in-use (kernel image, heap, MMIO).
     */
    static void MarkRangeUsed(uint64_t addr, uint64_t size)
    {
        uint64_t start = addr & ~(uint64_t)(PAGE_SIZE - 1);
        uint64_t end   = (addr + size + PAGE_SIZE - 1) & ~(uint64_t)(PAGE_SIZE - 1);
        if (end <= start) return;

        uint32_t first = (uint32_t)(start / PAGE_SIZE);
        uint32_t last  = (uint32_t)(end   / PAGE_SIZE);
        if (last > MAX_PAGES) last = MAX_PAGES;

        for (uint32_t p = first; p < last; p++)
        {
            if (!IsUsed(p))
            {
                SetUsed(p);
                if (free_pages > 0) free_pages--;
                used_pages++;
            }
        }
    }

    /**
     * System::PMM::Start(mbi)
     *
     * `mbi` is the multiboot2 info pointer (same one shiro_main received).
     * Pulls the mmap, populates the bitmap, then carves out the kernel +
     * heap range so AllocPage never hands out memory we're already using.
     */
    static void Start(void *mbi, uint32_t kernel_start, uint32_t kernel_end_plus_heap)
    {
        // 1. Reserve everything.
        for (uint32_t i = 0; i < BITMAP_BYTES; i++) bitmap[i] = 0xFF;
        total_pages    = MAX_PAGES;
        used_pages     = 0;
        free_pages     = 0;
        reserved_pages = MAX_PAGES;

        if (mbi == 0)
        {
            log("> [PMM] no multiboot info — pmm disabled");
            return;
        }

        // 2. Walk mmap and free available regions.
        uint8_t *cursor = (uint8_t *)mbi + 8;
        for (;;)
        {
            mb2_tag *tag = (mb2_tag *)cursor;
            if (tag->type == MB2_TAG_END) break;

            if (tag->type == MB2_TAG_MMAP)
            {
                mb2_tag_mmap *t = (mb2_tag_mmap *)tag;
                uint32_t entries_size = t->size - 16;
                uint32_t count = entries_size / t->entry_size;
                uint8_t *e = (uint8_t *)t->entries;
                for (uint32_t i = 0; i < count; i++)
                {
                    mb2_mmap_entry *entry = (mb2_mmap_entry *)(e + i * t->entry_size);
                    if (entry->type == MB2_MEMORY_AVAILABLE)
                    {
                        MarkRangeFree(entry->base_addr, entry->length);
                    }
                }
            }

            uint32_t step = (tag->size + 7) & ~7u;
            cursor += step;
        }

        // 3. Re-reserve kernel + heap region.
        MarkRangeUsed(kernel_start, kernel_end_plus_heap - kernel_start);

        // 4. Re-reserve the very first MiB (BIOS, IVT in real-mode, VGA, etc).
        MarkRangeUsed(0, 0x100000);

        ready = true;

        log("> PMM online: total_pages=", (int)total_pages,
            " free=", (int)free_pages, " used=", (int)used_pages);
    }

    /**
     * System::PMM::AllocPage()
     *
     * Returns the physical address of a freshly-marked page, or 0 on OOM.
     */
    static uint32_t AllocPage()
    {
        if (!ready) return 0;
        // Skip whole bytes that are fully allocated for a faster scan.
        for (uint32_t byte_i = 0; byte_i < BITMAP_BYTES; byte_i++)
        {
            if (bitmap[byte_i] == 0xFF) continue;
            for (uint32_t bit = 0; bit < 8; bit++)
            {
                if (!(bitmap[byte_i] & (1u << bit)))
                {
                    uint32_t page = byte_i * 8 + bit;
                    SetUsed(page);
                    free_pages--;
                    used_pages++;
                    return page * PAGE_SIZE;
                }
            }
        }
        return 0;
    }

    /**
     * System::PMM::FreePage(physical_addr)
     */
    static void FreePage(uint32_t physical_addr)
    {
        if (!ready) return;
        uint32_t page = physical_addr / PAGE_SIZE;
        if (page >= MAX_PAGES) return;
        if (!IsUsed(page)) return; // double free
        SetFree(page);
        free_pages++;
        used_pages--;
    }

    static uint32_t TotalPages() { return total_pages; }
    static uint32_t FreePages()  { return free_pages; }
    static uint32_t UsedPages()  { return used_pages; }
};

uint8_t  PMM::bitmap[PMM::BITMAP_BYTES];
uint32_t PMM::total_pages    = 0;
uint32_t PMM::free_pages     = 0;
uint32_t PMM::used_pages     = 0;
uint32_t PMM::reserved_pages = 0;
bool     PMM::ready          = false;

} // namespace System

#endif
