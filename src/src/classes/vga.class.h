#ifndef VGA_CLASS_H
#define VGA_CLASS_H

/**
 * namespace System
 *
 * This is our core namespace.
 * All system-related classes can be found on this namespace.
 */
namespace System 
{
    class VGA
    {
        public:
            
            enum colors {
                VGA_COLOUR_BLACK,
                VGA_COLOUR_BLUE,
                VGA_COLOUR_GREEN,
                VGA_COLOUR_CYAN,
                VGA_COLOUR_RED,
                VGA_COLOUR_MAGENTA,
                VGA_COLOUR_BROWN,
                VGA_COLOUR_LIGHT_GREY,
                VGA_COLOUR_DARK_GREY,
                VGA_COLOUR_LIGHT_BLUE,
                VGA_COLOUR_LIGHT_GREEN,
                VGA_COLOUR_LIGHT_CYAN,
                VGA_COLOUR_LIGHT_RED,
                VGA_COLOUR_LIGHT_MAGENTA,
                VGA_COLOUR_LIGHT_BROWN,
                VGA_COLOUR_WHITE,
            };

            static uint8_t selectColors(enum System::VGA::colors foreground, enum System::VGA::colors background){
                return foreground | (background << 4);
            };

            static uint16_t setColor(unsigned char content, uint8_t color){
                return (uint16_t) content | (uint16_t) color << 8;
            };
            
            void Start(auto& shiro)
            {
                // Log
                log("> Starting VGA");
                shiro.NotImplemented();
            }

            
    };
}

#endif