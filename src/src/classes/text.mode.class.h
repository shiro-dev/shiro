#ifndef TEXT_MODE_CLASS_H
#define TEXT_MODE_CLASS_H

/**
 * namespace System
 *
 * This is our core namespace.
 * All system-related classes can be found on this namespace.
 */
namespace System
{
/**
 * namespace System::Modes
 *
 * This namespace contains classes related to mode-switching.
 * You will find a Video Mode class and a Text Mode class.
 */
namespace Modes
{
/**
 * System::Modes::Text()
 *
 * This class is responsible for all interactions with the Text Mode.
 */
class Text
{
public:
    /**
     * Vars
     */
    uint16_t *screenAddr = (uint16_t *)0xB8000;

    /**
     * System::Modes::Text.Start()
     *
     * This is our pseudo constructor, a starting point.
     * Here we can implement methods that will initialize the Text Mode.
     * 
     * @return void
     */
    void Start()
    {
        // Log
        log("> Entering Text Mode");
    }

    /**
     * System::Modes::Text.Finish()
     *
     * This is our pseudo destructor, a finishing point.
     * Here we can implement methods that will be executed when we leave the Text Mode.
     * 
     * @return void
     */
    void Finish()
    {
        // Log
        log("> Running Text Mode final routines");
    }
};
} // namespace Modes
} // namespace System

#endif