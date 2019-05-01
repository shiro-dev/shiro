#ifndef CONSOLE_H
#define CONSOLE_H

size_t console_row;
size_t console_column;
uint8_t console_colour;
uint16_t* console_buffer;

void console_initialize(){
    console_row = 0;
    console_column = 0;
    console_colour = vga_entry_color(VGA_COLOUR_WHITE, VGA_COLOUR_BLUE);
    console_buffer = (uint16_t*) 0xB8000;
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            console_buffer[index] = vga_entry(' ', console_colour);
        }
    }
}

void console_scroll_up(){
    int unsigned i;
    for(i = 0; i < (VGA_WIDTH*VGA_HEIGHT-80); i++)
        console_buffer[i] = console_buffer[i+80];
    for(i = 0; i < VGA_WIDTH; i++)
        console_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + i] = vga_entry(' ', console_colour);
    
} 
 
void console_putcharat(char c, uint8_t color, size_t x, size_t y){
    const size_t index = y * VGA_WIDTH + x;
    console_buffer[index] = vga_entry(c, color);
}
 
void console_putchar(char c){

    if(console_column == VGA_WIDTH || c == '\n'){
        console_column = 0;
        if(console_row == VGA_HEIGHT-1){
            console_scroll_up();
        } else {
            console_row++;
        }
    }
    if(c == '\n') return;
    console_putcharat(c, console_colour, console_column++, console_row);
}
 
void console_write(const char* data, size_t size){

    for (size_t i = 0; i < size; i++)
        console_putchar(data[i]);
}
 
void console_writestring(const char* data){

    console_write(data, strlen(data));
}
// <newstuff>

void console_writestring_colour(const char* data, enum vga_colour fg, enum vga_colour bg){
    uint8_t oldcolour = console_colour;
    console_colour = vga_entry_color(fg, bg);
    console_writestring(data);
    console_colour = oldcolour;
}

void console_writeint(unsigned long n){
    if(n/10)
        console_writeint(n/10);
    console_putchar((n % 10) + '0');
}

#endif