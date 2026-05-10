#ifndef TIMER_CLASS_H
#define TIMER_CLASS_H

#include <stdint.h>

#define PIT_CHANNEL0 0x40
#define PIT_COMMAND  0x43
#define PIT_FREQUENCY 1193182u

namespace System
{
namespace Drivers
{
/**
 * System::Drivers::Timer()
 *
 * Programs PIT channel 0 to fire IRQ0 at TICK_HZ Hz. The IRQ handler
 * increments `ticks`, giving the rest of the kernel a notion of time
 * without busy-looping.
 */
class Timer
{
public:
    static const uint32_t TICK_HZ = 100; // 10 ms per tick

    static volatile uint32_t ticks;

    // Optional callback invoked from the timer ISR every PREEMPT_EVERY ticks.
    // The Scheduler registers its Yield() here at boot to get preemptive
    // multitasking. Stored as a function pointer so timer.class.h doesn't
    // have to know what Scheduler is.
    static void (*preempt_cb)();
    static const uint32_t PREEMPT_EVERY = 5; // every ~50 ms

    static void RegisterPreemptCallback(void (*cb)())
    {
        preempt_cb = cb;
    }

    /**
     * System::Drivers::Timer::Start()
     *
     * Programs the PIT for square-wave at TICK_HZ and unmasks IRQ0.
     * Caller must have remapped the PIC first (System::PIC::Start).
     */
    static void Start()
    {
        log("> Starting PIT timer at ", (int)TICK_HZ, " Hz");

        uint32_t divisor = PIT_FREQUENCY / TICK_HZ;

        // Channel 0, lobyte/hibyte access, mode 3 (square wave), binary.
        outb(PIT_COMMAND, 0x36);
        outb(PIT_CHANNEL0, (uint8_t)(divisor & 0xFF));
        outb(PIT_CHANNEL0, (uint8_t)((divisor >> 8) & 0xFF));

        // Open IRQ0 on the master PIC.
        System::PIC::Unmask(0);
    }

    /**
     * System::Drivers::Timer::Handler()
     *
     * Called from ISR::Handler when vector 0x20 (IRQ0) fires.
     * Must stay short — runs with interrupts disabled.
     *
     * If a preempt callback is registered (Scheduler::Yield), invoke it
     * every PREEMPT_EVERY ticks. This gives the kernel preemptive
     * multitasking on top of the cooperative switch_task plumbing.
     */
    static void Handler()
    {
        ticks++;
        if (preempt_cb != 0 && (ticks % PREEMPT_EVERY) == 0)
        {
            preempt_cb();
        }
    }

    static uint32_t GetTicks()
    {
        return ticks;
    }

    /**
     * System::Drivers::Timer::Sleep(ms)
     *
     * Blocks roughly `ms` milliseconds. Uses `hlt` so the CPU idles
     * between ticks instead of spinning. Requires IF=1.
     */
    static void Sleep(uint32_t ms)
    {
        uint32_t needed = (ms * TICK_HZ) / 1000;
        if (needed == 0)
            needed = 1;
        uint32_t target = ticks + needed;
        while (ticks < target)
        {
            asm volatile("hlt");
        }
    }
};

volatile uint32_t Timer::ticks = 0;
void (*Timer::preempt_cb)() = 0;

} // namespace Drivers
} // namespace System

#endif
