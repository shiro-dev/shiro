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
// Forward decl — commands.class.h provides the full definition. We include
// it at the bottom of this header, after Shell is fully visible.
class Commands;

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
     *
     * Layout (80x25 VGA text mode):
     *   row 0       header — title + tabs + clock
     *   row 1       status bar — uptime / mem / pmm / tasks
     *   rows 2..23  content scrollback (22 rows, chat-style: new at bottom)
     *   row 24      prompt
     */
    uint8_t header_color;
    uint8_t header_color_tab_active;
    uint8_t header_color_tab_inactive;
    uint8_t status_color;
    uint8_t terminal_color;
    uint8_t terminal_color_echo;       // yellow — for commands the user typed
    uint8_t terminal_color_box;        // cyan  — for box borders
    uint8_t terminal_color_highlight;  // light cyan — for emphasized values
    uint8_t footer_color;
    uint16_t *screen;
    uint16_t terminal_screen_header[80];
    uint16_t terminal_screen_status[160];   // row 1 — status bar
    uint16_t terminal_screen_footer[2000];
    uint16_t *terminal_screens[10][3000];
    uint16_t current_terminal;
    uint16_t header_starting_point;
    uint16_t status_starting_point;
    uint16_t content_starting_point;
    uint16_t footer_starting_point;
    uint16_t header_size;
    uint16_t status_size;
    uint16_t content_size;
    uint16_t footer_size;
    size_t terminal_header_width;
    size_t terminal_header_height;
    size_t terminal_status_width;
    size_t terminal_status_height;
    size_t terminal_content_width;
    size_t terminal_content_height;
    size_t terminal_footer_width;
    size_t terminal_footer_height;
    size_t screen_width;
    size_t screen_height;
    size_t header_current_x;
    size_t header_current_y;
    size_t status_current_x;
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
        log("\n> Starting Shell");

        System::Shiro *shiro = System::Shiro::GetInstance();

        // Colour palette (VGA text mode 16-colour).
        //   Header   : white on dark blue   (the "title bar")
        //   Tab on   : black on white       (active terminal)
        //   Tab off  : light grey on blue   (inactive terminals)
        //   Status   : black on light grey  (live system metrics)
        //   Content  : light grey on black  (scrollback area)
        //   Echo     : light brown (yellow) — typed commands
        //   Box      : light cyan           — frame borders
        //   Highlt.  : light cyan           — highlighted values
        //   Prompt   : light green on black (Shiro>)
        this->header_color =
            System::VGA::selectColors(System::VGA::VGA_COLOUR_WHITE,
                                      System::VGA::VGA_COLOUR_BLUE);
        this->header_color_tab_active =
            System::VGA::selectColors(System::VGA::VGA_COLOUR_BLACK,
                                      System::VGA::VGA_COLOUR_WHITE);
        this->header_color_tab_inactive =
            System::VGA::selectColors(System::VGA::VGA_COLOUR_LIGHT_GREY,
                                      System::VGA::VGA_COLOUR_BLUE);
        this->status_color =
            System::VGA::selectColors(System::VGA::VGA_COLOUR_BLACK,
                                      System::VGA::VGA_COLOUR_LIGHT_GREY);
        this->terminal_color =
            System::VGA::selectColors(System::VGA::VGA_COLOUR_LIGHT_GREY,
                                      System::VGA::VGA_COLOUR_BLACK);
        this->terminal_color_echo =
            System::VGA::selectColors(System::VGA::VGA_COLOUR_LIGHT_BROWN,
                                      System::VGA::VGA_COLOUR_BLACK);
        this->terminal_color_box =
            System::VGA::selectColors(System::VGA::VGA_COLOUR_LIGHT_CYAN,
                                      System::VGA::VGA_COLOUR_BLACK);
        this->terminal_color_highlight =
            System::VGA::selectColors(System::VGA::VGA_COLOUR_LIGHT_CYAN,
                                      System::VGA::VGA_COLOUR_BLACK);
        this->footer_color =
            System::VGA::selectColors(System::VGA::VGA_COLOUR_LIGHT_GREEN,
                                      System::VGA::VGA_COLOUR_BLACK);

        this->screen = shiro->textMode.screenAddr;

        // Layout in screen-cell positions (each row = 80 cells).
        //   row 0       header             positions     0 ..   80
        //   row 1       status             positions    80 ..  160
        //   rows 2..23  content (22 rows)  positions   160 .. 1920
        //   row 24      footer / prompt    positions  1920 .. 2000
        this->current_terminal = 1;
        this->screen_width = 80;
        this->screen_height = 25;

        this->terminal_header_height = 1;
        this->terminal_header_width  = this->screen_width;
        this->terminal_status_height = 1;
        this->terminal_status_width  = this->screen_width;
        this->terminal_footer_height = 1;
        this->terminal_footer_width  = this->screen_width;
        this->terminal_content_height = this->screen_height -
            (this->terminal_header_height + this->terminal_status_height +
             this->terminal_footer_height);
        this->terminal_content_width = this->screen_width;

        this->header_starting_point  = 0;
        this->status_starting_point  = (uint16_t)(this->terminal_header_height * this->screen_width);
        this->content_starting_point = (uint16_t)(this->status_starting_point + this->terminal_status_height * this->screen_width);
        this->footer_starting_point  = (uint16_t)(this->content_starting_point + this->terminal_content_height * this->screen_width);

        this->header_size  = (uint16_t)(this->header_starting_point  + this->terminal_header_height  * this->screen_width);
        this->status_size  = (uint16_t)(this->status_starting_point  + this->terminal_status_height  * this->screen_width);
        this->content_size = (uint16_t)(this->content_starting_point + this->terminal_content_height * this->screen_width);
        this->footer_size  = (uint16_t)(this->footer_starting_point  + this->terminal_footer_height  * this->screen_width);

        this->header_current_x = 0;
        this->header_current_y = 0;
        this->status_current_x = 0;
        this->content_current_x = 0;
        this->content_current_y = 0;
        this->footer_current_x = 0;
        this->footer_current_y = 0;

        // 4 user terminals + 1 debug terminal.
        this->StartTerminalHeader();
        this->StartTerminalStatus();
        this->StartTerminal(1);
        this->StartTerminal(2);
        this->StartTerminal(3);
        this->StartTerminal(4);
        this->StartTerminal(5); // Debug
        this->StartTerminalFooter();
        this->UpdateScreen();

        shiro->cursor.enable();
        shiro->cursor.moveTo(7, (this->screen_height - 1));

        // Initial header — drawn by RenderHeader() which paints title +
        // tabs + clock placeholder, all in the right colours.
        this->RenderHeader();
        this->RenderStatus("booting...");

        // Note: Keyboard::Start() was already invoked by Shiro::Start() — no
        // need to repeat it here.

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
        System::Shiro *shiro = System::Shiro::GetInstance();

        // We deliberately do NOT call RenderHeader() here. Re-painting the
        // entire header on every screen refresh would clobber the cells
        // that background tasks (spinner, heartbeat, clock) write into
        // columns 64..79. The header is repainted explicitly when the
        // active terminal changes (see Update() below) and once at boot.

        // Header (row 0)
        for (size_t pos = this->header_starting_point; pos < this->header_size; pos++)
            this->screen[pos] = this->terminal_screen_header[pos];

        // Status bar (row 1)
        for (size_t pos = this->status_starting_point; pos < this->status_size; pos++)
            this->screen[pos] = this->terminal_screen_status[pos];

        // Content (rows 2..23)
        for (size_t pos = this->content_starting_point; pos < this->content_size; pos++)
            this->screen[pos] = this->terminal_screens[this->current_terminal][pos];

        // Footer / prompt (row 24)
        for (size_t pos = this->footer_starting_point; pos < this->footer_size; pos++)
            this->screen[pos] = this->terminal_screen_footer[pos];

        // Place text cursor right after the typed prompt buffer.
        size_t size = System::Drivers::Keyboard::GetBufferSize();
        shiro->cursor.moveTo((size + 7), (this->screen_height - 1));
    }

    /**
     * Applications::Shell.RenderHeader()
     *
     * Repaints the entire header row in place. Layout (left → right):
     *
     *   "Shiro 0.5  | [1*] [2] [3] [4] [5⚙]                  ♥ /   HH:MM:SS"
     *
     * Active-tab highlight comes from header_color_tab_active; inactive
     * tabs from header_color_tab_inactive. The clock area is left blank
     * here — a background task overwrites it via WriteSAt / direct
     * buffer access, so we don't have to chase a moving target.
     */
    void RenderHeader()
    {
        // Fill columns 0..63 with header background. We deliberately leave
        // columns 64..79 alone — those are owned by the heartbeat /
        // spinner / clock tasks. They run asynchronously and we don't
        // want to blink them out every time the active tab changes.
        for (size_t pos = this->header_starting_point;
             pos < this->header_starting_point + 64; pos++)
            this->terminal_screen_header[pos] =
                System::VGA::setColor(' ', this->header_color);

        // Write " Shiro 0.5 " at the very left.
        const char *title = " Shiro 0.5  ";
        size_t col = 0;
        for (size_t i = 0; title[i] != '\0'; i++)
            this->terminal_screen_header[col++] =
                System::VGA::setColor(title[i], this->header_color);

        // Vertical separator.
        this->terminal_screen_header[col++] =
            System::VGA::setColor((char)0xB3, this->header_color); // │

        col++; // spacer

        // Tabs: [1] [2] [3] [4] [5dbg].
        for (uint16_t tab = 1; tab <= 5; tab++)
        {
            uint8_t color = (tab == this->current_terminal)
                ? this->header_color_tab_active
                : this->header_color_tab_inactive;

            // Open bracket
            this->terminal_screen_header[col++] =
                System::VGA::setColor('[', color);
            // Tab number
            char num = (char)('0' + tab);
            this->terminal_screen_header[col++] =
                System::VGA::setColor(num, color);
            // Tab label: "term" or "dbg"
            const char *label = (tab == 5) ? " dbg" : " term";
            for (size_t i = 0; label[i] != '\0'; i++)
                this->terminal_screen_header[col++] =
                    System::VGA::setColor(label[i], color);
            // Close bracket
            this->terminal_screen_header[col++] =
                System::VGA::setColor(']', color);
            // Spacer between tabs
            this->terminal_screen_header[col++] =
                System::VGA::setColor(' ', this->header_color);
        }

        // The right-hand side (spinner / heart / clock) is owned by the
        // background animation tasks; they write to columns 65..79
        // directly. We leave those cells blank here.
    }

    /**
     * Applications::Shell.RenderStatus(text)
     *
     * Paints the status bar row (row 1) with `text`, padded on the right
     * with spaces so the entire row uses the status colour scheme.
     * Truncated to 80 chars.
     */
    void RenderStatus(const char *text)
    {
        // Background fill.
        for (size_t pos = this->status_starting_point; pos < this->status_size; pos++)
            this->terminal_screen_status[pos] =
                System::VGA::setColor(' ', this->status_color);

        // Place text starting at column 1 (one-cell left padding).
        size_t col = this->status_starting_point + 1;
        size_t limit = this->status_size - 1;
        for (size_t i = 0; text[i] != '\0' && col < limit; i++, col++)
            this->terminal_screen_status[col] =
                System::VGA::setColor(text[i], this->status_color);
    }

    /**
     * Applications::Shell.WriteSAt(text, x)
     *
     * Writes `text` at column x of the status bar without disturbing the
     * rest. Used by header animation tasks that aren't writing the whole
     * status bar.
     */
    void WriteSAt(const char *text, size_t x)
    {
        size_t col = this->status_starting_point + x;
        size_t limit = this->status_size;
        for (size_t i = 0; text[i] != '\0' && col < limit; i++, col++)
            this->terminal_screen_status[col] =
                System::VGA::setColor(text[i], this->status_color);
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
            // Change current terminal — repaint the header so the active
            // tab highlight follows.
            this->current_terminal = function_key;
            this->RenderHeader();

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

            // Copy the buffer into a local string with the trailing '\n'
            // stripped, so the command parser sees a clean token.
            char command[51];
            size_t end = keyboardBufferSize;
            if (end > 0 && keyboardBuffer[end - 1] == '\n')
                end--;
            if (end > 50)
                end = 50;
            for (size_t i = 0; i < end; i++)
                command[i] = keyboardBuffer[i];
            command[end] = '\0';

            // Echo the typed command into the scrollback in the echo
            // colour (yellow), prefixed with the prompt so the user can
            // see what they ran.
            this->WriteCEcho(concat("Shiro> ", command), false);

            // Reset buffer before executing — the command may produce more
            // output and we don't want stale input lingering.
            System::Drivers::Keyboard::BufferReset();

            // Dispatch.
            this->Execute(command);

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
     * Applications::Shell.Execute(const char* command)
     *
     * Hands the parsed command line off to the Applications::Commands
     * dispatcher. Defined out-of-line at the bottom of this header so the
     * full Commands class definition is visible.
     */
    void Execute(const char *command);

    /**
     * Applications::Shell.ClearTerminal(uint16_t terminal_id)
     *
     * Wipes a terminal's content buffer and resets the write cursor.
     */
    void ClearTerminal(uint16_t terminal_id)
    {
        // Re-initialize the terminal's buffer to spaces with the default
        // background colour.
        size_t size = this->content_starting_point +
                      (this->terminal_content_height * this->terminal_content_width);
        for (size_t pos = this->content_starting_point; pos < size; pos++)
        {
            this->terminal_screens[terminal_id][pos] =
                System::VGA::setColorContent(' ', this->terminal_color);
        }
        this->content_current_x = 0;
        this->content_current_y = 0;
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
     */
    void StartTerminalFooter()
    {
        size_t size = this->footer_starting_point +
                      (this->terminal_footer_height * this->terminal_footer_width);

        for (size_t pos = this->footer_starting_point; pos < size; pos++)
            this->terminal_screen_footer[pos] =
                System::VGA::setColor(' ', this->footer_color);

        this->footer_current_x = 0;
        this->WriteF("Shiro> ");
    }

    /**
     * Applications::Shell.StartTerminalStatus()
     *
     * Wipes the status bar buffer to a fresh "ready" line. Live updates
     * happen through RenderStatus / WriteSAt.
     */
    void StartTerminalStatus()
    {
        log("> Initializing Status Bar");
        for (size_t pos = this->status_starting_point; pos < this->status_size; pos++)
            this->terminal_screen_status[pos] =
                System::VGA::setColor(' ', this->status_color);
        this->status_current_x = 0;
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
        // Always write into the LAST row of the content area, then Scroll
        // shifts everything up. So the "current row" is always
        // (footer_starting_point - one row), regardless of how many rows
        // the content area actually has.
        size_t last_line_starting_point =
            (size_t)this->footer_starting_point - this->screen_width;
        size_t limit = this->content_size - 2;

        WriteCColored(data, this->terminal_color, last_line_starting_point, limit);

        this->Scroll();

        if (update)
            this->UpdateScreen();
    }

    /**
     * Applications::Shell.WriteCEcho(data)
     *
     * Same as WriteC but renders the line in the "echo" color (yellow on
     * black) so the user can spot which lines were commands they typed
     * versus output from the kernel.
     */
    void WriteCEcho(const char *data, bool update = true)
    {
        size_t last_line_starting_point =
            (size_t)this->footer_starting_point - this->screen_width;
        size_t limit = this->content_size - 2;

        WriteCColored(data, this->terminal_color_echo, last_line_starting_point, limit);

        this->Scroll();

        if (update)
            this->UpdateScreen();
    }

    /**
     * Internal: write `data` into the last content row using the supplied
     * VGA color. Newlines force a Scroll mid-string. Shared by WriteC and
     * WriteCEcho so the color is the only difference between them.
     */
    void WriteCColored(const char *data, uint8_t color,
                       size_t last_line_starting_point, size_t limit)
    {
        size_t gi = 0;
        for (size_t i = 0; i < strlen(data); i++)
        {
            if (data[i] == '\n')
            {
                this->Scroll();
                continue;
            }
            if (gi >= limit)
                this->content_current_x = 0;
            gi = last_line_starting_point + this->content_current_x;
            this->terminal_screens[this->current_terminal][gi] =
                System::VGA::setColorContent(data[i], color);
            this->content_current_x++;
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
        // Compute content row range from absolute layout, not hard-coded
        // numbers. With the status bar inserted, content rows are 2..23
        // (positions 160..1840), so we shift rows from first_row+1 down
        // to first_row, sweeping upward.
        size_t first_row = (size_t)this->content_starting_point / this->screen_width; // 2
        size_t last_row  = (size_t)this->footer_starting_point  / this->screen_width - 1; // 23

        for (size_t row = first_row; row < last_row; row++)
        {
            for (size_t col = 0; col < this->screen_width; col++)
            {
                this->terminal_screens[this->current_terminal][row * this->screen_width + col] =
                    this->terminal_screens[this->current_terminal][(row + 1) * this->screen_width + col];
            }
        }

        // Clear the freshly-vacated last row so the next WriteC starts blank.
        for (size_t col = 0; col < this->screen_width; col++)
        {
            this->terminal_screens[this->current_terminal][last_row * this->screen_width + col] =
                System::VGA::setColorContent(' ', this->terminal_color);
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

// Now that Shell is fully defined, we can pull in Commands (which calls
// into Shell) and provide the out-of-line Shell::Execute body.
#include "commands.class.h"

inline void Applications::Shell::Execute(const char *command)
{
    if (command == 0 || command[0] == '\0') return;
    Applications::Commands::Dispatch(command);
}

#endif