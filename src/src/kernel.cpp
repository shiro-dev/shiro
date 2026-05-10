#include <stdint.h>
#include <helper/debug.h>
#include <multiboot2.h>
#include "heap.class.h"
#include <new.h>
#include "pmm.class.h"
#include "initrd.class.h"
#include "shiro.class.h"
#include "shell.class.h"
#include "commands.class.h"
#include "tasks.class.h"

extern "C" uint8_t _kernel_start[];
extern "C" uint8_t _kernel_end[];

/**
 * loop()
 *
 * Main idle loop, executing as task 0 ("kernel") under the cooperative
 * scheduler. After every Shell tick we Yield() so background tasks
 * (spinner, heartbeat) can run, and HLT until the next interrupt to save
 * power.
 */
void loop()
{
    Applications::Shell *shell = Applications::Shell::GetInstance();
    while (true)
    {
        shell->Update();
        System::Scheduler::Yield();
        asm volatile("hlt");
    }
}

/**
 * shiro_main(magic, mbi)
 *
 * Shiro's entry point. boot.asm pushes EBX (multiboot info pointer) and
 * EAX (multiboot magic) on the stack just before calling us, in that
 * order, so our first arg is the magic and the second is the info pointer.
 */
extern "C" void shiro_main(uint32_t multiboot_magic, void *multiboot_info)
{
    // 1a. Pre-scan the multiboot info for any MODULE tag so we can put
    //     the heap *after* whatever GRUB loaded into memory. Without this
    //     the heap clobbers the initrd, since GRUB drops the module right
    //     after our BSS where _kernel_end points.
    uint32_t initrd_end = 0;
    if (multiboot_info != 0)
    {
        uint8_t *cur = (uint8_t *)multiboot_info + 8;
        for (;;)
        {
            mb2_tag *tag = (mb2_tag *)cur;
            if (tag->type == MB2_TAG_END) break;
            if (tag->type == MB2_TAG_MODULE)
            {
                uint32_t *u = (uint32_t *)tag;
                uint32_t mod_end = u[3];
                if (mod_end > initrd_end) initrd_end = mod_end;
            }
            uint32_t step = (tag->size + 7) & ~7u;
            cur += step;
        }
    }

    // 1b. Heap online — every singleton below uses `new`. Start it past
    //     any modules so we don't trash them on first allocation.
    System::Heap::Start(initrd_end);

    // 2. Physical Memory Manager — needs the multiboot mmap, and needs to
    //    know what address range our kernel + heap occupies so it doesn't
    //    hand those pages out.
    System::PMM::Start(multiboot_info,
                       (uint32_t)_kernel_start,
                       System::Heap::End());

    // 2.5. Initrd — find the multiboot2 MODULE tag and remember where
    //      our ramdisk (built by tools/mkinitrd.py) lives in memory.
    System::Initrd::Start(multiboot_info);

    // 3. Bring up Shiro and friends.
    System::Shiro *shiro = System::Shiro::GetInstance();
    shiro->multiboot.Start(multiboot_magic, multiboot_info);
    shiro->Start();
    shiro->Finish();

    Applications::Shell *shell = Applications::Shell::GetInstance();
    shell->Start();

    // 4. Run /init.sh from the initrd, if present. Each non-comment,
    //    non-empty line is fed through the same command dispatcher the
    //    interactive shell uses, so anything you can type at the prompt
    //    works as a startup directive.
    {
        System::Initrd::file_entry_t *e = System::Initrd::Find("init.sh");
        if (e != 0)
        {
            const uint8_t *data = System::Initrd::Data(e);
            char line[80];
            uint32_t li = 0;
            for (uint32_t i = 0; i < e->size; i++)
            {
                char b = (char)data[i];
                if (b == '\r') continue;
                if (b == '\n' || li >= 79)
                {
                    line[li] = '\0';
                    if (li > 0 && line[0] != '#')
                        Applications::Commands::Dispatch(line);
                    li = 0;
                    if (b != '\n') line[li++] = b;
                    continue;
                }
                line[li++] = b;
            }
            if (li > 0)
            {
                line[li] = '\0';
                if (line[0] != '#')
                    Applications::Commands::Dispatch(line);
            }
        }
    }

    // 5. Spawn background tasks: spinner + heartbeat for visual feedback,
    //    clock and statusbar for live system metrics in the header / row 1.
    System::Scheduler::Spawn(System::DemoTasks::task_spinner,   "spinner");
    System::Scheduler::Spawn(System::DemoTasks::task_heartbeat, "heartbeat");
    System::Scheduler::Spawn(System::DemoTasks::task_clock,     "clock");
    System::Scheduler::Spawn(System::DemoTasks::task_statusbar, "statusbar");

    loop();
}
