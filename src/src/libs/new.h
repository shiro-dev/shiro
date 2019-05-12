#include <stddef.h>

/**
 * The bottom of our stack is set to 16384 (boot.asm)
 * Based on that, I'm pointing our current memory to bottom+1024.
 * This is not even close to being a memory manager, but it allows us to use the "new" operator.
 * Every time the operator is called, the memory base increases.
 * This is a very (very, very, ..., very) basic version of a WatterMark Allocator.
 */
uint16_t *memory = (uint16_t *)(16384 + 1024);

/**
 * This is a very basic implementation of the "new" operator.
 * Usage (example):
 *  char* c = new char(10);
 */
void *operator new(size_t size)
{
    // Set new memory base
    memory += size;

    // Return
    return memory;
}