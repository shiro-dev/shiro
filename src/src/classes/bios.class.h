#ifndef BIOS_CLASS_H
#define BIOS_CLASS_H

/**
 * struct regs16_t
 */
typedef struct __attribute__ ((packed)) {
    /**
     * Not usable yet
     **/
    unsigned short ah, bh, ch, dh;
    unsigned short al, bl, cl, dl;
    // ---
    unsigned short di, si, bp, sp, bx, dx, cx, ax;
    unsigned short gs, fs, es, ds, eflags;
} regs16_t;

/**
 * int32(unsigned char intnum, regs16_t *regs)
 *
 * This is an external function. 
 * You can find it's code on real-mode-switcher.asm.
 * This function will send data to BIOS interrupts.
 */
extern "C" void int32(unsigned char intnum, regs16_t *regs);

/**
 * namespace System
 *
 * This is our core namespace.
 * All system-related classes can be found on this namespace.
 */
namespace System 
{
    /**
     * System::Bios()
     *
     * This class is responsible for all interactions with the BIOS.
     * Basic Usage:
     *  System::Bios bios;
     *  bios.regs.ax = 0x00A;
     *  bios.Send2Interrupt(0x10);
     */
    class Bios
    {
        public:

            /**
             * regs16_t regs;
             * 
             * This parameter contains our struct template.
             * Usage:
             *  bios.regs.ax = 0x00A;
             * 
             * @var regs16_t 
             */
            regs16_t regs;

            /**
             * System::Bios.Send2Interrupt(unsigned char intnum)
             *
             * This method will send data to BIOS interrupts.
             * 
             * @return void
             */
            void Send2Interrupt(unsigned char intnum)
            {
                int32(intnum, &this->regs);
            }

            /**
             * System::Bios.Send2Interrupt(unsigned char intnum, bool clean_after)
             *
             * This method will send data to BIOS interrupts.
             * If the clean_after flag it TRUE: It will clean all regs after sending it to the BIOS.
             * If the clean_after flag it FALSE: The regs are going to be kept after we send then to the BIOS.
             * 
             * @return void
             */
            void Send2Interrupt(unsigned char intnum, bool clean_after)
            {
                int32(intnum, &this->regs);
                if(clean_after == true){
                    this->Clean();
                }
            }

            /**
             * System::Bios.Clean()
             *
             * This method is responsible for cleaning all regs after sending then to the BIOS.
             * 
             * @return void
             */
            void Clean()
            {
                this->regs.ah = 0x0000;
                this->regs.bh = 0x0000;
                this->regs.ch = 0x0000;
                this->regs.dh = 0x0000;
                this->regs.al = 0x0000;
                this->regs.bl = 0x0000;
                this->regs.cl = 0x0000;
                this->regs.dl = 0x0000;
                this->regs.di = 0x0000;
                this->regs.si = 0x0000;
                this->regs.bp = 0x0000;
                this->regs.sp = 0x0000;
                this->regs.bx = 0x0000;
                this->regs.dx = 0x0000;
                this->regs.cx = 0x0000;
                this->regs.ax = 0x0000;
                this->regs.gs = 0x0000;
                this->regs.fs = 0x0000;
                this->regs.es = 0x0000;
                this->regs.ds = 0x0000;
                this->regs.eflags = 0x0000;
            }
    };
}

#endif