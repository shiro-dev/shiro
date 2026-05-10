#ifndef COMMANDS_CLASS_H
#define COMMANDS_CLASS_H

#include <stdint.h>
#include <stddef.h>
#include "shell.class.h"
#include "heap.class.h"
#include "pmm.class.h"
#include "paging.class.h"
#include "scheduler.class.h"
#include "initrd.class.h"
#include "shiro.class.h"
#include "timer.class.h"

namespace Applications
{
/**
 * Applications::Commands
 *
 * Simple line-based command shell. Each builtin is a static method with
 * the signature `void(const char *args)`. Dispatch() looks the first
 * word up in the table and calls the matching function.
 *
 * Output goes through the Shell's WriteC, which writes one line to the
 * scrollback area and shifts older lines up.
 */
class Commands
{
public:
    struct entry
    {
        const char *name;
        void (*fn)(const char *args);
        const char *desc;
    };

    /**
     * Print a single line to the current terminal's scrollback.
     * Uses update=false so multi-line output isn't penalised by a full
     * UpdateScreen between every WriteC; Dispatch flushes once at the end.
     */
    static void Print(const char *s)
    {
        Shell *shell = Shell::GetInstance();
        shell->WriteC(s, false);
    }

    // CP437 line-drawing characters used by the box helpers.
    static const char BOX_HORIZONTAL = (char)0xC4; // ─
    static const char BOX_VERTICAL   = (char)0xB3; // │
    static const char BOX_TL         = (char)0xDA; // ┌
    static const char BOX_TR         = (char)0xBF; // ┐
    static const char BOX_BL         = (char)0xC0; // └
    static const char BOX_BR         = (char)0xD9; // ┘

    static const int BOX_WIDTH = 78; // boxes use cols 0..77

    /**
     * Print the top border of a box, with an embedded title.
     *   ┌─ Title ──────────────────────┐
     */
    static void BoxTop(const char *title)
    {
        char buf[82];
        int j = 0;
        buf[j++] = BOX_TL;
        buf[j++] = BOX_HORIZONTAL;
        buf[j++] = ' ';
        for (int i = 0; title[i] && j < BOX_WIDTH - 2; i++)
            buf[j++] = title[i];
        buf[j++] = ' ';
        while (j < BOX_WIDTH - 1) buf[j++] = BOX_HORIZONTAL;
        buf[j++] = BOX_TR;
        buf[j] = '\0';
        Print(buf);
    }

    /**
     * Print a content line of a box.
     *   │  whatever you wanted...      │
     */
    static void BoxLine(const char *content)
    {
        char buf[82];
        int j = 0;
        buf[j++] = BOX_VERTICAL;
        buf[j++] = ' ';
        buf[j++] = ' ';
        for (int i = 0; content[i] && j < BOX_WIDTH - 1; i++)
            buf[j++] = content[i];
        while (j < BOX_WIDTH - 1) buf[j++] = ' ';
        buf[j++] = BOX_VERTICAL;
        buf[j] = '\0';
        Print(buf);
    }

    /**
     * Print an empty content line of a box.
     */
    static void BoxBlank()
    {
        BoxLine("");
    }

    /**
     * Print the bottom border of a box.
     *   └──────────────────────────────┘
     */
    static void BoxBottom()
    {
        char buf[82];
        int j = 0;
        buf[j++] = BOX_BL;
        while (j < BOX_WIDTH - 1) buf[j++] = BOX_HORIZONTAL;
        buf[j++] = BOX_BR;
        buf[j] = '\0';
        Print(buf);
    }

    /**
     * strcmp — case-sensitive byte equality.
     */
    static int Strcmp(const char *a, const char *b)
    {
        while (*a && *a == *b) { a++; b++; }
        return (int)(uint8_t)*a - (int)(uint8_t)*b;
    }

