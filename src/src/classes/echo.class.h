#ifndef ECHO_CLASS_H
#define ECHO_CLASS_H

#include <stdlib.h>
#include <vga.h>
#include <console.h>

/**
 * namespace PHP
 *
 * This namespace contains classes that have a similar syntax to PHP.
 * Why? Just because.
 */
namespace PHP 
{
    /**
     * PHP::Echo()
     *
     * This class is responsible for outputting text to the screen in Text Mode.
     * If you use:
     *  #include <helper/php.h>
     * You will be able to use the PHP::Echo without calling this class directly:
     *  echo("This is a string");
     */
    class Echo
    {
        private:

            /**
             * PHP::Echo.Prepare(const char* content)
             *
             * This method will prepare the content that will be outputted.
             * 
             * @param const char* content
             * @return void
             */
            void Prepare(const char* content)
            {
                this->Output(content);
            }

            /**
             * PHP::Echo.Prepare(int content)
             *
             * This method will prepare the content that will be outputted.
             * 
             * @param int content
             * @return void
             */
            void Prepare(int content)
            {
                this->Output(int2char(content));
            }

            /**
             * PHP::Echo.Output(const char* content)
             *
             * This method is responsible for the actual output.
             * 
             * @param const char* content
             * @return void
             */
            void Output(const char* content)
            {
                const char* newline = "\n";
                console_write(content, strlen(content));
                console_write(newline, strlen(newline));
            }

        public:

            /**
             * PHP::Echo.Call(Any content)
             *
             * This is the method you call to output something.
             * Example:
             *  PHP::Echo echo;
             *  echo.Call("This is a string");
             * 
             * Because of it's type, this method will accept any variable type,
             * but at the moment only strings and ints can be outputted.
             * 
             * @param Any content
             * @return void
             */
            template <typename Any>
            void Call(Any content)
            {
                this->Prepare(content);
            }

            /**
             * PHP::Echo.Call(Any content, Rest... rest)
             *
             * This is the method you call to output something.
             * It is responsible for multiple outputs on the same call.
             * Example:
             *  PHP::Echo echo;
             *  echo.Call("A", "B", "C", "D", "E");
             * 
             * Because of it's type, this method will accept any variable type,
             * but at the moment only strings and ints can be outputted.
             * 
             * @param Any content
             * @param Rest... rest
             * @return void
             */
            template <typename Any, typename... Rest>
            void Call(Any content, Rest... rest)
            {
                this->Prepare(content);
                this->Call(rest...);
            }
    };
}

#endif