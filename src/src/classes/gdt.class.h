#ifndef GDT_CLASS_H
#define GDT_CLASS_H

struct gdt_entry_struct
{
    uint16_t limit;
    uint16_t low;
    uint8_t middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t high;
} __attribute__((packed));
typedef struct gdt_entry_struct gdt_entry_t;

struct gdt_ptr_struct
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));
typedef struct gdt_ptr_struct gdt_ptr_t;

extern "C" void GDTFlush(uint32_t);

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
class GDT
{
public:
    gdt_entry_t entries[5];
    gdt_ptr_t ptr;

    /**
     * System::GDT.Start()
     *
     * This is our pseudo constructor, a starting point.
     * 
     * @return void
     */
    void Start(auto &shiro)
    {
        // Set Limit and Base
        this->ptr.limit = (sizeof(gdt_entry_t) * 5) - 1;
        this->ptr.base = (uint32_t)&entries;

        // Set GDT Gates
        this->SetGate(0, 0, 0, 0, 0);
        this->SetGate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
        this->SetGate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
        this->SetGate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
        this->SetGate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);

        // Flush
        GDTFlush((uint32_t) & this->ptr);

        shiro.NotImplemented();
    }

    /**
     * System::GDT.SetGate()
     *
     * This method is used to set the value of a GDT Gate.
     * 
     * @return void
     */
    void SetGate(uint32_t gate, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
    {
        // Set Bases
        this->entries[gate].low = (base & 0xFFFF);
        this->entries[gate].middle = (base >> 16) & 0xFF;
        this->entries[gate].high = (base >> 24) & 0xFF;

        // Set Limit
        this->entries[gate].limit = (limit & 0xFFFF);

        // Set Granularity
        this->entries[gate].granularity = (limit >> 16) & 0x0F;
        this->entries[gate].granularity |= gran & 0xF0;

        // Set Access
        this->entries[gate].access = access;
    }
};
} // namespace System

#endif