    /**
     * Append `s` to `dst` starting at index *idx*. Writes the trailing
     * '\0'. Returns the new index. Refuses to overflow `cap` (with NUL).
     */
    static int AppendStr(char *dst, int idx, int cap, const char *s)
    {
        while (*s && idx < cap - 1) dst[idx++] = *s++;
        dst[idx] = '\0';
        return idx;
    }

    /**
     * Decimal int → string, in the caller-provided buffer. Returns the
     * number of chars written (excluding NUL).
     */
    static int IntToStr(uint32_t v, char *buf, int cap)
    {
        if (cap < 2) { if (cap > 0) buf[0] = '\0'; return 0; }
        if (v == 0) { buf[0] = '0'; buf[1] = '\0'; return 1; }
        char tmp[16]; int n = 0;
        while (v && n < 15) { tmp[n++] = (char)('0' + (v % 10)); v /= 10; }
        int j = 0;
        while (n > 0 && j < cap - 1) buf[j++] = tmp[--n];
        buf[j] = '\0';
        return j;
    }

    /**
     * Hex int → string, lowercase, prefixed "0x". Returns chars written.
     */
    static int HexToStr(uint32_t v, char *buf, int cap)
    {
        if (cap < 4) { if (cap > 0) buf[0] = '\0'; return 0; }
        const char *hex = "0123456789abcdef";
        buf[0] = '0'; buf[1] = 'x';
        char tmp[8]; int n = 0;
        if (v == 0) { tmp[n++] = '0'; }
        else { while (v && n < 8) { tmp[n++] = hex[v & 0xf]; v >>= 4; } }
        int j = 2;
        while (n > 0 && j < cap - 1) buf[j++] = tmp[--n];
        buf[j] = '\0';
        return j;
    }

    // ---------------- Builtins ----------------

    static void cmd_help(const char *args);
    static void cmd_ver(const char *args);
    static void cmd_mem(const char *args);
    static void cmd_pmm(const char *args);
    static void cmd_paging(const char *args);
    static void cmd_tasks(const char *args);
    static void cmd_ls(const char *args);
    static void cmd_cat(const char *args);
    static void cmd_peek(const char *args);
    static void cmd_dumpmem(const char *args);
    static void cmd_uptime(const char *args);
    static void cmd_ticks(const char *args);
    static void cmd_clear(const char *args);
    static void cmd_echo(const char *args);
    static void cmd_halt(const char *args);
    static void cmd_reboot(const char *args);
    static void cmd_panic(const char *args);
    static void cmd_cpuid(const char *args);
    static void cmd_date(const char *args);

    static const entry *Table()
    {
        static const entry table[] = {
            {"help",   cmd_help,   "List all commands"},
            {"ver",    cmd_ver,    "Kernel version & build info"},
            {"mem",    cmd_mem,    "Kernel heap stats"},
            {"pmm",    cmd_pmm,    "Physical memory bitmap stats"},
            {"paging", cmd_paging, "MMU / page directory state"},
            {"tasks",  cmd_tasks,  "List kernel tasks (scheduler)"},
            {"ls",     cmd_ls,     "List files in the initrd ramdisk"},
            {"cat",    cmd_cat,    "Print a file from the ramdisk"},
            {"peek",   cmd_peek,   "Read a 32-bit value: peek <hex-addr>"},
            {"dumpmem",cmd_dumpmem,"Hexdump 64 bytes: dumpmem <hex-addr>"},
            {"uptime", cmd_uptime, "Time since boot"},
            {"ticks",  cmd_ticks,  "Raw timer tick count"},
            {"date",   cmd_date,   "Approximate timestamp (uptime-based)"},
            {"clear",  cmd_clear,  "Clear current terminal"},
            {"echo",   cmd_echo,   "Print arguments"},
            {"cpuid",  cmd_cpuid,  "CPU vendor & feature flags"},
            {"halt",   cmd_halt,   "Halt the CPU forever"},
            {"reboot", cmd_reboot, "Reboot via triple-fault"},
            {"panic",  cmd_panic,  "Trigger an intentional kernel panic"},
            {0, 0, 0}
        };
        return table;
    }

