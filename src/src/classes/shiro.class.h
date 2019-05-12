#ifndef SHIRO_CLASS_H
#define SHIRO_CLASS_H

#include <string.h>
#include <common.h>
#include "keyboard.class.h"
#include "gdt.class.h"
#include "isr.class.h"
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
     * This parameter is responsible for all vga related items (colors, mostly).
     */
    System::VGA vga;

    /**
     * This parameter is responsible for all text mode interactions.
     */
    System::Modes::Text textMode;

    /**
     * This parameter is responsible for all video mode interactions.
     */
    System::Modes::Video videoMode;

    /**
     * This parameter is responsible for all interactions with the BIOS.
     */
    System::Bios bios;

    /**
     * This parameter is responsible for all interactions with the keyboard.
     */
    System::Drivers::Keyboard keyboard;

    /**
     * This parameter is responsible for all interactions with the GDT.
     */
    System::GDT gdt;

    /**
     * This parameter is responsible for all interactions with the IDT.
     */
    System::IDT idt;

    /**
     * This parameter is responsible for all interactions with the text cursor.
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
     * This is our pseudo constructor, a starting point.
     * Here we can implement the actual OS code.
     * 
     * @return void
     */
    void Start()
    {
        // Intro message
        log("> Welcome to Shiro - Operating System");

        // Load our Global Descriptor Table
        this->gdt.Start();

        // Load our Interrupt Descriptor Table
        this->idt.Start();

        // Start Text Mode
        this->textMode.Start();

        // Initializing VGA
        this->vga.Start();
    }

    /**
     * System::Shiro.Finish()
     *
     * This is our pseudo destructor, a finishing point.
     * Here we can implement our final routines before ending our OS execution.
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