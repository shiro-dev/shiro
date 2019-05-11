#ifndef CURSOR_CLASS_H
#define CURSOR_CLASS_H

#include "bios.class.h"

/**
 * namespace System
 *
 * This is our core namespace.
 * All system-related classes can be found on this namespace.
 */
namespace System
{
class Cursor
{
public:
    /**
     * System::Cursor.enable()
     *
     * This method is responsible for enabling the cursor on screen.
     * 
     * @return void
     */
    void enable()
    {
        // Log
        log("> Enabling Cursor");

        // Enable Cursor
        outb(0x3D4, 0x0A);
        outb(0x3D5, (inb(0x3D5) & 0xC0) | 0);
        outb(0x3D4, 0x0B);
        outb(0x3D5, (inb(0x3d5) & 0xE0) | 15);
    }

    /**
     * System::Cursor.moveTo(int x, int y)
     *
     * This method is responsible for moving the cursor to a certain coordinate.
     * 
     * @return void
     */
    void moveTo(int x, int y)
    {
        // Calculate cursor position
        uint16_t pos = (y * 80) + x;

        // Move cursor
        outb(0x3D4, 0x0F);
        outb(0x3D5, (uint8_t)(pos & 0xFF));
        outb(0x3D4, 0x0E);
        outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
    }
};
} // namespace System

#endif