    /**
     * Applications::Commands::Dispatch(line)
     *
     * Parses `line` into name + args and invokes the matching builtin.
     * Trailing newlines / leading whitespace are tolerated.
     */
    static void Dispatch(const char *line)
    {
        // Skip leading whitespace.
        while (*line == ' ' || *line == '\t') line++;
        if (*line == '\0' || *line == '\n') return;

        // Extract command name.
        char name[32];
        int i = 0;
        while (line[i] && line[i] != ' ' && line[i] != '\t' &&
               line[i] != '\n' && i < 31)
        {
            name[i] = line[i];
            i++;
        }
        name[i] = '\0';

        // Move past the name + any whitespace to find args.
        const char *args = line + i;
        while (*args == ' ' || *args == '\t') args++;

        for (const entry *e = Table(); e->name; e++)
        {
            if (Strcmp(name, e->name) == 0)
            {
                e->fn(args);
                return;
            }
        }

        char buf[80];
        int j = 0;
        j = AppendStr(buf, j, 80, "shiro: command not found: ");
        AppendStr(buf, j, 80, name);
        Print(buf);
    }
};

// ---------------- Builtin definitions ----------------

inline void Commands::cmd_help(const char *args)
{
    (void)args;
    Print("Available commands:");
    char buf[80];
    for (const entry *e = Table(); e->name; e++)
    {
        int j = 0;
        j = AppendStr(buf, j, 80, "  ");
        j = AppendStr(buf, j, 80, e->name);
        // pad to column 12
        while (j < 12 && j < 79) buf[j++] = ' ';
        j = AppendStr(buf, j, 80, "- ");
        j = AppendStr(buf, j, 80, e->desc);
        buf[j] = '\0';
        Print(buf);
    }
}

inline void Commands::cmd_ver(const char *args)
{
    (void)args;
    Print("Shiro - Operating System");
    Print("  Version: 0.5 (development)");
    Print("  Compiler: i686-elf-g++ 13.2.0");
    Print("  Boot:     Multiboot2 / GRUB");
    Print("  Build:    " __DATE__ " " __TIME__);
}

inline void Commands::cmd_mem(const char *args)
{
    (void)args;
    char buf[80]; int j;

    BoxTop("Kernel Heap (System::Heap)");

    j = 0;
    j = AppendStr(buf, j, 80, "base       = ");
    j += HexToStr(System::Heap::Base(), buf + j, 80 - j);
    j = AppendStr(buf, j, 80, "    end = ");
    j += HexToStr(System::Heap::End(), buf + j, 80 - j);
    BoxLine(buf);

    j = 0;
    j = AppendStr(buf, j, 80, "used       = ");
    j += IntToStr(System::Heap::BytesUsed(), buf + j, 80 - j);
    j = AppendStr(buf, j, 80, " bytes    free = ");
    j += IntToStr(System::Heap::BytesFree(), buf + j, 80 - j);
    j = AppendStr(buf, j, 80, " bytes");
    BoxLine(buf);

    j = 0;
    j = AppendStr(buf, j, 80, "allocs     = ");
    j += IntToStr(System::Heap::AllocCount(), buf + j, 80 - j);
    j = AppendStr(buf, j, 80, "    frees = ");
    j += IntToStr(System::Heap::FreeCount(), buf + j, 80 - j);
    BoxLine(buf);

    BoxBottom();

    BoxTop("Multiboot Info");
    System::Shiro *shiro = System::Shiro::GetInstance();

    j = 0;
    j = AppendStr(buf, j, 80, "mem_lower  = ");
    j += IntToStr(shiro->multiboot.mem_lower, buf + j, 80 - j);
    j = AppendStr(buf, j, 80, " KiB    mem_upper = ");
    j += IntToStr(shiro->multiboot.mem_upper, buf + j, 80 - j);
    j = AppendStr(buf, j, 80, " KiB");
    BoxLine(buf);

    j = 0;
    j = AppendStr(buf, j, 80, "mmap avail = ");
    j += IntToStr((uint32_t)(shiro->multiboot.mmap_total_available / (1024 * 1024)),
                  buf + j, 80 - j);
    j = AppendStr(buf, j, 80, " MiB across ");
    j += IntToStr(shiro->multiboot.mmap_count, buf + j, 80 - j);
    j = AppendStr(buf, j, 80, " regions");
    BoxLine(buf);

    BoxBottom();
}

inline void Commands::cmd_pmm(const char *args)
{
    (void)args;
    char buf[80]; int j;
    BoxTop("Physical Memory Manager");

    BoxLine("page size = 4 KiB");

    j = 0;
    j = AppendStr(buf, j, 80, "total = ");
    j += IntToStr(System::PMM::TotalPages(), buf + j, 80 - j);
    j = AppendStr(buf, j, 80, " pages (");
    j += IntToStr(System::PMM::TotalPages() / 256, buf + j, 80 - j);
    j = AppendStr(buf, j, 80, " MiB tracked)");
    BoxLine(buf);

    j = 0;
    j = AppendStr(buf, j, 80, "used  = ");
    j += IntToStr(System::PMM::UsedPages(), buf + j, 80 - j);
    j = AppendStr(buf, j, 80, " pages (");
    j += IntToStr(System::PMM::UsedPages() * 4, buf + j, 80 - j);
    j = AppendStr(buf, j, 80, " KiB)");
    BoxLine(buf);

    j = 0;
    j = AppendStr(buf, j, 80, "free  = ");
    j += IntToStr(System::PMM::FreePages(), buf + j, 80 - j);
    j = AppendStr(buf, j, 80, " pages (");
    j += IntToStr(System::PMM::FreePages() / 256, buf + j, 80 - j);
    j = AppendStr(buf, j, 80, " MiB)");
    BoxLine(buf);

    BoxBottom();
}

/**
 * Parse a hex string ("0xABCD" or "ABCD") into a uint32. Returns true on
 * success and writes the value to *out. Stops at first non-hex char.
 */
inline bool ParseHex(const char *s, uint32_t *out)
{
    if (!s || !*s) return false;
    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) s += 2;
    uint32_t v = 0; bool any = false;
    while (*s)
    {
        char c = *s;
        uint32_t d;
        if (c >= '0' && c <= '9') d = (uint32_t)(c - '0');
        else if (c >= 'a' && c <= 'f') d = (uint32_t)(c - 'a' + 10);
        else if (c >= 'A' && c <= 'F') d = (uint32_t)(c - 'A' + 10);
        else break;
        v = (v << 4) | d;
        any = true;
        s++;
    }
    if (!any) return false;
    *out = v;
    return true;
}

