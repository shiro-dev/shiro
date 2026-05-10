#ifndef PANIC_CLASS_H
#define PANIC_CLASS_H

#include <stdint.h>
#include <registers.h>

namespace System
{
/**
 * System::Panic()
 *
 * Catastrophic-error handler. ISR::Handler routes every CPU exception
 * (vector 0..31) here. Paints the screen red, dumps register state to
 * both VGA text mode and the serial port, and halts forever.
 */
class Panic
{
public:
    static const char *ExceptionName(uint32_t int_no)
    {
        switch (int_no)
        {
            case 0:  return "Division by Zero";
            case 1:  return "Debug";
            case 2:  return "Non-Maskable Interrupt";
            case 3:  return "Breakpoint";
            case 4:  return "Overflow";
            case 5:  return "Bound Range Exceeded";
            case 6:  return "Invalid Opcode";
            case 7:  return "Device Not Available";
            case 8:  return "Double Fault";
            case 9:  return "Coprocessor Segment Overrun";
            case 10: return "Invalid TSS";
            case 11: return "Segment Not Present";
            case 12: return "Stack-Segment Fault";
            case 13: return "General Protection Fault";
            case 14: return "Page Fault";
            case 15: return "Reserved";
            case 16: return "x87 Floating-Point Exception";
            case 17: return "Alignment Check";
            case 18: return "Machine Check";
            case 19: return "SIMD Floating-Point Exception";
            case 20: return "Virtualization Exception";
            case 21: return "Control Protection Exception";
            default: return "Reserved Exception";
        }
    }

    /**
     * Writes a string to (screen + offset) using VGA color attribute `attr`.
     * Returns the number of cells written, so the caller can chain.
     */
    static size_t WriteAt(uint16_t *screen, size_t offset, const char *s, uint8_t attr)
    {
        size_t i = 0;
        while (s[i] != '\0')
        {
            screen[offset + i] = (uint16_t)s[i] | ((uint16_t)attr << 8);
            i++;
        }
        return i;
    }

    /**
     * Render a uint32 as 8 hex digits at (col, row) on the panic screen.
     */
    static void HexAt(uint16_t *screen, size_t row, size_t col, uint32_t v, uint8_t attr)
    {
        const char *hex = "0123456789ABCDEF";
        screen[row * 80 + col + 0] = (uint16_t)'0' | ((uint16_t)attr << 8);
        screen[row * 80 + col + 1] = (uint16_t)'x' | ((uint16_t)attr << 8);
        for (int i = 0; i < 8; i++)
        {
            uint8_t nib = (v >> (28 - i * 4)) & 0xF;
            screen[row * 80 + col + 2 + i] =
                (uint16_t)hex[nib] | ((uint16_t)attr << 8);
        }
    }

