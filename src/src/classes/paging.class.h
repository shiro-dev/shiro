#ifndef PAGING_CLASS_H
#define PAGING_CLASS_H

#include <stdint.h>

namespace System
{
/**
 * System::Paging
 *
 * Brings up x86 paging. We use the simplest configuration that's still
 * a real MMU setup: 4 MiB pages via PSE (Page Size Extension), with the
 * entire 4 GiB virtual address space identity-mapped.
 *
 * Why this configuration:
 *   - 4 MiB pages keep the page directory tiny (1 entry = 4 MiB), so we
 *     only need a single 4 KiB-aligned 1024-entry table — no second-
 *     level page tables, no TLB pressure for kernel code.
 *   - Identity mapping (virt == phys) means every existing pointer in
 *     the kernel keeps working unchanged. Switching to higher-half or
 *     per-process address spaces is a future patch.
 *   - The MMU is now active, so a page fault becomes a real signal we
 *     can act on (System::Panic catches vector 14). When we eventually
 *     add userspace, this is the foundation it sits on.
 *
 * Steps performed by Start():
 *   1. Fill the page directory with 4 MiB identity mappings (P|RW|PS).
 *   2. Set CR4.PSE so 4 MiB pages are recognised.
 *   3. Load CR3 with the page directory's physical address.
 *   4. Set CR0.PG to actually enable paging.
 */
class Paging
{
public:
    // 4 KiB-aligned page directory in BSS.
    static uint32_t directory[1024] __attribute__((aligned(4096)));

    // Page directory entry flags.
    static const uint32_t PDE_PRESENT = 0x001;
    static const uint32_t PDE_RW      = 0x002;
    static const uint32_t PDE_USER    = 0x004;
    static const uint32_t PDE_PWT     = 0x008;
    static const uint32_t PDE_PCD     = 0x010;
    static const uint32_t PDE_ACCESS  = 0x020;
    static const uint32_t PDE_DIRTY   = 0x040;
    static const uint32_t PDE_PS      = 0x080; // 1 = 4 MiB page

    static bool ready;

    static void Start()
    {
        log("> Setting up identity-mapped paging (PSE / 4 MiB pages)");

        // 1. Identity-map all 4 GiB. Each PDE covers 4 MiB.
        for (uint32_t i = 0; i < 1024; i++)
        {
            uint32_t phys = i * 0x00400000u;
            directory[i] = phys | PDE_PRESENT | PDE_RW | PDE_PS;
        }

        // 2. CR4.PSE = 1.
        uint32_t cr4;
        asm volatile("mov %%cr4, %0" : "=r"(cr4));
        cr4 |= 0x00000010u;
        asm volatile("mov %0, %%cr4" : : "r"(cr4));

        // 3. CR3 = &directory.
        asm volatile("mov %0, %%cr3" : : "r"((uint32_t)directory));

        // 4. CR0.PG = 1.
        uint32_t cr0;
        asm volatile("mov %%cr0, %0" : "=r"(cr0));
        cr0 |= 0x80000000u;
        asm volatile("mov %0, %%cr0" : : "r"(cr0));

        ready = true;
        log("> Paging enabled (CR0.PG=1, CR4.PSE=1)");
    }

    static uint32_t GetCR0()
    {
        uint32_t v;
        asm volatile("mov %%cr0, %0" : "=r"(v));
        return v;
    }

    static uint32_t GetCR3()
    {
        uint32_t v;
        asm volatile("mov %%cr3, %0" : "=r"(v));
        return v;
    }

    static uint32_t GetCR4()
    {
        uint32_t v;
        asm volatile("mov %%cr4, %0" : "=r"(v));
        return v;
    }

    static bool IsEnabled() { return ready && (GetCR0() & 0x80000000u); }
};

uint32_t Paging::directory[1024] __attribute__((aligned(4096)));
bool     Paging::ready = false;

} // namespace System

#endif
