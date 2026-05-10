#ifndef ISR_CLASS_H
#define ISR_CLASS_H

#include <registers.h>

// CPU exceptions / reserved (vectors 0..31)
extern "C" void isr0();
extern "C" void isr1();
extern "C" void isr2();
extern "C" void isr3();
extern "C" void isr4();
extern "C" void isr5();
extern "C" void isr6();
extern "C" void isr7();
extern "C" void isr8();
extern "C" void isr9();
extern "C" void isr10();
extern "C" void isr11();
extern "C" void isr12();
extern "C" void isr13();
extern "C" void isr14();
extern "C" void isr15();
extern "C" void isr16();
extern "C" void isr17();
extern "C" void isr18();
extern "C" void isr19();
extern "C" void isr20();
extern "C" void isr21();
extern "C" void isr22();
extern "C" void isr23();
extern "C" void isr24();
extern "C" void isr25();
extern "C" void isr26();
extern "C" void isr27();
extern "C" void isr28();
extern "C" void isr29();
extern "C" void isr30();
extern "C" void isr31();

// Hardware IRQs after PIC remap to 0x20 (master) / 0x28 (slave).
extern "C" void isr32();
extern "C" void isr33();
extern "C" void isr34();
extern "C" void isr35();
extern "C" void isr36();
extern "C" void isr37();
extern "C" void isr38();
extern "C" void isr39();
extern "C" void isr40();
extern "C" void isr41();
extern "C" void isr42();
extern "C" void isr43();
extern "C" void isr44();
extern "C" void isr45();
extern "C" void isr46();
extern "C" void isr47();

namespace System
{
/**
 * System::ISR()
 *
 * Routes every interrupt and exception that the CPU dispatches through
 * the IDT.
 *
 *   vec 0..31  : CPU exceptions  -> System::Panic::Trigger (never returns)
 *   vec 32     : IRQ0 timer      -> Drivers::Timer::Handler
 *   vec 33     : IRQ1 keyboard   -> Drivers::Keyboard::Handler
 *   vec 34..47 : other IRQs      -> ack PIC and ignore (for now)
 */
class ISR
{
public:
    static void Handler(registers_t regs)
    {
        // CPU exceptions: never return — Panic halts the kernel.
        if (regs.int_no < 32)
        {
            System::Panic::Trigger(regs);
            return;
        }

        // Hardware IRQs (PIC) after remap.
        if (regs.int_no <= 47)
        {
            uint8_t irq = (uint8_t)(regs.int_no - 32);

            if (irq == 0)
            {
                System::Drivers::Timer::Handler();
            }
            else if (irq == 1)
            {
                System::Drivers::Keyboard::Handler();
            }
            // Any other IRQ is currently unhandled but still gets EOI'd
            // below so the PIC keeps delivering interrupts.

            System::PIC::EndOfInterrupt(irq);
            return;
        }

        // Anything outside [0, 47] is something we never registered.
        // Best-effort ack and continue.
        outb(0x20, 0x20);
    }
};
} // namespace System

/**
 * extern "C" so that isr_common_stub in boot/asm/isr.asm can `call` it.
 * Lives outside the namespace so the linker resolves the symbol verbatim.
 */
extern "C" void isr_handler(registers_t regs)
{
    System::ISR::Handler(regs);
}

#endif
