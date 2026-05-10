#ifndef HEAP_CLASS_H
#define HEAP_CLASS_H

#include <stdint.h>
#include <stddef.h>

/**
 * Provided by the linker. _kernel_end marks the first address past the
 * kernel image (page-aligned). The heap lives above that.
 */
extern "C" uint8_t _kernel_start[];
extern "C" uint8_t _kernel_end[];

namespace System
{
/**
 * System::Heap
 *
 * First-fit free-list allocator covering a contiguous region of physical
 * memory placed immediately after the kernel image.
 *
 *   [block_header_t][user payload][block_header_t][user payload]...
 *
 * Each block carries its size (payload-only), a `used` flag, and a pointer
 * to the next block in *physical* order. Walking that list is the entire
 * algorithm. On free we coalesce with neighbours so fragmentation doesn't
 * grow without bound.
 *
 * Trade-offs (intentional, for a hobby kernel):
 *   - Linear walk (O(n) on alloc/free); fine for the few-hundred-allocation
 *     workloads the kernel sees.
 *   - No alignment guarantees beyond sizeof(void *).
 *   - Single global heap; no per-CPU arenas.
 */
class Heap
{
public:
    struct block_header_t
    {
        uint32_t        magic;   // sanity check on free
        uint32_t        size;    // payload bytes, excluding this header
        uint32_t        used;    // 0 = free, 1 = in use
        block_header_t *next;    // physically next block (or NULL)
    };

    static const uint32_t HEAP_MAGIC = 0x5481B07Eu; // "SHRO" mangled
    static const uint32_t HEAP_SIZE  = 0x00400000u; // 4 MiB heap

    static block_header_t *first;
    static uint32_t        base;
    static uint32_t        end;
    static uint32_t        bytes_used;
    static uint32_t        bytes_free;
    static uint32_t        alloc_count;
    static uint32_t        free_count;
    static bool            ready;

    /**
     * System::Heap::Start(base_hint)
     *
     * Carves out a single huge free block at [base .. base+HEAP_SIZE).
     * `base_hint` lets the caller bump the start past whatever else lives
     * up there (the initrd module GRUB drops right after our BSS, etc.).
     * If the hint is below `_kernel_end`, the kernel-end is used instead.
     * Must be called exactly once, before any kmalloc/new.
     */
    static void Start(uint32_t base_hint = 0)
    {
        uint32_t kend = (uint32_t)_kernel_end;
        base = (base_hint > kend) ? base_hint : kend;
        // 4 KiB-align — keeps the heap on page boundaries for future paging
        // tricks and is good for cache lines anyway.
        if (base & 0xFFF) base = (base + 0xFFF) & ~0xFFFu;
        end = base + HEAP_SIZE;

        first = (block_header_t *)base;
        first->magic = HEAP_MAGIC;
        first->size  = HEAP_SIZE - sizeof(block_header_t);
        first->used  = 0;
        first->next  = 0;

        bytes_used  = 0;
        bytes_free  = first->size;
        alloc_count = 0;
        free_count  = 0;
        ready       = true;

        log("> Heap online: base=", (int)base, " end=", (int)end,
            " size(KiB)=", (int)(HEAP_SIZE / 1024));
    }

    /**
     * Round size up to the implementation's minimum allocation granularity.
     * Keeping payloads multiples of 8 means split-block headers stay aligned.
     */
    static uint32_t Round(uint32_t n)
    {
        if (n < 8) n = 8;
        return (n + 7) & ~7u;
    }

    /**
     * System::Heap::Alloc(size)
     *
     * First-fit search. Splits the chosen block if it has slack to spare.
     * Returns NULL if out of memory or before Start() ran.
     */
    static void *Alloc(uint32_t size)
    {
        if (!ready) return 0;
        size = Round(size);

        for (block_header_t *b = first; b != 0; b = b->next)
        {
            if (b->used || b->size < size) continue;

            // Split iff the leftover is big enough to host another header
            // plus a non-trivial payload.
            uint32_t leftover = b->size - size;
            if (leftover > sizeof(block_header_t) + 16)
            {
                block_header_t *tail = (block_header_t *)((uint8_t *)b + sizeof(block_header_t) + size);
                tail->magic = HEAP_MAGIC;
                tail->size  = leftover - sizeof(block_header_t);
                tail->used  = 0;
                tail->next  = b->next;

                b->size = size;
                b->next = tail;
                bytes_free -= sizeof(block_header_t);
            }

            b->used = 1;
            bytes_used += b->size;
            bytes_free -= b->size;
            alloc_count++;

            return (void *)((uint8_t *)b + sizeof(block_header_t));
        }

        log("> [HEAP] OOM requesting ", (int)size, " bytes");
        return 0;
    }

    /**
     * System::Heap::Free(p)
     *
     * Marks p's block free and coalesces forward (and, via a linear search,
     * backward) so contiguous free blocks merge.
     */
    static void Free(void *p)
    {
        if (!ready || p == 0) return;

        block_header_t *b = (block_header_t *)((uint8_t *)p - sizeof(block_header_t));
        if (b->magic != HEAP_MAGIC)
        {
            log("> [HEAP] free of bad pointer ", (int)(uint32_t)p);
            return;
        }
        if (!b->used)
        {
            log("> [HEAP] double free at ", (int)(uint32_t)p);
            return;
        }

        b->used = 0;
        bytes_used -= b->size;
        bytes_free += b->size;
        free_count++;

        // Forward coalesce.
        if (b->next && !b->next->used)
        {
            block_header_t *n = b->next;
            b->size += sizeof(block_header_t) + n->size;
            b->next = n->next;
            bytes_free += sizeof(block_header_t);
        }

        // Backward coalesce — find predecessor by linear walk.
        block_header_t *prev = 0;
        for (block_header_t *cur = first; cur && cur != b; cur = cur->next)
            prev = cur;
        if (prev && !prev->used)
        {
            prev->size += sizeof(block_header_t) + b->size;
            prev->next = b->next;
            bytes_free += sizeof(block_header_t);
        }
    }

    static uint32_t BytesUsed() { return bytes_used; }
    static uint32_t BytesFree() { return bytes_free; }
    static uint32_t AllocCount() { return alloc_count; }
    static uint32_t FreeCount() { return free_count; }
    static uint32_t Base() { return base; }
    static uint32_t End() { return end; }
};

Heap::block_header_t *Heap::first       = 0;
uint32_t              Heap::base        = 0;
uint32_t              Heap::end         = 0;
uint32_t              Heap::bytes_used  = 0;
uint32_t              Heap::bytes_free  = 0;
uint32_t              Heap::alloc_count = 0;
uint32_t              Heap::free_count  = 0;
bool                  Heap::ready       = false;

} // namespace System

#endif
