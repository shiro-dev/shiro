#ifndef DEBUG_CLASS_H
#define DEBUG_CLASS_H

#include <stdlib.h>

/**
 * outb(auto port, auto data)
 *
 * This is an external function. 
 * You can find it's code on boot.asm.
 * This function will output data to a port, in our case we will output chars to the serial port for debugging purposes.
 */
extern "C" void outb(auto port, auto data);

/**
 * namespace System
 *
 * This is our core namespace.
 * All system-related classes can be found on this namespace.
 */
namespace System 
{
    /**
     * System::Debug()
     *
     * This class is responsible for all debug methods.
     * If you use:
     *  #include <helper/debug.h>
     * You will be able to use the System::Debug without calling this class directly:
     *  log("This is a string");
     */
    class Debug
    {
        public:
            
            /**
             * SERIAL_PORT = 0x3F8
             * 
             * We are going to use this port to output our logs to the serial port on QEMU.
             * 
             * @var unsigned short 
             */
            unsigned short SERIAL_PORT = 0x3F8;

            /**
             * System::Debug::Log(const char* value)
             *
             * This is our log function. 
             * It will send a string to our selected port, char by char.
             * 
             * @return void
             */
            void Log(const char* value)
            {
                for (size_t i = 0; i < strlen(value); i++)
                {
                    outb(this->SERIAL_PORT, value[i]);
                }
                outb(this->SERIAL_PORT, '\r');
                outb(this->SERIAL_PORT, '\n');
            }

            /**
             * System::Debug::Log(int value)
             *
             * This is our log function. 
             * It will convert int into char* to be sent to our selected port, char by char.
             * 
             * @return void
             */
            void Log(int value)
            {
                char* nvalue = int2char(value);
                for (size_t i = 0; i < strlen(nvalue); i++)
                {
                    outb(this->SERIAL_PORT, nvalue[i]);
                }
                outb(this->SERIAL_PORT, '\r');
                outb(this->SERIAL_PORT, '\n');
            }
    };
}

#endif