inline void Commands::cmd_peek(const char *args)
{
    uint32_t addr;
    if (!ParseHex(args, &addr))
    {
        Print("usage: peek <hex-address>");
        Print("example: peek 0xB8000");
        return;
    }
    // Force 4-byte alignment.
    addr &= ~3u;
    uint32_t value = *(volatile uint32_t *)addr;

    char buf[80]; int j = 0;
    j = AppendStr(buf, j, 80, "[");
    j += HexToStr(addr, buf + j, 80 - j);
    j = AppendStr(buf, j, 80, "] = ");
    j += HexToStr(value, buf + j, 80 - j);
    Print(buf);
}

inline void Commands::cmd_dumpmem(const char *args)
{
    uint32_t addr;
    if (!ParseHex(args, &addr))
    {
        Print("usage: dumpmem <hex-address>");
        return;
    }
    addr &= ~0xFu; // align to 16 bytes for clean rows

    char buf[80];
    const uint8_t *p = (const uint8_t *)addr;
    for (int row = 0; row < 4; row++)
    {
        int j = 0;
        j += HexToStr(addr + (uint32_t)(row * 16), buf + j, 80 - j);
        j = AppendStr(buf, j, 80, ":  ");
        // Hex bytes
        for (int col = 0; col < 16; col++)
        {
            uint8_t b = p[row * 16 + col];
            const char *hex = "0123456789abcdef";
            buf[j++] = hex[b >> 4];
            buf[j++] = hex[b & 0xF];
            buf[j++] = ' ';
            if (col == 7) buf[j++] = ' ';
        }
        buf[j++] = ' ';
        // ASCII column
        for (int col = 0; col < 16; col++)
        {
            uint8_t b = p[row * 16 + col];
            buf[j++] = (b >= 0x20 && b < 0x7F) ? (char)b : '.';
        }
        buf[j] = '\0';
        Print(buf);
    }
}

