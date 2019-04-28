#include "libs/stdlib.h"
#include "classes/debug.h"
#include "classes/vga.h"
#include "classes/console.h"
#include "classes/seeder.h"

extern "C" void shiro_main() 
{
    Log("Shiro Debugger: Initialized");

    console_initialize();
    Log("Terminal: Initialized");

    console_writestring("Welcome to Shiro\n");
    
    seeder_run();
}