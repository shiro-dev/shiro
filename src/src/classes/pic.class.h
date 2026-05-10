#ifndef PIC_CLASS_H
#define PIC_CLASS_H

#include <stdint.h>

#define PIC1_CMD  0x20
#define PIC1_DATA 0x21
#define PIC2_CMD  0xA0
#define PIC2_DATA 0xA1

#define PIC_EOI   0x20

#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01

namespace System
{
/**
 * System::PIC()
 *
 * Programs the legacy 8259A PIC pair.
 * After Remap, IRQ0..IRQ7 fire as INT 0x20..0x27 (master) and
 * IRQ8..IRQ15 as INT 0x28..0x2F (slave). This lifts the collision
 * with CPU exceptions 0x08..0x0F that the default real-mode PIC
 * configuration suffers from.
 */
class PIC
{
public:
    /**
     * System::PIC::Remap(master_offset, slave_offset)
     *
     * Reinitializes both PICs and points them at the given vector offsets.
     * Standard call: Remap(0x20, 0x28).
     */
    static void Remap(uint8_t master_offset, uint8_t slave_offset)
    {
        // Save existing masks so callers can restore behavior if they want.
        uint8_t mask_master = inb(PIC1_DATA);
        uint8_t mask_slave  = inb(PIC2_DATA);

        // ICW1: start init sequence in cascade mode, expecting ICW4.
        outb(PIC1_CMD, ICW1_INIT | ICW1_ICW4);
        outb(PIC2_CMD, ICW1_INIT | ICW1_ICW4);

        // ICW2: vector offsets.
        outb(PIC1_DATA, master_offset);
        outb(PIC2_DATA, slave_offset);

        // ICW3: tell master that there's a slave on IRQ2 (bit 2),
        //       and tell slave its cascade identity is 2.
        outb(PIC1_DATA, 0x04);
        outb(PIC2_DATA, 0x02);

        // ICW4: 8086 mode.
        outb(PIC1_DATA, ICW4_8086);
        outb(PIC2_DATA, ICW4_8086);

        // Restore the previous masks.
        outb(PIC1_DATA, mask_master);
        outb(PIC2_DATA, mask_slave);
    }

    /**
     * System::PIC::Start()
     *
     * Boot-time entrypoint: remaps PICs to 0x20/0x28 and masks every IRQ.
     * Drivers (timer, keyboard, ...) call Unmask(irq) after they're ready.
     */
    static void Start()
    {
        log("> Remapping PIC: master=0x20, slave=0x28");
        Remap(0x20, 0x28);

        // Mask everything; drivers unmask their own line on Start().
        outb(PIC1_DATA, 0xFF);
        outb(PIC2_DATA, 0xFF);
    }

    /**
     * System::PIC::Unmask(irq)
     *
     * Clears the mask bit for one IRQ line so the PIC starts delivering it.
     */
    static void Unmask(uint8_t irq)
    {
        uint16_t port;
        uint8_t bit;
        if (irq < 8)
        {
            port = PIC1_DATA;
            bit = irq;
        }
        else
        {
            port = PIC2_DATA;
            bit = irq - 8;
        }
        uint8_t mask = inb(port) & ~(1 << bit);
        outb(port, mask);
    }

    /**
     * System::PIC::Mask(irq)
     */
    static void Mask(uint8_t irq)
    {
        uint16_t port;
        uint8_t bit;
        if (irq < 8)
        {
            port = PIC1_DATA;
            bit = irq;
        }
        else
        {
            port = PIC2_DATA;
            bit = irq - 8;
        }
        uint8_t mask = inb(port) | (1 << bit);
        outb(port, mask);
    }

    /**
     * System::PIC::EndOfInterrupt(irq)
     *
     * Tells the PIC(s) we're done handling an IRQ. If the IRQ came from
     * the slave (>= 8) we have to acknowledge both chips.
     */
    static void EndOfInterrupt(uint8_t irq)
    {
        if (irq >= 8)
            outb(PIC2_CMD, PIC_EOI);
        outb(PIC1_CMD, PIC_EOI);
    }
};
} // namespace System

#endif
