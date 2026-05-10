#ifndef SHIRO_CLASS_H
#define SHIRO_CLASS_H

#include <string.h>
#include <common.h>

// New subsystems first — order matters because isr.class.h calls into
// Panic, Timer, Keyboard, and PIC, so they all have to be visible before
// the ISR dispatcher is defined.
#include "multiboot.class.h"
#include "panic.class.h"
#include "pic.class.h"
#include "keyboard.class.h"
#include "timer.class.h"
#include "isr.class.h"
#include "heap.class.h"
#include "pmm.class.h"
#include "paging.class.h"
#include "scheduler.class.h"
#include "initrd.class.h"

#include "gdt.class.h"
#include "idt.class.h"
#include "bios.class.h"
#include "vga.class.h"
#include "text.mode.class.h"
#include "video.mode.class.h"
#include "cursor.class.h"

/**
 * namespace System
 *
 * This is our core namespace.
 * All system-related classes can be found on this namespace.
 */
namespace System
{
/**
 * System::Shiro()
 *
 * This class is the core of shiro.
 * The most important methods to run the OS can be found/added here.
 */
class Shiro
{
private:
    /**
     * This parameter will hold our instance.
     */
    static System::Shiro *instance;

    /**
     * Adding a private contructor.
     */
    Shiro(){};

public:
    /**
     * Multiboot info parser. Populated by shiro_main before Start() runs.
     */
    System::Multiboot multiboot;

    /**
     * VGA color helpers (palette + bit-packing for the text-mode buffer).
     */
    System::VGA vga;

    /**
     * Text-mode controller — owns the 0xB8000 screen address.
     */
    System::Modes::Text textMode;

    /**
     * Video-mode placeholder (currently scaffolding only).
     */
    System::Modes::Video videoMode;

    /**
     * BIOS bridge for real-mode interrupt calls.
     */
    System::Bios bios;

    /**
     * Keyboard driver (PS/2 set-1).
     */
    System::Drivers::Keyboard keyboard;

    /**
     * GDT manager.
     */
    System::GDT gdt;

    /**
     * IDT manager.
     */
    System::IDT idt;

    /**
     * Text cursor controller.
     */
    System::Cursor cursor;

    /**
     * This parameter will set/return our current instance
     */
    static System::Shiro *GetInstance()
    {
        if (instance == 0)
        {
            instance = new System::Shiro();
        }

        return instance;
    }

    /**
     * System::Shiro.Start()
     *
     * Pseudo-constructor. Brings every kernel subsystem online in order:
     *   1. GDT       — descriptor tables for kernel & (scaffolded) user-mode
     *   2. PIC       — remap legacy IRQs from 0x08-0x0F (collision) to 0x20-0x2F
     *   3. IDT       — interrupt vectors 0..47 (CPU exceptions + hardware IRQs)
     *   4. Timer     — PIT at 100 Hz (gives the kernel a notion of time)
     *   5. Keyboard  — unmasks IRQ1
     *   6. Text mode — exposes 0xB8000 to the rest of the kernel
     *   7. VGA       — color palette helpers
     *   8. STI       — finally, open the IF flag so interrupts start firing
     */
    void Start()
    {
        // Intro message
        log("> Welcome to Shiro - Operating System");

        // 1. Global Descriptor Table.
        this->gdt.Start();

        // 2. Remap PICs to 0x20 (master) / 0x28 (slave). Must happen before
        //    IDT::Start so that the gates we install for IRQs already point
        //    at non-colliding vectors, and before any device unmasks itself.
        System::PIC::Start();

        // 3. Interrupt Descriptor Table — installs handlers for vectors
        //    0..31 (exceptions) and 32..47 (hardware IRQs).
        this->idt.Start();

        // 3.5. Paging — identity-map 4 GiB with 4 MiB pages and turn on
        //      the MMU. Must come after IDT so a misstep faults into our
        //      Panic handler (vector 14) instead of a silent triple fault.
        System::Paging::Start();

        // 4. Programmable Interval Timer — IRQ0 at 100 Hz.
        System::Drivers::Timer::Start();

        // 5. Keyboard driver — unmasks IRQ1.
        this->keyboard.Start();

        // 6. Text Mode (memory-mapped 0xB8000).
        this->textMode.Start();

        // 7. VGA color helpers.
        this->vga.Start();

        // 7.5. Cooperative scheduler. Brings up task table and registers
        //      the currently-running flow as task 0 ("kernel"). No tasks
        //      are spawned here; kernel.cpp does that after Shell::Start.
        System::Scheduler::Start();

        // 8. Open IF — only NOW are interrupts allowed to fire. Doing this
        //    last avoids races where a half-initialized driver gets called
        //    by an IRQ that arrives mid-Start.
        asm volatile("sti");

        log("> Interrupts enabled (IF=1)");
    }

    /**
     * System::Shiro.Finish()
     *
     * This is our pseudo destructor, a finishing point.
     *
     * @return void
     */
    void Finish()
    {
        // Finish Text Mode
        this->textMode.Finish();

        // We are good to go
        log("> SHIRO IS LOADED AND READY TO GO");
    }
};
} // namespace System

// Let's mark our current instance as non-existing
System::Shiro *System::Shiro::instance = 0;

#endif
