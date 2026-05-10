#ifndef SCHEDULER_CLASS_H
#define SCHEDULER_CLASS_H

#include <stdint.h>
#include <stddef.h>
#include "heap.class.h"
#include "timer.class.h"

extern "C" void switch_task(uint32_t *old_esp_save, uint32_t new_esp);

namespace System
{
/**
 * System::Scheduler
 *
 * Cooperative round-robin scheduler for kernel-mode tasks.
 *
 * Each task gets its own kernel stack (4 KiB) and a saved ESP. When a task
 * calls Yield(), the asm helper switch_task() pushes its callee-saved
 * registers onto its own stack, stores the resulting ESP, then loads the
 * next task's ESP and pops a matching frame.
 *
 * On Spawn(), the scheduler pre-formats the new task's stack so the very
 * first switch_task() entry pops zeros for the four callee-saved regs and
 * `ret`s straight into the task's entry function — same shape as if the
 * task had been mid-yield all along.
 *
 * Constraints (intentional, hobby-OS scope):
 *   - cooperative: tasks must call Yield() / Sleep() periodically.
 *   - fixed task table (MAX_TASKS).
 *   - no priorities, no blocking, no IPC.
 *   - shared address space (we run a single process; "tasks" are kernel
 *     fibers).
 */
class Scheduler
{
public:
    static const uint32_t MAX_TASKS  = 8;
    static const uint32_t STACK_SIZE = 4096;

    enum task_state
    {
        TASK_UNUSED   = 0,
        TASK_READY    = 1,
        TASK_SLEEPING = 2,
        TASK_DONE     = 3,
    };

    struct task_t
    {
        uint32_t   esp;            // saved kernel ESP for this task
        uint8_t   *stack_base;     // bottom of allocated stack
        uint8_t   *stack_top;      // top of allocated stack (esp starts near here)
        uint32_t   sleep_until;    // tick count to wake on (TASK_SLEEPING only)
        uint32_t   state;
        uint32_t   id;
        char       name[16];
        uint32_t   yields;         // diagnostic counter
    };

    static task_t   tasks[MAX_TASKS];
    static int      current;       // index into tasks[] of the running task
    static uint32_t spawned_count; // total tasks ever created (for naming)
    static bool     ready;

    /**
     * System::Scheduler::Start()
     *
     * Initialises the task table. The currently-executing flow becomes
     * task 0 ("kernel"); its stack is the existing kernel stack, and we
     * don't pre-load an ESP for it — switch_task() will save into it on
     * the first yield.
     */
    static void Start()
    {
        for (uint32_t i = 0; i < MAX_TASKS; i++)
        {
            tasks[i].state = TASK_UNUSED;
            tasks[i].esp   = 0;
            tasks[i].stack_base = 0;
            tasks[i].stack_top  = 0;
            tasks[i].sleep_until = 0;
            tasks[i].id    = (uint32_t)-1;
            tasks[i].yields = 0;
            for (int k = 0; k < 16; k++) tasks[i].name[k] = 0;
        }

        // Task 0 is "kernel" — the flow that called Start().
        tasks[0].state = TASK_READY;
        tasks[0].id    = 0;
        const char *kn = "kernel";
        for (int k = 0; k < 7; k++) tasks[0].name[k] = kn[k];

        current = 0;
        spawned_count = 1;
        ready = true;

        // Hand the timer a pointer to Yield so it can preempt running
        // tasks every Timer::PREEMPT_EVERY ticks. Cooperative-only mode
        // would skip this line.
        Drivers::Timer::RegisterPreemptCallback(Scheduler::PreemptiveYield);

        log("> Scheduler online (preemptive, ", (int)MAX_TASKS, " task slots)");
    }

    /**
     * System::Scheduler::PreemptiveYield()
     *
     * Wrapper called from the timer ISR. Identical to Yield() but with
     * a guard that prevents reentrance (we don't want one Yield to
     * trigger another mid-switch).
     */
    static void PreemptiveYield()
    {
        static volatile bool in_preempt = false;
        if (!ready) return;
        if (in_preempt) return; // already mid-switch, skip this tick
        in_preempt = true;
        Yield();
        in_preempt = false;
    }

