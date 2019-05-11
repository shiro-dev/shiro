#include <helper/debug.h>
#include "shiro.class.h"

// Instantiate the System::Shiro class
System::Shiro shiro;

/**
 * loop()
 *
 * Here we can place routines that are going to run on loop once the whole OS is loaded.
 * 
 * @return void
 */
void loop()
{
    while (true)
    {
        shiro.shell.Update();
        delay(10);
    }
}

/**
 * shiro_main()
 *
 * This is the Shiro's entry point. 
 * Once the whole boot is loaded, assembly will call this function.
 * 
 * @return void
 */
extern "C" void shiro_main()
{
    // Load Shiro starting method (construtor)
    shiro.Start(shiro);

    // Load Shiro finishing method (destructor)
    shiro.Finish(shiro);

    // Call final loop
    loop();
}