inline void Commands::cmd_ls(const char *args)
{
    (void)args;
    if (!System::Initrd::Ready())
    {
        Print("ls: initrd not loaded");
        return;
    }

    char buf[80]; int j;

    j = 0;
    j = AppendStr(buf, j, 80, "Initrd contents (");
    j += IntToStr(System::Initrd::Count(), buf + j, 80 - j);
    j = AppendStr(buf, j, 80, " files, ");
    j += IntToStr(System::Initrd::Length(), buf + j, 80 - j);
    j = AppendStr(buf, j, 80, " bytes total):");
    Print(buf);

    for (uint32_t i = 0; i < System::Initrd::Count(); i++)
    {
        System::Initrd::file_entry_t *e = System::Initrd::EntryAt(i);
        if (!e) continue;

        j = 0;
        j = AppendStr(buf, j, 80, "  ");
        j += IntToStr(e->size, buf + j, 80 - j);
        // pad to col 10
        while (j < 10 && j < 79) buf[j++] = ' ';
        j = AppendStr(buf, j, 80, "  ");
        j = AppendStr(buf, j, 80, e->name);
        buf[j] = '\0';
        Print(buf);
    }
}

inline void Commands::cmd_cat(const char *args)
{
    if (!*args)
    {
        Print("usage: cat <filename>");
        return;
    }
    if (!System::Initrd::Ready())
    {
        Print("cat: initrd not loaded");
        return;
    }

    System::Initrd::file_entry_t *e = System::Initrd::Find(args);
    if (!e)
    {
        char buf[80]; int j = 0;
        j = AppendStr(buf, j, 80, "cat: file not found: ");
        j = AppendStr(buf, j, 80, args);
        Print(buf);
        return;
    }

    const uint8_t *data = System::Initrd::Data(e);
    if (!data) { Print("cat: read error"); return; }

    // Print line by line. Files in our ramdisk are plain text with \n
    // line endings; we lift one line at a time into a stack buffer and
    // hand it to Print (which scrolls).
    char line[80];
    uint32_t li = 0;
    for (uint32_t i = 0; i < e->size; i++)
    {
        uint8_t b = data[i];
        if (b == '\r') continue;
        if (b == '\n' || li >= 79)
        {
            line[li] = '\0';
            Print(line);
            li = 0;
            if (b != '\n')
            {
                // We wrapped because the line was too long; replay the byte
                // we just rejected.
                line[li++] = (char)b;
            }
            continue;
        }
        line[li++] = (char)b;
    }
    if (li > 0)
    {
        line[li] = '\0';
        Print(line);
    }
}

