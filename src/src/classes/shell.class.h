#ifndef SHELL_CLASS_H
#define SHELL_CLASS_H

/**
 * namespace System
 *
 * This is our core namespace.
 * All system-related classes can be found on this namespace.
 */
namespace System
{
class Shell
{
public:
    /**
     * Vars
     */
    uint8_t header_color;
    uint8_t terminal_color;
    uint8_t footer_color;
    uint16_t *screen;
    uint16_t *terminal_screen_header;
    uint16_t *terminal_screen_footer;
    uint16_t *terminal_screens[10];
    uint16_t current_terminal;
    uint16_t header_starting_point;
    uint16_t content_starting_point;
    uint16_t footer_starting_point;
    uint16_t header_size;
    uint16_t content_size;
    uint16_t footer_size;
    size_t terminal_header_width;
    size_t terminal_header_height;
    size_t terminal_content_width;
    size_t terminal_content_height;
    size_t terminal_footer_width;
    size_t terminal_footer_height;
    size_t screen_width;
    size_t screen_height;
    size_t header_current_x;
    size_t header_current_y;
    size_t content_current_x;
    size_t content_current_y;
    size_t footer_current_x;
    size_t footer_current_y;

    System::Cursor cursor;

    /**
     * System::Shell.Start(auto &shiro)
     *
     * This is our pseudo constructor, a starting point.
     * 
     * @return void
     */
    void Start(auto &shiro)
    {
        // Log
        log("> Starting Shell");

        // Vars
        System::Modes::Text textMode;

        // Colors
        System::VGA::colors headerTextColor = System::VGA::colors::VGA_COLOUR_WHITE;
        System::VGA::colors headerBackgroundColor = System::VGA::colors::VGA_COLOUR_BROWN;
        System::VGA::colors terminalTextColor = System::VGA::colors::VGA_COLOUR_WHITE;
        System::VGA::colors terminalBackgroundColor = System::VGA::colors::VGA_COLOUR_BLACK;
        System::VGA::colors footerTextColor = System::VGA::colors::VGA_COLOUR_WHITE;
        System::VGA::colors footerBackgroundColor = System::VGA::colors::VGA_COLOUR_BLACK;

        // Set Screen Address
        this->screen = textMode.screenAddr;

        // Define the initial terminal
        this->screen = (uint16_t *)0xB8000;
        this->current_terminal = 1;
        this->screen_width = 80;
        this->screen_height = 25;
        this->terminal_header_height = 1;
        this->terminal_header_width = this->screen_width;
        this->terminal_footer_height = 1;
        this->terminal_footer_width = this->screen_width;
        this->terminal_content_height = this->screen_height - (this->terminal_header_height + this->terminal_footer_height);
        this->terminal_content_width = this->screen_width;
        this->header_color = System::VGA::selectColors(headerTextColor, headerBackgroundColor);
        this->terminal_color = System::VGA::selectColors(terminalTextColor, terminalBackgroundColor);
        this->footer_color = System::VGA::selectColors(footerTextColor, footerBackgroundColor);
        this->header_starting_point = 0;                                                                                             // Should be 0
        this->content_starting_point = (this->terminal_header_height * this->terminal_header_width);                                 // Should be 160
        this->footer_starting_point = this->content_starting_point + (this->terminal_content_height * this->terminal_content_width); // Should be 1840
        this->header_size = this->header_starting_point + (this->terminal_header_height * this->terminal_header_width);
        this->content_size = (this->content_starting_point + (this->terminal_content_height * this->terminal_content_width));
        this->footer_size = (this->footer_starting_point + (this->terminal_footer_height * this->terminal_footer_width));
        this->header_current_x = 0;
        this->content_current_x = 0;
        this->footer_current_x = 0;
        /* _y is not being used at the moment */
        this->header_current_y = 0;
        this->content_current_y = 0;
        this->footer_current_y = 0;

        // Initilize Terminals
        // We are going to use 4 user terminals + 1 debug terminal
        this->StartTerminalHeader();
        this->StartTerminal(1);
        this->StartTerminal(2);
        this->StartTerminal(3);
        this->StartTerminal(4);
        this->StartTerminal(5); // Debug
        this->StartTerminalFooter();
        this->UpdateScreen();

        // Enabling Cursor
        cursor.enable();
        cursor.moveTo(7, (this->screen_height - 1));

        // Write header
        this->WriteH("Welcome to Shiro - Operating System");

        // Now that everything is set, let's enable the keyboard
        System::Drivers::Keyboard::Start();

        shiro.NotImplemented();
    }

