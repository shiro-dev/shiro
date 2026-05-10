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
 * Full PS/2 scancode set 1 driver for a US QWERTY layout.
 *
 *  - Tracks key release (high bit of scancode) so modifiers can be released.
 *  - Tracks Shift / Ctrl / Alt / Caps Lock state.
 *  - Handles the 0xE0 "extended" prefix (arrows, etc) — currently swallowed
 *    rather than mapped, but the prefix logic is in place so a future patch
 *    only has to fill the extended-key table.
 *  - F1–F5 still drive the terminal switcher.
 */
class Keyboard
{
public:
    static char buffer[50];
    static bool was_updated;
    static uint16_t function_key;

    // Modifier / state flags. Public so anyone can poll if needed.
    static bool shift_pressed;
    static bool ctrl_pressed;
    static bool alt_pressed;
    static bool caps_lock;
    static bool extended_prefix;

    /**
     * System::Drivers::Keyboard::Start()
     *
     * Unmasks IRQ1 on the (already-remapped) master PIC. After PIC::Start
     * the master is at vector base 0x20, so the keyboard arrives as INT 0x21.
     * IF is enabled at the very end of Shiro::Start.
     */
    static void Start()
    {
        log("> Enabling Keyboard (IRQ1 -> INT 0x21)");
        System::PIC::Unmask(1);
    }

    /**
     * System::Drivers::Keyboard::Handler()
     *
     * Called from the IRQ1 ISR. Reads one byte from the PS/2 data port,
     * interprets it as a scancode, and either updates internal state or
     * appends the resulting ASCII character to the keyboard buffer.
     */
    static void Handler()
    {
        // Only act if the controller actually has data ready.
        if (!(inb(0x64) & 1))
            return;

        uint8_t sc = inb(0x60);

        // 0xE0 introduces an extended scancode (arrow keys, right-modifiers,
        // numpad / etc). Eat the prefix and remember it for the next byte.
        if (sc == 0xE0)
        {
            extended_prefix = true;
            return;
        }

        bool released = (sc & 0x80) != 0;
        uint8_t code = sc & 0x7F;

        if (extended_prefix)
        {
            extended_prefix = false;
            // (Future: arrows, Right Ctrl/Alt, Home/End, Insert/Delete, etc.)
            return;
        }

        // ---- Modifier keys (set/clear state, no buffer write) ----
        switch (code)
        {
        case 0x2A: // Left Shift
        case 0x36: // Right Shift
            shift_pressed = !released;
            return;
        case 0x1D: // Left Ctrl
            ctrl_pressed = !released;
            return;
        case 0x38: // Left Alt
            alt_pressed = !released;
            return;
        case 0x3A: // Caps Lock — toggle on press only
            if (!released)
                caps_lock = !caps_lock;
            return;
        }

        // From here on we only care about presses.
        if (released)
            return;

        // ---- Function keys (F1–F5 = terminal switcher) ----
        if (code >= 0x3B && code <= 0x3F)
        {
            SetFunctionKey((uint16_t)(code - 0x3A));
            return;
        }

        // ---- Look up the printable character ----
        char c = shift_pressed ? Shifted(code) : Unshifted(code);
        if (c == 0)
            return;

        // Caps Lock affects letters only, and only inverts shift's effect.
        if (caps_lock && c >= 'a' && c <= 'z')
            c = (char)(c - 32);
        else if (caps_lock && c >= 'A' && c <= 'Z')
            c = (char)(c + 32);

        // ---- Special handling: Backspace, Enter ----
        if (c == '\b')
        {
            if (GetBufferSize() > 0)
                BufferRemoveOne();
            return;
        }
        if (c == '\n')
        {
            // Mirror the original behaviour: an Enter on an empty buffer is
            // a no-op (keeps the prompt clean).
            if (GetBufferSize() == 0)
                return;
            const char nl[2] = { '\n', '\0' };
            BufferAdd(nl);
            return;
        }

        // ---- Regular printable: append as a 1-char null-terminated string. ----
        const char str[2] = { c, '\0' };
        BufferAdd(str);
    }

