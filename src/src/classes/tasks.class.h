#ifndef TASKS_CLASS_H
#define TASKS_CLASS_H

#include <stdint.h>
#include "scheduler.class.h"
#include "shell.class.h"
#include "heap.class.h"
#include "pmm.class.h"
#include "timer.class.h"

namespace System
{
namespace DemoTasks
{
/**
 * Format an unsigned integer as decimal into `buf`. Returns chars written.
 */
inline int u32_to_str(uint32_t v, char *buf, int cap)
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
 * Format `n` as a 2-digit zero-padded decimal.
 */
inline void u32_to_2digit(uint32_t v, char *out)
{
    out[0] = (char)('0' + ((v / 10) % 10));
    out[1] = (char)('0' + (v % 10));
}

/**
 * task_spinner()
 *
 * Animates a one-cell spinner (|, /, -, \) in the right-hand side of the
 * header bar at column 65, just left of the heart and clock. Header
 * background is dark blue now, so we paint with white-on-blue to match.
 */
inline void task_spinner()
{
    using Applications::Shell;

    const char frames[] = { '|', '/', '-', '\\' };
    Shell *shell = Shell::GetInstance();
    uint32_t i = 0;

    while (true)
    {
        // Match the header colour scheme.
        uint16_t color = (uint16_t)shell->header_color;
        uint16_t cell  = (color << 8) | (uint16_t)frames[i & 3];

        // Right-side cluster: heart at col 64, spinner at col 66.
        shell->terminal_screen_header[66] = cell;
        shell->UpdateScreen();

        i++;
        Scheduler::Sleep(150);
    }
}

/**
 * task_heartbeat()
 *
 * Blinks a heart character (CP437 0x03) in the header.
 */
inline void task_heartbeat()
{
    using Applications::Shell;

    Shell *shell = Shell::GetInstance();
    bool on = false;

    while (true)
    {
        uint16_t color = (uint16_t)shell->header_color;
        uint16_t cell;
        if (on)
            cell = (color << 8) | 0x03; // CP437 ♥
        else
            cell = (color << 8) | (uint16_t)' ';

        shell->terminal_screen_header[64] = cell;
        shell->UpdateScreen();

        on = !on;
        Scheduler::Sleep(700);
    }
}

/**
 * task_clock()
 *
 * Renders an HH:MM:SS uptime clock at the right edge of the header
 * (columns 70..78). Updates every second.
 */
inline void task_clock()
{
    using Applications::Shell;

    Shell *shell = Shell::GetInstance();

    while (true)
    {
        uint32_t total_sec = Drivers::Timer::GetTicks() / Drivers::Timer::TICK_HZ;
        uint32_t h = total_sec / 3600;
        uint32_t m = (total_sec / 60) % 60;
        uint32_t s = total_sec % 60;

        char buf[10];
        u32_to_2digit(h, &buf[0]);
        buf[2] = ':';
        u32_to_2digit(m, &buf[3]);
        buf[5] = ':';
        u32_to_2digit(s, &buf[6]);
        buf[8] = '\0';

        // Paint into the header buffer at column 70 (uses header colour).
        uint16_t color = (uint16_t)shell->header_color;
        for (int i = 0; i < 8; i++)
            shell->terminal_screen_header[70 + i] = (color << 8) | (uint16_t)buf[i];

        shell->UpdateScreen();
        Scheduler::Sleep(1000);
    }
}

/**
 * task_statusbar()
 *
 * Repaints row 1 (the status bar) every 250 ms with live system metrics:
 *
 *   "  up HH:MM:SS  |  mem 132K/4M  |  pmm 122M free  |  tasks N  |  ticks NNNNNNN"
 *
 * Uses Shell::RenderStatus which fills the row with the status colour
 * scheme then overlays the text starting at column 1.
 */
inline void task_statusbar()
{
    using Applications::Shell;

    Shell *shell = Shell::GetInstance();
    char buf[80];

    while (true)
    {
        uint32_t total_sec = Drivers::Timer::GetTicks() / Drivers::Timer::TICK_HZ;
        uint32_t h = total_sec / 3600;
        uint32_t m = (total_sec / 60) % 60;
        uint32_t s = total_sec % 60;

        int j = 0;

        // " up HH:MM:SS"
        buf[j++] = ' '; buf[j++] = 'u'; buf[j++] = 'p'; buf[j++] = ' ';
        u32_to_2digit(h, &buf[j]); j += 2;
        buf[j++] = ':';
        u32_to_2digit(m, &buf[j]); j += 2;
        buf[j++] = ':';
        u32_to_2digit(s, &buf[j]); j += 2;

        // "  |  mem <K>K/<M>M"
        buf[j++] = ' '; buf[j++] = ' '; buf[j++] = (char)0xB3; buf[j++] = ' '; buf[j++] = ' ';
        buf[j++] = 'm'; buf[j++] = 'e'; buf[j++] = 'm'; buf[j++] = ' ';
        j += u32_to_str(System::Heap::BytesUsed() / 1024, &buf[j], 80 - j);
        buf[j++] = 'K'; buf[j++] = '/';
        j += u32_to_str((System::Heap::BytesUsed() + System::Heap::BytesFree()) / (1024 * 1024), &buf[j], 80 - j);
        buf[j++] = 'M';

        // "  |  pmm <M>M free"
        buf[j++] = ' '; buf[j++] = ' '; buf[j++] = (char)0xB3; buf[j++] = ' '; buf[j++] = ' ';
        buf[j++] = 'p'; buf[j++] = 'm'; buf[j++] = 'm'; buf[j++] = ' ';
        j += u32_to_str(System::PMM::FreePages() / 256, &buf[j], 80 - j);
        buf[j++] = 'M'; buf[j++] = ' ';
        buf[j++] = 'f'; buf[j++] = 'r'; buf[j++] = 'e'; buf[j++] = 'e';

        // "  |  tasks N"
        buf[j++] = ' '; buf[j++] = ' '; buf[j++] = (char)0xB3; buf[j++] = ' '; buf[j++] = ' ';
        buf[j++] = 't'; buf[j++] = 'a'; buf[j++] = 's'; buf[j++] = 'k'; buf[j++] = 's'; buf[j++] = ' ';
        j += u32_to_str((uint32_t)Scheduler::Count(), &buf[j], 80 - j);

        // "  |  ticks NNNN"
        buf[j++] = ' '; buf[j++] = ' '; buf[j++] = (char)0xB3; buf[j++] = ' '; buf[j++] = ' ';
        buf[j++] = 't'; buf[j++] = 'i'; buf[j++] = 'c'; buf[j++] = 'k'; buf[j++] = 's'; buf[j++] = ' ';
        j += u32_to_str(Drivers::Timer::GetTicks(), &buf[j], 80 - j);

        buf[j] = '\0';

        shell->RenderStatus(buf);
        shell->UpdateScreen();

        Scheduler::Sleep(250);
    }
}

} // namespace DemoTasks
} // namespace System

#endif