    /**
     * System::Shell.UpdateScreen()
     *
     * This method will fetch information from all screen buffers and rebuild the whole screen.
     * 
     * @return void
     */
    void UpdateScreen()
    {
        // Log
        log("> Updating Screen");

        // Push Header to Screen
        size_t pos = 0;
        for (pos = this->header_starting_point; pos < this->header_size; pos++)
        {
            this->screen[pos] = this->terminal_screen_header[pos];
        }

        // Push Content to Screen
        pos = 0;
        for (pos = this->content_starting_point; pos < this->content_size; pos++)
        {
            this->screen[pos] = this->terminal_screens[this->current_terminal][pos];
        }

        // Push Footer to Screen
        pos = 0;
        for (pos = this->footer_starting_point; pos < this->footer_size; pos++)
        {
            this->screen[pos] = this->terminal_screen_footer[pos];
        }

        // Moving cursor
        size_t size = System::Drivers::Keyboard::GetBufferSize();
        cursor.moveTo((size + 7), (this->screen_height - 1));
    }

    /**
     * System::Shell.Update()
     *
     * This method is called by the OS on its final loop.
     * If the keyboard buffer was updated, this method will handle it.
     * 
     * @return void
     */
    void Update()
    {
        // Vars
        bool execute = false;

        // Fetch Buffer information
        char *keyboardBuffer = System::Drivers::Keyboard::GetBuffer();
        size_t keyboardBufferSize = System::Drivers::Keyboard::GetBufferSize();
        bool was_updated = System::Drivers::Keyboard::WasUpdated();

        // If no changes were made to the buffer, we don't need to do anything
        if (was_updated == false)
            return;

        // Let's check if we should execute this command
        for (size_t i = 0; i < keyboardBufferSize; i++)
        {
            if (keyboardBuffer[i] == '\n')
            {
                execute = true;
            }
        }

        // Clean the footer
        this->StartTerminalFooter();

        // Handle Command Execution
        if (execute == true && keyboardBufferSize > 1)
        {
            // Set execute to FALSE
            execute = false;

            // Write command on content/screen
            this->WriteC(keyboardBuffer);

            // Reset buffer
            System::Drivers::Keyboard::BufferReset();

            // Clean the footer
            this->StartTerminalFooter();

            // Push to screen
            this->UpdateScreen();
        }
        else
        {
            // Write command on footer
            this->WriteF(keyboardBuffer);
        }
    }

    /**
     * System::Shell.StartTerminalHeader()
     *
     * This method will kick-start our terminal header.
     * 
     * @return void
     */
    void StartTerminalHeader()
    {
        // Log
        log("> Initializing Terminal Header");

        // Define the size of the header and where to start
        size_t size = this->header_starting_point + (this->terminal_header_height * this->terminal_header_width);

        // Write header to header buffer
        for (size_t pos = 0; pos < size; pos++)
        {
            this->terminal_screen_header[pos] = System::VGA::setColor(' ', this->header_color);
        }
    }

    /**
     * System::Shell.StartTerminalFooter()
     *
     * This method will kick-start our terminal footer.
     * 
     * @return void
     */
    void StartTerminalFooter()
    {
        // Log
        log("> Initializing Terminal Footer (User Point)");

        // Define the size of the footer and where to start
        size_t size = this->footer_starting_point + (this->terminal_footer_height * this->terminal_footer_width);

        // Write footer to footer buffer
        for (size_t pos = this->footer_starting_point; pos < size; pos++)
        {
            this->terminal_screen_footer[pos] = System::VGA::setColor(' ', this->footer_color);
        }

        // Reset footer position
        this->footer_current_x = 0;

        // Write footer
        this->WriteF("Shiro> ");
    }

    /**
     * System::Shell.StartTerminal()
     *
     * This method will kick-start all our terminals.
     * 
     * @return void
     */
    void StartTerminal(uint16_t terminal_id)
    {
        // Log
        log("> Initializing Terminal $terminal_id");

        // Define the size of the content and where to start
        size_t size = this->content_starting_point + (this->terminal_content_height * this->terminal_content_width);

        // Write content to content buffer
        for (size_t pos = this->content_starting_point; pos < size; pos++)
        {
            this->terminal_screens[terminal_id][pos] = System::VGA::setColor(' ', this->terminal_color);
        }
    }

