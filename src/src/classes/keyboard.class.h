#ifndef KEYBOARD_CLASS_H
#define KEYBOARD_CLASS_H

/**
 * namespace System
 *
 * This is our core namespace.
 * All system-related classes can be found on this namespace.
 */
namespace System
{
/**
 * System::Drivers
 *
 * This namespace contains all drivers developed for Shiro.
 */
namespace Drivers
{
/**
 * System::Drivers::Keyboard()
 *
 * This class is responsible for controlling the Keyboard.
 * The "keys" are going to be received by the ISR handler and sent here.
 */
class Keyboard
{
public:
    static char buffer[50];
    static bool was_updated;
    static uint16_t function_key;

    /**
     * System::Drivers::Keyboard::Start()
     *
     * This method will activate the keyboard.
     */
    static void Start()
    {
        // Log
        log("> Enabling Keyboard Interrupt");

        // Enable Keyboard
        asm("cli");
        outb(0x21, 0xFD);
        outb(0xA1, 0xFF);
        asm("sti");
    }

    /**
     * System::Drivers::Keyboard::Key2ASCII(uint8_t key)
     * 
     * This method is responsible for converting a scancode into a character or action
     * 
     * @return uint8_t
     */
    static uint8_t Key2ASCII(uint8_t key)
    {
        if (key == 0x1E)
            return 'a';
        if (key == 0x10)
            return 'q';
        if (key == 0x11)
            return 'w';
        if (key == 0x12)
            return 'e';
        if (key == 0x13)
            return 'r';
        if (key == 0x14)
            return 't';
        if (key == 0x15)
            return 'y';
        if (key == 0x16)
            return 'u';
        if (key == 0x17)
            return 'i';
        if (key == 0x18)
            return 'o';
        if (key == 0x19)
            return 'p';
        if (key == 0x1E)
            return 'a';
        if (key == 0x1F)
            return 's';
        if (key == 0x20)
            return 'd';
        if (key == 0x21)
            return 'f';
        if (key == 0x22)
            return 'g';
        if (key == 0x23)
            return 'h';
        if (key == 0x24)
            return 'j';
        if (key == 0x25)
            return 'k';
        if (key == 0x26)
            return 'l';
        if (key == 0x2C)
            return 'z';
        if (key == 0x2D)
            return 'x';
        if (key == 0x2E)
            return 'c';
        if (key == 0x2F)
            return 'v';
        if (key == 0x30)
            return 'b';
        if (key == 0x31)
            return 'n';
        if (key == 0x32)
            return 'm';
        if (key == 0x29)
            return '0';
        if (key == 0x02)
            return '1';
        if (key == 0x03)
            return '2';
        if (key == 0x04)
            return '3';
        if (key == 0x05)
            return '4';
        if (key == 0x06)
            return '5';
        if (key == 0x07)
            return '6';
        if (key == 0x08)
            return '7';
        if (key == 0x09)
            return '8';
        if (key == 0x0A)
            return '9';
        if (key == 0x34)
            return '.';
        if (key == 0x39)
            return ' ';
        if (key == 0x1C && GetBufferSize() > 0)
            return '\n';
        if (key == 0xE && GetBufferSize() > 0)
            BufferRemoveOne();
        if (key == 0x3B)
            SetFunctionKey(1);
        if (key == 0x3C)
            SetFunctionKey(2);
        if (key == 0x3D)
            SetFunctionKey(3);
        if (key == 0x3E)
            SetFunctionKey(4);
        if (key == 0x3F)
            SetFunctionKey(5);

        return false;
    }

    /**
     * System::Drivers::Keyboard::Handler();
     * 
     * This method will handle interactions with the keyboard
     * 
     * @return void 
     */
    static void Handler()
    {
        if (inb(0x64) & 1)
        {
            // Get scancode
            uint8_t scancode = inb(0x60);

            // Handle scancode
            const char key = Key2ASCII(scancode);

            // If we were able to convert scancode into character, add to buffer
            if (key)
            {
                BufferAdd(&key);
            }
        }
    }

