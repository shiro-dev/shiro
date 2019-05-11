#ifndef VIDEO_MODE_CLASS_H
#define VIDEO_MODE_CLASS_H

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
 * System::Modes::Video()
 *
 * This class is responsible for all interactions with the Video Mode.
 */
class Video
{
public:
    /**
     * System::Modes::Video.Start()
     *
     * This is our pseudo constructor, a starting point.
     * Here we can implement methods that will initialize the Video Mode.
     * 
     * @return void
     */
    void Start()
    {
        log("> Entering Video Mode");
    }

    /**
     * System::Modes::Video.Finish()
     *
     * This is our pseudo destructor, a finishing point.
     * Here we can implement methods that will be executed when we leave the Video Mode.
     * 
     * @return void
     */
    void Finish()
    {
        log("> Leaving Video Mode");
    }
};
} // namespace Modes
} // namespace System

#endif