inline void Commands::cmd_tasks(const char *args)
{
    (void)args;
    char buf[80]; int j;

    BoxTop("Kernel Scheduler (preemptive @ 50ms)");

    j = 0;
    j = AppendStr(buf, j, 80, "id  state     name             yields");
    BoxLine(buf);

    for (uint32_t i = 0; i < System::Scheduler::MAX_TASKS; i++)
    {
        System::Scheduler::task_t *t = &System::Scheduler::tasks[i];
        if (t->state == System::Scheduler::TASK_UNUSED) continue;

        const char *st = "?";
        if (t->state == System::Scheduler::TASK_READY)    st = "READY   ";
        if (t->state == System::Scheduler::TASK_SLEEPING) st = "SLEEPING";
        if (t->state == System::Scheduler::TASK_DONE)     st = "DONE    ";

        j = 0;
        // Marker for currently-running task
        bool is_current = (t->id == (uint32_t)System::Scheduler::CurrentId());
        j = AppendStr(buf, j, 80, is_current ? ">> " : "   ");
        j -= 1; // overwrite trailing nul with leading column
        j += IntToStr(t->id, buf + j, 80 - j);
        while (j < 4 && j < 79) buf[j++] = ' ';
        j = AppendStr(buf, j, 80, st);
        j = AppendStr(buf, j, 80, "  ");
        j = AppendStr(buf, j, 80, t->name);
        while (j < 33 && j < 79) buf[j++] = ' ';
        j += IntToStr(t->yields, buf + j, 80 - j);
        buf[j] = '\0';
        BoxLine(buf);
    }

    j = 0;
    j = AppendStr(buf, j, 80, "running: ");
    j = AppendStr(buf, j, 80, System::Scheduler::CurrentName());
    BoxLine(buf);

    BoxBottom();
}

inline void Commands::cmd_paging(const char *args)
{
    (void)args;
    char buf[80]; int j;

    BoxTop("Paging / MMU");

    j = 0;
    j = AppendStr(buf, j, 80, "enabled    = ");
    j = AppendStr(buf, j, 80, System::Paging::IsEnabled() ? "yes (CR0.PG=1)" : "no");
    BoxLine(buf);

    j = 0;
    j = AppendStr(buf, j, 80, "CR0        = ");
    j += HexToStr(System::Paging::GetCR0(), buf + j, 80 - j);
    BoxLine(buf);

    j = 0;
    j = AppendStr(buf, j, 80, "CR3        = ");
    j += HexToStr(System::Paging::GetCR3(), buf + j, 80 - j);
    j = AppendStr(buf, j, 80, "  (page directory)");
    BoxLine(buf);

    j = 0;
    j = AppendStr(buf, j, 80, "CR4        = ");
    j += HexToStr(System::Paging::GetCR4(), buf + j, 80 - j);
    j = AppendStr(buf, j, 80, "  (PSE bit ");
    j = AppendStr(buf, j, 80, (System::Paging::GetCR4() & 0x10) ? "on)" : "off)");
    BoxLine(buf);

    BoxLine("page size  = 4 MiB (PSE)");
    BoxLine("mapped     = 4 GiB identity (1024 x 4 MiB)");

    BoxBottom();
}

inline void Commands::cmd_uptime(const char *args)
{
    (void)args;
    uint32_t ticks = System::Drivers::Timer::GetTicks();
    uint32_t hz    = System::Drivers::Timer::TICK_HZ;
    uint32_t total_s = ticks / hz;
    uint32_t hours   = total_s / 3600;
    uint32_t mins    = (total_s % 3600) / 60;
    uint32_t secs    = total_s % 60;

    char buf[80]; int j = 0;
    j = AppendStr(buf, j, 80, "uptime: ");
    j += IntToStr(hours, buf + j, 80 - j);
    j = AppendStr(buf, j, 80, "h ");
    j += IntToStr(mins, buf + j, 80 - j);
    j = AppendStr(buf, j, 80, "m ");
    j += IntToStr(secs, buf + j, 80 - j);
    j = AppendStr(buf, j, 80, "s  (");
    j += IntToStr(ticks, buf + j, 80 - j);
    j = AppendStr(buf, j, 80, " ticks @ ");
    j += IntToStr(hz, buf + j, 80 - j);
    j = AppendStr(buf, j, 80, " Hz)");
    Print(buf);
}

inline void Commands::cmd_ticks(const char *args)
{
    (void)args;
    char buf[64]; int j = 0;
    j = AppendStr(buf, j, 64, "ticks = ");
    j += IntToStr(System::Drivers::Timer::GetTicks(), buf + j, 64 - j);
    Print(buf);
}

inline void Commands::cmd_clear(const char *args)
{
    (void)args;
    Shell *shell = Shell::GetInstance();
    shell->ClearTerminal(shell->current_terminal);
}