    /**
     * Scancode-set-1 → ASCII (no modifier).
     * Returns 0 for keys that don't produce a printable character on their own.
     */
    static char Unshifted(uint8_t code)
    {
        switch (code)
        {
        case 0x01: return 27;    // Esc
        case 0x02: return '1';
        case 0x03: return '2';
        case 0x04: return '3';
        case 0x05: return '4';
        case 0x06: return '5';
        case 0x07: return '6';
        case 0x08: return '7';
        case 0x09: return '8';
        case 0x0A: return '9';
        case 0x0B: return '0';
        case 0x0C: return '-';
        case 0x0D: return '=';
        case 0x0E: return '\b';  // Backspace
        case 0x0F: return '\t';  // Tab
        case 0x10: return 'q';
        case 0x11: return 'w';
        case 0x12: return 'e';
        case 0x13: return 'r';
        case 0x14: return 't';
        case 0x15: return 'y';
        case 0x16: return 'u';
        case 0x17: return 'i';
        case 0x18: return 'o';
        case 0x19: return 'p';
        case 0x1A: return '[';
        case 0x1B: return ']';
        case 0x1C: return '\n';  // Enter
        case 0x1E: return 'a';
        case 0x1F: return 's';
        case 0x20: return 'd';
        case 0x21: return 'f';
        case 0x22: return 'g';
        case 0x23: return 'h';
        case 0x24: return 'j';
        case 0x25: return 'k';
        case 0x26: return 'l';
        case 0x27: return ';';
        case 0x28: return '\'';
        case 0x29: return '`';
        case 0x2B: return '\\';
        case 0x2C: return 'z';
        case 0x2D: return 'x';
        case 0x2E: return 'c';
        case 0x2F: return 'v';
        case 0x30: return 'b';
        case 0x31: return 'n';
        case 0x32: return 'm';
        case 0x33: return ',';
        case 0x34: return '.';
        case 0x35: return '/';
        case 0x37: return '*';   // keypad-*
        case 0x39: return ' ';
        }
        return 0;
    }

    /**
     * Scancode-set-1 → ASCII when Shift is held.
     */
    static char Shifted(uint8_t code)
    {
        switch (code)
        {
        case 0x01: return 27;
        case 0x02: return '!';
        case 0x03: return '@';
        case 0x04: return '#';
        case 0x05: return '$';
        case 0x06: return '%';
        case 0x07: return '^';
        case 0x08: return '&';
        case 0x09: return '*';
        case 0x0A: return '(';
        case 0x0B: return ')';
        case 0x0C: return '_';
        case 0x0D: return '+';
        case 0x0E: return '\b';
        case 0x0F: return '\t';
        case 0x10: return 'Q';
        case 0x11: return 'W';
        case 0x12: return 'E';
        case 0x13: return 'R';
        case 0x14: return 'T';
        case 0x15: return 'Y';
        case 0x16: return 'U';
        case 0x17: return 'I';
        case 0x18: return 'O';
        case 0x19: return 'P';
        case 0x1A: return '{';
        case 0x1B: return '}';
        case 0x1C: return '\n';
        case 0x1E: return 'A';
        case 0x1F: return 'S';
        case 0x20: return 'D';
        case 0x21: return 'F';
        case 0x22: return 'G';
        case 0x23: return 'H';
        case 0x24: return 'J';
        case 0x25: return 'K';
        case 0x26: return 'L';
        case 0x27: return ':';
        case 0x28: return '"';
        case 0x29: return '~';
        case 0x2B: return '|';
        case 0x2C: return 'Z';
        case 0x2D: return 'X';
        case 0x2E: return 'C';
        case 0x2F: return 'V';
        case 0x30: return 'B';
        case 0x31: return 'N';
        case 0x32: return 'M';
        case 0x33: return '<';
        case 0x34: return '>';
        case 0x35: return '?';
        case 0x37: return '*';
        case 0x39: return ' ';
        }
        return 0;
    }

    /**
     * Legacy API kept for source compatibility — returns the unshifted char.
     */
    static uint8_t Key2ASCII(uint8_t key)
    {
        return (uint8_t)Unshifted(key);
    }

    static char *GetBuffer() { return buffer; }
    static size_t GetBufferSize() { return strlen(buffer); }

    static void BufferRemoveOne()
    {
        size_t size = GetBufferSize();
        if (size > 0)
        {
            buffer[size - 1] = '\0';
        }
        UpdateNotification(true);
    }

    static void SetFunctionKey(uint16_t key) { function_key = key; }

    static uint16_t GetFunctionKey()
    {
        uint16_t k = function_key;
        function_key = 0;
        return k;
    }

    static void BufferAdd(const char *command)
    {
        size_t size = GetBufferSize();
        const char *linebreak = "\n";

        // Cap at 50, except for Enter which we always allow through so the
        // shell can detect end-of-line.
        if (size >= 50 && command != linebreak)
            return;

        if (size > 0)
        {
            const char *string = concat(buffer, command);
            strcpy(buffer, string);
        }
        else
        {
            strcpy(buffer, command);
        }

        UpdateNotification(true);
    }

    static void BufferReset()
    {
        buffer[0] = '\0';
        UpdateNotification(true);
    }

    static void UpdateNotification(bool flag) { was_updated = flag; }

    static bool WasUpdated()
    {
        bool wupd = was_updated;
        if (wupd)
            UpdateNotification(false);
        return wupd;
    }
};
} // namespace Drivers
} // namespace System

// Static member definitions
char System::Drivers::Keyboard::buffer[50];
bool System::Drivers::Keyboard::was_updated = false;
uint16_t System::Drivers::Keyboard::function_key = 0;
bool System::Drivers::Keyboard::shift_pressed = false;
bool System::Drivers::Keyboard::ctrl_pressed = false;
bool System::Drivers::Keyboard::alt_pressed = false;
bool System::Drivers::Keyboard::caps_lock = false;
bool System::Drivers::Keyboard::extended_prefix = false;

#endif