    /**
     * System::Drivers::Keyboard::GetBuffer()
     * 
     * This method will return the current keyboard buffer
     * 
     * @return char 
     */
    static char *GetBuffer()
    {
        return buffer;
    }

    /**
     * System::Drivers::Keyboard::GetBufferSize()
     * 
     * This method will return the size of the current keyboard buffer
     * 
     * @return size_t 
     */
    static size_t GetBufferSize()
    {
        return strlen(buffer);
    }

    /**
     * System::Drivers::Keyboard::BufferRemoveOne()
     * 
     * This method will remove the last character of the keyboard buffer
     * 
     * @return void 
     */
    static void BufferRemoveOne()
    {
        // Get current buffer size
        size_t size = GetBufferSize();

        // Remove last character from command buffer
        if (size > 0)
        {
            if (size == 1)
            {
                buffer[0] = '\0';
            }
            else
            {
                buffer[size - 1] = '\0';
            }
        }

        // Mark as updated
        System::Drivers::Keyboard::UpdateNotification(true);
    }

    /**
     * System::Drivers::Keyboard::SetFunctionKey(uint16_t key)
     * 
     * This method will set a function key
     * 
     * @return void 
     */
    static void SetFunctionKey(uint16_t key)
    {
        function_key = key;
    }

    /**
     * System::Drivers::Keyboard::GetFunctionKey()
     * 
     * This method will get which function key was pressed
     * 
     * @return uint16_t 
     */
    static uint16_t GetFunctionKey()
    {
        // Store the selected function key to a temporary variable
        uint16_t return_function_key = function_key;

        // Reset the functino key to zero
        function_key = 0;

        // Return selected function key
        return return_function_key;
    }

    /**
     * System::Drivers::Keyboard::BufferAdd(const char *command)
     * 
     * This method will add a character to the current keyboard buffer
     * 
     * @return void 
     */
    static void BufferAdd(const char *command)
    {
        // Get Buffer Size
        size_t size = GetBufferSize();

        // Set linebreak
        const char *linebreak = "\n";

        // Don't let the user type more than 50 characters, except for \n
        if (size >= 50 && command != linebreak)
            return;

        // Let's copy the new command to the buffer
        if (size > 0)
        {
            const char *string = concat(buffer, command);
            strcpy(buffer, string);
        }
        else
        {
            strcpy(buffer, command);
        }

        // Mark as updated
        System::Drivers::Keyboard::UpdateNotification(true);
    }

    /**
     * System::Drivers::Keyboard::BufferReset()
     * 
     * This method will clean the current keyboard buffer
     * 
     * @return void 
     */
    static void BufferReset()
    {
        // Clean command buffer
        buffer[0] = '\0';

        // Mark as updated
        System::Drivers::Keyboard::UpdateNotification(true);
    }

    /**
     * System::Drivers::Keyboard::UpdateNotification()
     * 
     * This method will mark the buffer as updated so it can be user by other softwares
     * 
     * @return char 
     */
    static void UpdateNotification(bool flag)
    {
        System::Drivers::Keyboard::was_updated = flag;
    }

    /**
     * System::Drivers::Keyboard::WasUpdated()
     * 
     * This method can be used to see if the keyboard buffer was updated.
     * Once checked, the was_updated flag will be set to false.
     * 
     * @return char 
     */
    static bool WasUpdated()
    {
        bool wupd = System::Drivers::Keyboard::was_updated;
        if (wupd == true)
        {
            System::Drivers::Keyboard::UpdateNotification(false);
        }
        return wupd;
    }
};
} // namespace Drivers
} // namespace System

/**
 * @todo Move this somewhere else
 * This is not the best place for this, but it was the only way to make it work.
 */
char System::Drivers::Keyboard::buffer[50];
bool System::Drivers::Keyboard::was_updated = false;
uint16_t System::Drivers::Keyboard::function_key = 0;

#endif