    /**
     * System::Panic::Trigger(regs)
     *
     * Never returns. Called by ISR::Handler for vectors 0..31.
     */
    static void Trigger(registers_t regs)
    {
        // Disable interrupts immediately — a fault while handling a fault
        // becomes a triple fault.
        asm volatile("cli");

        const char *name = ExceptionName(regs.int_no);

        // CR2 holds the faulting linear address for page faults (vec 14).
        uint32_t cr2 = 0;
        asm volatile("mov %%cr2, %0" : "=r"(cr2));

        // Mirror to serial so the QEMU log captures everything even if
        // the display is unreliable.
        log("=================== KERNEL PANIC ===================");
        log("Exception ", (int)regs.int_no, ": ", name);
        log("err_code=", (int)regs.err_code, " cr2=", (int)cr2);
        log("eip=", (int)regs.eip, " cs=", (int)regs.cs, " eflags=", (int)regs.eflags);
        log("eax=", (int)regs.eax, " ebx=", (int)regs.ebx,
            " ecx=", (int)regs.ecx, " edx=", (int)regs.edx);
        log("esi=", (int)regs.esi, " edi=", (int)regs.edi,
            " ebp=", (int)regs.ebp, " esp=", (int)regs.esp);
        log("====================================================");

        // Repaint the entire VGA text-mode screen with white-on-red,
        // overwriting whatever the shell had drawn.
        uint16_t *screen = (uint16_t *)0xB8000;
        const uint8_t panic_attr = 0x4F; // white fg on red bg
        for (size_t i = 0; i < 80 * 25; i++)
        {
            screen[i] = (uint16_t)' ' | ((uint16_t)panic_attr << 8);
        }

        WriteAt(screen, 80 * 0 + 28, "*** KERNEL PANIC ***", panic_attr);

        // Exception number and name.
        size_t off = 80 * 2;
        off += WriteAt(screen, off, "  Exception #", panic_attr);
        off += WriteAt(screen, off, int2char((int)regs.int_no), panic_attr);
        off += WriteAt(screen, off, ": ", panic_attr);
        WriteAt(screen, off, name, panic_attr);

        // Two columns of register values, hex.
        WriteAt(screen, 80 * 4 + 2,  "EAX=", panic_attr);
        HexAt(screen, 4, 6, regs.eax, panic_attr);
        WriteAt(screen, 80 * 4 + 22, "EBX=", panic_attr);
        HexAt(screen, 4, 26, regs.ebx, panic_attr);
        WriteAt(screen, 80 * 4 + 42, "ECX=", panic_attr);
        HexAt(screen, 4, 46, regs.ecx, panic_attr);
        WriteAt(screen, 80 * 4 + 62, "EDX=", panic_attr);
        HexAt(screen, 4, 66, regs.edx, panic_attr);

        WriteAt(screen, 80 * 5 + 2,  "ESI=", panic_attr);
        HexAt(screen, 5, 6, regs.esi, panic_attr);
        WriteAt(screen, 80 * 5 + 22, "EDI=", panic_attr);
        HexAt(screen, 5, 26, regs.edi, panic_attr);
        WriteAt(screen, 80 * 5 + 42, "EBP=", panic_attr);
        HexAt(screen, 5, 46, regs.ebp, panic_attr);
        WriteAt(screen, 80 * 5 + 62, "ESP=", panic_attr);
        HexAt(screen, 5, 66, regs.esp, panic_attr);

        WriteAt(screen, 80 * 7 + 2,  "EIP=",      panic_attr);
        HexAt(screen, 7, 6, regs.eip, panic_attr);
        WriteAt(screen, 80 * 7 + 22, "EFLAGS=",   panic_attr);
        HexAt(screen, 7, 29, regs.eflags, panic_attr);
        WriteAt(screen, 80 * 7 + 45, "ERR=",      panic_attr);
        HexAt(screen, 7, 49, regs.err_code, panic_attr);

        // CR2 — faulting address for page faults. Only meaningful when
        // int_no == 14, but we always show it; it'll be 0 (or stale)
        // for other exceptions.
        WriteAt(screen, 80 * 9 + 2, "CR2 (faulting linear addr)= ", panic_attr);
        HexAt(screen, 9, 30, cr2, panic_attr);
        if (regs.int_no == 14)
        {
            // Decode page-fault error code bits. See Intel SDM 6.15.
            //   bit 0: P  (1 = protection violation, 0 = not present)
            //   bit 1: WR (1 = write, 0 = read)
            //   bit 2: US (1 = user mode, 0 = kernel mode)
            //   bit 3: RSVD (1 = reserved-bit set in PDE/PTE)
            //   bit 4: ID (1 = instruction fetch)
            uint32_t e = regs.err_code;
            const char *p_msg  = (e & 1) ? "protection-fault" : "page-not-present";
            const char *rw_msg = (e & 2) ? "write" : "read";
            const char *us_msg = (e & 4) ? "user-mode" : "kernel-mode";
            WriteAt(screen, 80 * 10 + 2, "Cause: ", panic_attr);
            size_t o = 80 * 10 + 9;
            o += WriteAt(screen, o, p_msg, panic_attr);
            o += WriteAt(screen, o, " on ", panic_attr);
            o += WriteAt(screen, o, rw_msg, panic_attr);
            o += WriteAt(screen, o, " in ", panic_attr);
            WriteAt(screen, o, us_msg, panic_attr);
        }

        // Stack snapshot — top 8 dwords from regs.esp upward. Useful for
        // tracing back to the call site that faulted.
        WriteAt(screen, 80 * 12 + 2, "Top of stack:", panic_attr);
        uint32_t *sp = (uint32_t *)regs.esp;
        for (int i = 0; i < 8; i++)
        {
            // Print as "[esp+NN] = 0xNNNNNNNN"
            WriteAt(screen, 80 * (13 + i) + 4, "[esp+", panic_attr);
            HexAt(screen, 13 + i, 9, (uint32_t)(i * 4), panic_attr);
            WriteAt(screen, 80 * (13 + i) + 19, "] = ", panic_attr);
            // Read carefully — stack might be near a guard page. For our
            // identity-mapped 4 GiB, any address is readable, so direct
            // dereference is safe.
            HexAt(screen, 13 + i, 23, sp[i], panic_attr);
        }

        WriteAt(screen, 80 * 23 + 2, "System halted. Reboot to recover.", panic_attr);

        // Hang forever. `hlt` lets the CPU idle; cli prevents waking on IRQ.
        for (;;)
        {
            asm volatile("cli; hlt");
        }
    }
};
} // namespace System

#endif
