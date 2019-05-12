#ifndef IDT_CLASS_H
#define IDT_CLASS_H

struct idt_entry_struct
{
   uint16_t base_lo; // The lower 16 bits of the address to jump to when this interrupt fires.
   uint16_t sel;     // Kernel segment selector.
   uint8_t always0;  // This must always be zero.
   uint8_t flags;    // More flags. See documentation.
   uint16_t base_hi; // The upper 16 bits of the address to jump to.
} __attribute__((packed));
typedef struct idt_entry_struct idt_entry_t;

struct idt_ptr_struct
{
   uint16_t limit;
   uint32_t base; // The address of the first element in our idt_entry_t array.
} __attribute__((packed));
typedef struct idt_ptr_struct idt_ptr_t;

extern "C" void IDTFlush(uint32_t);

/**
 * namespace System
 *
 * This is our core namespace.
 * All system-related classes can be found on this namespace.
 */
namespace System
{
/**
 * System::GDT()
 *
 * This class is responsible for controlling our Global Descriptor Table.
 */
class IDT
{
public:
   idt_entry_t entries[256];
   idt_ptr_t ptr;

   /**
    * System::IDT.Start()
    *
    * This is our pseudo constructor, a starting point.
    * 
    * @return void
    */
   void Start()
   {
      ptr.limit = sizeof(idt_entry_t) * 256 - 1;
      ptr.base = (uint32_t)&entries;

      memset(&entries, 0, sizeof(idt_entry_t) * 256);

      SetGate(0, (uint32_t)isr0, 0x08, 0x8E);
      SetGate(1, (uint32_t)isr1, 0x08, 0x8E);
      SetGate(2, (uint32_t)isr2, 0x08, 0x8E);
      SetGate(3, (uint32_t)isr3, 0x08, 0x8E);
      SetGate(4, (uint32_t)isr4, 0x08, 0x8E);
      SetGate(5, (uint32_t)isr5, 0x08, 0x8E);
      SetGate(6, (uint32_t)isr6, 0x08, 0x8E);
      SetGate(7, (uint32_t)isr7, 0x08, 0x8E);
      SetGate(8, (uint32_t)isr8, 0x08, 0x8E);
      SetGate(9, (uint32_t)isr9, 0x08, 0x8E);
      SetGate(10, (uint32_t)isr10, 0x08, 0x8E);
      SetGate(11, (uint32_t)isr11, 0x08, 0x8E);
      SetGate(12, (uint32_t)isr12, 0x08, 0x8E);
      SetGate(13, (uint32_t)isr13, 0x08, 0x8E);
      SetGate(14, (uint32_t)isr14, 0x08, 0x8E);
      SetGate(15, (uint32_t)isr15, 0x08, 0x8E);
      SetGate(16, (uint32_t)isr16, 0x08, 0x8E);
      SetGate(17, (uint32_t)isr17, 0x08, 0x8E);
      SetGate(18, (uint32_t)isr18, 0x08, 0x8E);
      SetGate(19, (uint32_t)isr19, 0x08, 0x8E);
      SetGate(20, (uint32_t)isr20, 0x08, 0x8E);
      SetGate(21, (uint32_t)isr21, 0x08, 0x8E);
      SetGate(22, (uint32_t)isr22, 0x08, 0x8E);
      SetGate(23, (uint32_t)isr23, 0x08, 0x8E);
      SetGate(24, (uint32_t)isr24, 0x08, 0x8E);
      SetGate(25, (uint32_t)isr25, 0x08, 0x8E);
      SetGate(26, (uint32_t)isr26, 0x08, 0x8E);
      SetGate(27, (uint32_t)isr27, 0x08, 0x8E);
      SetGate(28, (uint32_t)isr28, 0x08, 0x8E);
      SetGate(29, (uint32_t)isr29, 0x08, 0x8E);
      SetGate(30, (uint32_t)isr30, 0x08, 0x8E);
      SetGate(31, (uint32_t)isr31, 0x08, 0x8E);

      IDTFlush((uint32_t)&ptr);
   }

   /**
    * System::IDT.SetGate()
    *
    * This method is responsible for registering our ISR's.
    * 
    * @return void
    */
   void SetGate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags)
   {
      entries[num].base_lo = base & 0xFFFF;
      entries[num].base_hi = (base >> 16) & 0xFFFF;

      entries[num].sel = sel;
      entries[num].always0 = 0;
      // We must uncomment the OR below when we get to using user-mode.
      // It sets the interrupt gate's privilege level to 3.
      entries[num].flags = flags /* | 0x60 */;
   }
};
} // namespace System

#endif