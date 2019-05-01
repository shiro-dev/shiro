#ifndef SHIRO_CLASS_H
#define SHIRO_CLASS_H

#include "bios.class.h"
#include "text.mode.class.h"
#include "video.mode.class.h"

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
             * This private parameter is responsible for all text mode interactions.
             */
            System::Modes::Text textMode;

            /**
             * This private parameter is responsible for all video mode interactions.
             */
            System::Modes::Video videoMode;

        public:

            /**
             * System::Shiro()
             *
             * This is our actual constructor and it is called everytime the System::Shiro() class is instantiated.
             */
            Shiro()
            {
                log("Welcome to Shiro - Operating System");
                log("--------------------------------------------------------------------------------");
                log("");

                this->textMode.Start();
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
                echo("Welcome to Shiro - Operating System");
                echo("--------------------------------------------------------------------------------");
                echo("");
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
                this->textMode.Finish();
            }
    };
}

#endif