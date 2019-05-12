#include <helper/debug.h>
#include <new.h>
#include "shiro.class.h"
#include "shell.class.h"

/**
 * loop()
 *
 * Here we can place routines that are going to run on loop once the whole OS is loaded.
 * 
 * @return void
 */
void loop()
{
    // Get Shell's Instance
    Applications::Shell *shell = Applications::Shell::GetInstance();

    // Loop
    while (true)
    {
        // Update Shell
        shell->Update();

        // Rinse and repeat
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
    // Get Shiro's Instance
    System::Shiro *shiro = System::Shiro::GetInstance();

    // Load Shiro starting method (construtor)
    shiro->Start();

    // Load Shiro finishing method (destructor)
    shiro->Finish();

    // Get Shell's Instance
    Applications::Shell *shell = Applications::Shell::GetInstance();

    // Now that Shiro is loaded, let's start our Shell Application
    shell->Start();

    // Call final loop
    loop();
}