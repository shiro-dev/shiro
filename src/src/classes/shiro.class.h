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
#include "shell.class.h"

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
     * This parameter is responsible for all shell interactions.
     */
    System::Shell shell;

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
     * System::Shiro.NotImplemented()
     * 
     * This method was created to avoid the "unused parameter" warning when compiling the code.
     */
    void NotImplemented()
    {
        // ...
    }

    /**
     * System::Shiro.Start(auto &shiro)
     *
     * This is our pseudo constructor, a starting point.
     * Here we can implement the actual OS code.
     * 
     * @return void
     */
    void Start(auto &shiro)
    {
        // Intro message
        log("> Welcome to Shiro - Operating System");

        // Load our Global Descriptor Table
        this->gdt.Start(shiro);

        // Load our Interrupt Descriptor Table
        this->idt.Start(shiro);

        // Start Text Mode
        this->textMode.Start(shiro);
    }

    /**
     * System::Shiro.Finish(auto &shiro)
     *
     * This is our pseudo destructor, a finishing point.
     * Here we can implement our final routines before ending our OS execution.
     * 
     * @return void
     */
    void Finish(auto &shiro)
    {
        // Finish Text Mode
        this->textMode.Finish(shiro);

        // Start Shell
        this->shell.Start(shiro);

        // We are good to go
        log("> SHIRO IS LOADED AND READY TO GO");
        shiro.shell.echo("> SHIRO IS LOADED AND READY TO GO\n");

        // Start our loop sequence
        this->Loop(shiro);
    }

    /**
     * System::Shiro.Loop(auto &shiro)
     *
     * Here we can place routines that are going to run on loop once the whole OS is loaded.
     * 
     * @return void
     */
    void Loop(auto &shiro)
    {
        
    }
};
} // namespace System

#endif