    /**
     * System::Scheduler::Spawn(fn, name)
     *
     * Creates a new task that begins execution at fn() the first time it
     * is scheduled. fn must never return — instead it should loop and
     * call Yield()/Sleep() to give other tasks the CPU.
     *
     * Returns the new task id, or -1 on OOM / no slots.
     */
    static int Spawn(void (*entry)(), const char *name)
    {
        if (!ready) return -1;

        // Find a free slot.
        int slot = -1;
        for (uint32_t i = 0; i < MAX_TASKS; i++)
        {
            if (tasks[i].state == TASK_UNUSED) { slot = (int)i; break; }
        }
        if (slot < 0) return -1;

        uint8_t *stack = (uint8_t *)Heap::Alloc(STACK_SIZE);
        if (stack == 0) return -1;

        // Pre-format the stack so the first switch_task() into it pops
        // zeros for the 4 callee-saved regs and rets to entry().
        //
        //  high addr +-----------------+
        //            |  entry          |  <- ret addr (popped by `ret`)
        //            |  0  (saved ebp) |
        //            |  0  (saved ebx) |
        //            |  0  (saved esi) |
        //            |  0  (saved edi) |  <- task->esp on first switch
        //  low  addr +-----------------+
        uint32_t *sp = (uint32_t *)(stack + STACK_SIZE);
        *(--sp) = (uint32_t)entry;  // ret addr
        *(--sp) = 0;                // ebp
        *(--sp) = 0;                // ebx
        *(--sp) = 0;                // esi
        *(--sp) = 0;                // edi

        tasks[slot].esp         = (uint32_t)sp;
        tasks[slot].stack_base  = stack;
        tasks[slot].stack_top   = stack + STACK_SIZE;
        tasks[slot].sleep_until = 0;
        tasks[slot].state       = TASK_READY;
        tasks[slot].id          = spawned_count++;
        tasks[slot].yields      = 0;

        // Copy name (truncated to 15 chars + NUL).
        int i = 0;
        while (name[i] && i < 15) { tasks[slot].name[i] = name[i]; i++; }
        tasks[slot].name[i] = '\0';

        log("> Scheduler: spawned task '", tasks[slot].name, "' id=", (int)tasks[slot].id);
        return slot;
    }

    /**
     * Find the next runnable task starting from current+1. Wakes any
     * sleeping tasks whose deadline has passed. Returns the slot index.
     */
    static int PickNext()
    {
        uint32_t now = Drivers::Timer::GetTicks();
        for (uint32_t step = 1; step <= MAX_TASKS; step++)
        {
            int idx = (current + (int)step) % (int)MAX_TASKS;
            if (tasks[idx].state == TASK_SLEEPING && now >= tasks[idx].sleep_until)
                tasks[idx].state = TASK_READY;
            if (tasks[idx].state == TASK_READY) return idx;
        }
        return current; // fall back to ourselves (only us is runnable)
    }

    /**
     * System::Scheduler::Yield()
     *
     * Give up the CPU to the next runnable task. Returns when this task
     * is scheduled again.
     */
    static void Yield()
    {
        if (!ready) return;
        int next = PickNext();
        if (next == current) return;

        int prev = current;
        current = next;
        tasks[prev].yields++;
        switch_task(&tasks[prev].esp, tasks[next].esp);
    }

    /**
     * System::Scheduler::Sleep(ms)
     *
     * Mark the running task sleeping for ~ms milliseconds and yield.
     */
    static void Sleep(uint32_t ms)
    {
        if (!ready) return;
        uint32_t ticks = (ms * Drivers::Timer::TICK_HZ) / 1000;
        if (ticks == 0) ticks = 1;
        tasks[current].sleep_until = Drivers::Timer::GetTicks() + ticks;
        tasks[current].state = TASK_SLEEPING;
        Yield();
    }

    static int CurrentId() { return ready ? (int)tasks[current].id : -1; }
    static const char *CurrentName() { return ready ? tasks[current].name : "?"; }
    static int Count()
    {
        int n = 0;
        for (uint32_t i = 0; i < MAX_TASKS; i++)
            if (tasks[i].state != TASK_UNUSED) n++;
        return n;
    }
};

Scheduler::task_t Scheduler::tasks[Scheduler::MAX_TASKS];
int               Scheduler::current = 0;
uint32_t          Scheduler::spawned_count = 0;
bool              Scheduler::ready = false;

} // namespace System

#endif
