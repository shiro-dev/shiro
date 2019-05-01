#include <helper/debug.h>
#include <helper/php.h>
#include "classes/shiro.class.h"

/**
 * shiro_main()
 *
 * This function is called from boot.asm when the GRUB Bootloader has finished loading everything.
 * System::Shiro::Start() will act like a constructor, loading everything we need.
 * System::Shiro::Finish() will act like a destructor, running the finals routines before we finish the code.
 *
 * @return void
 */
extern "C" void shiro_main() 
{
    // Instantiate the System::Shiro class
    System::Shiro shiro;

    // Load Shiro starting method (construtor)
    shiro.Start();

    // Anything we want
    // ...

    // Load Shiro finishing method (destructor)
    shiro.Finish();
}