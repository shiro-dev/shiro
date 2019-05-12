#ifndef SHELL_CLASS_H
#define SHELL_CLASS_H

/**
 * namespace Applications
 *
 * This is the namespace for all our base applications.
 * All application-related classes can be found on this namespace.
 */
namespace Applications
{
class Shell
{
private:
    /**
     * This parameter will hold our instance.
     */
    static Applications::Shell *instance;

    /**
     * Adding a private contructor.
     */
    Shell(){};

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
    uint16_t *terminal_screens[10][3000];
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

    /**
     * This parameter will set/return our current instance
     */
    static Applications::Shell *GetInstance()
    {
        if (instance == 0)
        {
            instance = new Applications::Shell();
        }

        return instance;
    }

    /**
     * Applications::Shell.Start()
     *
     * This is our pseudo constructor, a starting point.
     * 
     * @return void
     */
    void Start()
    {
        // Log
        log("\n> Starting Shell");

        // Get Shiro's Instance
        System::Shiro *shiro = System::Shiro::GetInstance();

        // Colors
        System::VGA::colors headerTextColor = shiro->vga.VGA_COLOUR_WHITE;
        System::VGA::colors headerBackgroundColor = shiro->vga.VGA_COLOUR_BROWN;
        System::VGA::colors terminalTextColor = shiro->vga.VGA_COLOUR_WHITE;
        System::VGA::colors terminalBackgroundColor = shiro->vga.VGA_COLOUR_BLACK;
        System::VGA::colors footerTextColor = shiro->vga.VGA_COLOUR_WHITE;
        System::VGA::colors footerBackgroundColor = shiro->vga.VGA_COLOUR_BLACK;

        // Set Screen Address
        this->screen = shiro->textMode.screenAddr;

        // Define the initial terminal
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
        shiro->cursor.enable();
        shiro->cursor.moveTo(7, (this->screen_height - 1));

        // Write header
        this->WriteH("Welcome to Shiro - Operating System");

        // Now that everything is set, let's enable the keyboard
        System::Drivers::Keyboard::Start();

        // Routines that should run once the shell is loaded
        this->Finish();
    }

    /**
     * Applications::Shell.Finish()
     *
     * This is our pseudo destructor, a finishing point.
     * Here we can implement our final routines for the Shell application.
     * 
     * @return void
     */
    void Finish()
    {
        // Log
        log("> Finishing Shell\n");

        // Get current terminal
        uint16_t active_terminal = this->current_terminal;

        // Write message to terminal 1 to 4
        for (size_t terminal_id = 1; terminal_id <= 4; terminal_id++)
        {
            this->current_terminal = terminal_id;
            this->echo("> SHIRO IS LOADED AND READY TO GO\n");
        }

        // Write message to terminal 5 (debug)
        this->current_terminal = 5;
        this->echo("> THE DEBUG TERMINAL IS LOADED AND READY TO GO\n");

        // Set terminal back to the active one
        this->current_terminal = active_terminal;

        // Update Sccreen
        this->UpdateScreen();
    }

    /**
     * Applications::Shell.UpdateScreen()
     *
     * This method will fetch information from all screen buffers and rebuild the whole screen.
     * 
     * @return void
     */
    void UpdateScreen()
    {
        // Get Shiro's Instance
        System::Shiro *shiro = System::Shiro::GetInstance();

        /** 
         * We need to keep track of our current terminal
         * That information will be displayed at the header
         */
        this->WriteToH(concat("| Terminal #", int2char(this->current_terminal)), (80 - 13), 0);

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

        // Set cursor position
        shiro->cursor.moveTo((size + 7), (this->screen_height - 1));
    }

    /**
     * Applications::Shell.Update()
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

        /**
         * Before checking the actual Keyboard Buffer, let's check function keys.
         * In the context of this shell, the function key will be a terminal switcher.
         * If a user selectes F1, it will display terminal 1.
         * The same is true for all terminals up to F5.
         */
        uint16_t function_key = System::Drivers::Keyboard::GetFunctionKey();

        // Now we handle the function key, if it is selected
        if (function_key > 0)
        {
            // Change current terminal
            this->current_terminal = function_key;

            // Update Screen
            this->UpdateScreen();
        }

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
     * Applications::Shell.StartTerminalHeader()
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
     * Applications::Shell.StartTerminalFooter()
     *
     * This method will kick-start our terminal footer.
     * 
     * @return void
     */
    void StartTerminalFooter()
    {
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
     * Applications::Shell.StartTerminal()
     *
     * This method will kick-start all our terminals.
     * 
     * @return void
     */
    void StartTerminal(uint16_t terminal_id)
    {
        // Log
        log("> Initializing Terminal #", terminal_id);

        // Define the size of the content and where to start
        size_t size = this->content_starting_point + (this->terminal_content_height * this->terminal_content_width);

        // Write content to content buffer
        for (size_t pos = this->content_starting_point; pos < size; pos++)
        {
            this->terminal_screens[terminal_id][pos] = System::VGA::setColorContent(' ', this->terminal_color);
        }
    }

    /**
     * Applications::Shell.WriteC()
     *
     * This method will wrinte content to the currently active terminal.
     * 
     * @return void
     */
    void WriteC(const char *data, bool update = true)
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
            this->terminal_screens[this->current_terminal][gi] = System::VGA::setColorContent(data[i], this->terminal_color);
            this->content_current_x++;
        }

        this->Scroll();

        if (update)
        {
            this->UpdateScreen();
        }
    }

    /**
     * Applications::Shell.WriteD()
     *
     * This method will wrinte content to the debug terminal.
     * 
     * @return void
     */
    void WriteD(const char *data)
    {
        // Get current terminal
        uint16_t active_terminal = this->current_terminal;

        // Change current_terminal to Debug Terminal (#5)
        this->current_terminal = 5;

        // Write the message
        this->WriteC(data, false);

        // Return to the current terminal
        this->current_terminal = active_terminal;

        this->UpdateScreen();
    }

    /**
     * Applications::Shell.WriteH(const char *data, bool update = true)
     *
     * This method will write content to our header.
     * 
     * @return void
     */
    void WriteH(const char *data, bool update = true)
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

        if (update)
        {
            this->UpdateScreen();
        }
    }

    /**
     * Applications::Shell.WriteF()
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
     * Applications::Shell.WriteToH()
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

        this->WriteH(data, false);

        this->header_current_x = org_x;
        this->header_current_y = org_y;
    }

    /**
     * Applications::Shell.Scroll()
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
            this->terminal_screens[this->current_terminal][i] = System::VGA::setColorContent(' ', this->terminal_color);
        }

        this->content_current_x = 0;
    }

    /**
     * Applications::Shell.Prepare(const char* content)
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
     * Applications::Shell.Prepare(int content)
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
     * Applications::Shell.echo(Any content)
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
     * Applications::Shell.echo(Any content, Rest... rest)
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
} // namespace Applications

// Let's mark our current instance as non-existing
Applications::Shell *Applications::Shell::instance = 0;

#endif