    /**
     * System::Shell.StartTerminalFooter()
     *
     * This method will wrinte content to the currently active terminal.
     * 
     * @return void
     */
    void WriteC(const char *data)
    {
        size_t gi = 0;
        size_t last_line_starting_point = this->terminal_content_width * this->terminal_content_height;
        size_t limit = this->content_size - 2;

        for (size_t i = 0; i < strlen(data); i++)
        {
            if (data[i] == '\n')
            {
                this->Scroll();
                continue;
            }
            if (gi >= limit)
            {
                this->content_current_x = 0;
            }
            gi = last_line_starting_point + this->content_current_x;
            this->terminal_screens[this->current_terminal][gi] = System::VGA::setColor(data[i], this->terminal_color);
            this->content_current_x++;
        }

        this->Scroll();
        this->UpdateScreen();
    }

    /**
     * System::Shell.WriteH()
     *
     * This method will write content to our header.
     * 
     * @return void
     */
    void WriteH(const char *data)
    {
        size_t gi = 0;
        size_t limit = this->header_size - 1;

        for (size_t i = 0; i < strlen(data); i++)
        {
            if (gi >= limit)
            {
                this->header_current_x = 0;
            }
            gi = this->header_starting_point + this->header_current_x;
            this->terminal_screen_header[gi] = System::VGA::setColor(data[i], this->header_color);
            this->header_current_x++;
        }
        this->UpdateScreen();
    }

    /**
     * System::Shell.WriteF()
     *
     * This method will write content to our footer.
     * 
     * @return void
     */
    void WriteF(const char *data)
    {
        size_t gi = 0;
        size_t limit = this->footer_size - 1;

        for (size_t i = 0; i < strlen(data); i++)
        {
            if (gi >= limit)
            {
                this->footer_current_x = 0;
            }
            gi = this->footer_starting_point + this->footer_current_x;
            this->terminal_screen_footer[gi] = System::VGA::setColor(data[i], this->footer_color);
            this->footer_current_x++;
        }
        this->UpdateScreen();
    }

    /**
     * System::Shell.WriteToH()
     *
     * This method will write content to our header on a specific coordinate.
     * 
     * @return void
     */
    void WriteToH(const char *data, int x, int y)
    {
        size_t org_x = this->header_current_x;
        size_t org_y = this->header_current_y;

        this->header_current_x = x - 1;
        this->header_current_y = y;

        this->WriteH(data);

        this->header_current_x = org_x;
        this->header_current_y = org_y;
    }

    /**
     * System::Shell.Scroll()
     *
     * This method will scroll the content on our currently active terminal.
     * 
     * @return void
     */
    void Scroll()
    {
        size_t i = 0;
        size_t limit = this->terminal_content_height;

        for (size_t line = 1; line < limit; line++)
        {
            for (i = 0; i < 80; i++)
            {
                this->terminal_screens[this->current_terminal][((line)*80) + i] = this->terminal_screens[this->current_terminal][((line + 1) * 80) + i];
            }
        }

        for (i = ((this->screen_height - this->terminal_footer_height - 1) * 80); i < (80 * 24); i++)
        {
            this->terminal_screens[this->current_terminal][i] = System::VGA::setColor(' ', this->terminal_color);
        }

        this->content_current_x = 0;
    }

    /**
     * System::Shell.Prepare(const char* content)
     *
     * This method will prepare the content that will be outputted.
     * 
     * @param const char* content
     * @return const char*
     */
    const char *Prepare(const char *content)
    {
        return content;
    }

    /**
     * System::Shell.Prepare(int content)
     *
     * This method will prepare the content that will be outputted.
     * 
     * @param int content
     * @return void
     */
    const char *Prepare(int content)
    {
        return int2char(content);
    }

    /**
     * System::Shell.echo(Any content)
     *
     * This method will write content to our currently active terminal.
     * 
     * @return void
     */
    template <typename Any>
    void echo(Any content)
    {
        const char *newContent = this->Prepare(content);
        this->WriteC(newContent);
    }

    /**
     * System::Shell.echo(Any content, Rest... rest)
     *
     * This method will write more than one content to our currently active terminal.
     * 
     * @return void
     */
    template <typename Any, typename... Rest>
    void echo(Any content, Rest... rest)
    {
        const char *newContent = this->Prepare(content);
        this->WriteC(newContent);
        echo(content, rest...);
    }
};
} // namespace System

#endif