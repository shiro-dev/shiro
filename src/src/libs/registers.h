#ifndef REGISTERS_H
#define REGISTERS_H

#include <stdint.h>

typedef struct registers
{
    uint32_t ds;                                     // Data segment selector pushed manually by the ISR stub
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha
    uint32_t int_no, err_code;                       // Vector number (and CPU error code, when applicable)
    uint32_t eip, cs, eflags, useresp, ss;           // Pushed by the CPU on the interrupt
} registers_t;

#endif
