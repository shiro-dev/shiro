#ifndef NEW_H
#define NEW_H

#include <stddef.h>
#include "heap.class.h"

/**
 * Real `operator new` / `operator delete` backed by System::Heap.
 *
 * The previous implementation was a one-line bump that handed out the
 * post-increment address (so every `new T()` lived in unowned memory) and
 * did pointer arithmetic on a uint16_t* (advancing 2*size bytes per call).
 * It "worked" only because the kernel's allocations were few and small.
 *
 * Now `new` is a thin wrapper around Heap::Alloc, and `delete` actually
 * returns memory to the free list.
 */

inline void *operator new(size_t size)
{
    return System::Heap::Alloc((uint32_t)size);
}

inline void *operator new[](size_t size)
{
    return System::Heap::Alloc((uint32_t)size);
}

inline void operator delete(void *p) noexcept
{
    System::Heap::Free(p);
}

inline void operator delete[](void *p) noexcept
{
    System::Heap::Free(p);
}

// Sized-deallocation forms required by C++14+ even if we ignore the size.
inline void operator delete(void *p, size_t) noexcept
{
    System::Heap::Free(p);
}

inline void operator delete[](void *p, size_t) noexcept
{
    System::Heap::Free(p);
}

#endif