inline void Commands::cmd_echo(const char *args)
{
    Print(*args ? args : "");
}

inline void Commands::cmd_halt(const char *args)
{
    (void)args;
    Print("Halting CPU. Reset the machine to continue.");
    asm volatile("cli");
    for (;;) asm volatile("hlt");
}

inline void Commands::cmd_reboot(const char *args)
{
    (void)args;
    Print("Rebooting...");
    // Try the keyboard controller's CPU reset line first.
    // Wait for the input buffer to clear.
    while (inb(0x64) & 0x02) { /* spin */ }
    outb(0x64, 0xFE);
    // Fallback: triple fault by loading a null IDT and firing an int.
    struct { uint16_t limit; uint32_t base; } __attribute__((packed)) null_idt = {0, 0};
    asm volatile("lidt (%0); int $0x03" :: "r"(&null_idt));
    for (;;) asm volatile("hlt");
}

inline void Commands::cmd_panic(const char *args)
{
    (void)args;
    Print("Triggering test panic via INT3 (breakpoint)...");
    asm volatile("int $0x03");
}

inline void Commands::cmd_cpuid(const char *args)
{
    (void)args;

    // Vendor string (CPUID leaf 0): EBX, EDX, ECX (in that order, 12 chars).
    uint32_t a, b, c, d;
    asm volatile("cpuid" : "=a"(a), "=b"(b), "=c"(c), "=d"(d) : "a"(0));
    char vendor[13];
    *(uint32_t *)(vendor + 0) = b;
    *(uint32_t *)(vendor + 4) = d;
    *(uint32_t *)(vendor + 8) = c;
    vendor[12] = '\0';

    char buf[80]; int j = 0;
    j = AppendStr(buf, j, 80, "vendor: ");
    j = AppendStr(buf, j, 80, vendor);
    Print(buf);

    // Feature flags (leaf 1).
    uint32_t a1, b1, c1, d1;
    asm volatile("cpuid" : "=a"(a1), "=b"(b1), "=c"(c1), "=d"(d1) : "a"(1));
    j = 0;
    j = AppendStr(buf, j, 80, "family/model = ");
    j += IntToStr((a1 >> 8) & 0xF, buf + j, 80 - j);
    j = AppendStr(buf, j, 80, "/");
    j += IntToStr((a1 >> 4) & 0xF, buf + j, 80 - j);
    j = AppendStr(buf, j, 80, "  stepping = ");
    j += IntToStr(a1 & 0xF, buf + j, 80 - j);
    Print(buf);

    j = 0;
    j = AppendStr(buf, j, 80, "features (EDX) = ");
    j += HexToStr(d1, buf + j, 80 - j);
    Print(buf);

    j = 0;
    j = AppendStr(buf, j, 80, "  fpu=");
    j += IntToStr((d1 >> 0) & 1, buf + j, 80 - j);
    j = AppendStr(buf, j, 80, " pse=");
    j += IntToStr((d1 >> 3) & 1, buf + j, 80 - j);
    j = AppendStr(buf, j, 80, " tsc=");
    j += IntToStr((d1 >> 4) & 1, buf + j, 80 - j);
    j = AppendStr(buf, j, 80, " msr=");
    j += IntToStr((d1 >> 5) & 1, buf + j, 80 - j);
    j = AppendStr(buf, j, 80, " pae=");
    j += IntToStr((d1 >> 6) & 1, buf + j, 80 - j);
    j = AppendStr(buf, j, 80, " apic=");
    j += IntToStr((d1 >> 9) & 1, buf + j, 80 - j);
    j = AppendStr(buf, j, 80, " sse=");
    j += IntToStr((d1 >> 25) & 1, buf + j, 80 - j);
    Print(buf);
}

inline void Commands::cmd_date(const char *args)
{
    (void)args;
    // We don't have an RTC driver yet, so this is a fake "uptime since boot"
    // counter rather than a wall-clock time.
    cmd_uptime(args);
}

} // namespace Applications

#endif
