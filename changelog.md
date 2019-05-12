# Changelog

## [v0.4] - 2019-05-12

- Adding new methods and functions
- Improving ```Keyboard Driver```
    - <small>New ```Numeral Keys```</small>
    - <small>New ```Function Keys``` from ```F1``` to ```F5```</small>
- Implementing a new ```Applications``` namespace
- Implementing a Singleton Pattern for Shiro (kernel)
    - <small>The ```(auto &shiro)``` parameter is no longer required when calling a method</small>
    - <small>Now you can call ```System::Shiro::GetInstance()``` to access Shiro's methods</small>
- Implementing a Singleton Pattern for Shell (application)
    - <small>Now you can call ```Applications::Shell::GetInstance()``` to access the Shell's methods</small>
- Implementing a ```WatterMark Allocator``` for ```Memory Management```
    - <small>This is a very (very, very, ..., very) basic version of the feature</small>
- Implementing the ```new``` operator
- Implementing a Shell Switcher
    - <small>You can now change terminals by pressing ```F1``` to ```F5```</small>
    - <small>```F5``` is the ```Debug Terminal```</small>
    - <small>The current terminal is displayed at the top of the screen (header)</small>
- Removing ```VGA.Start()``` from inside ```Modes::Text.Start()```
    - <small>Now you can find it at ```Shiro.Start()```</small>
- Removing Shell from inside Shiro
    - <small>Now, instead of being part of Shiro, it can use Shiro's resources as an application.</small>
- Removing unused ```bios``` class from ```cursor``` class

## [v0.3] - 2019-05-11

- New namespaces, classes, methods, and functions
- Moving certain lib functions into classes/methods
- Implementing a Global Descriptor Table (GDT)
- Implementing a Interrupt Descriptor Table (IDT)
- Implementing Interrupt Service Routines (ISR)
- Implementing a Shell, Shiro's first application
- Implementing a structure to support up to 5 shell screens (switcher not implemented yet)
- Creating a very basic keyboard driver to allow user interactions with the Shell
- Removing PHP-like functions
- Adding a final loop, to group routines to be executed in a loop after the OS is loaded
- Adding a "make debug" option on the Makefile that will output debugging information into the cmd/terminal
- Adding a log folder
- Adding new lib functions
- Updating GCC Cross-Compiler from 7.1.0 to 9.1.0
- Adding Golang support to Cross-Compiler
- Changing build parameters to hide unecessary warnings and notices
- Adding Composer to Dockerfile
- Adding AST-PECL to Dockerfile
- Adding LLVM to Dockerfile
- Adding other libs to Dockerfile
- Adding more names to the "Special Thanks" area on the readme file

## [v0.2] - 2019-05-01

- New file structure
- New namespaces, classes, methods, and functions
- Debugging methods and functions
- Preparations for the Video Mode (actual mode not implemented yet)
- Organizing Makefiles
- Improving build bash script
- Adding "Special Thanks" section to readme
- Adding comments to classes and methods
- Adding a Real Mode Switcher (by Napalm)
- Adding PHP-like functions

## [v0.1] - 2019-04-28
