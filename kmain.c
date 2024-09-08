#include "io.h"
#include "multiboot.h"

#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))

void kmain (unsigned long magic, unsigned long addr)
{
    multiboot_info_t *mbi;

    /* Clear the screen. */
    cls ();

    /* Am I booted by a Multiboot-compliant boot loader? */
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
    {
        printf ("Invalid magic number: 0x%x\n", (unsigned) magic);
        return;
    }

    /* Set MBI to the address of the Multiboot information structure. */
    mbi = (multiboot_info_t *) addr;

    /* Print out the flags. */
    printf ("flags = 0x%x\n", (unsigned) mbi->flags);

    /* Are mem_* valid? */
    if (CHECK_FLAG (mbi->flags, 0))
        printf ("mem_lower = %uKB, mem_upper = %uKB\n",
                (unsigned) mbi->mem_lower, (unsigned) mbi->mem_upper);

    /* Is boot_device valid? */
    if (CHECK_FLAG (mbi->flags, 1))
        printf ("boot_device = 0x%x\n", (unsigned) mbi->boot_device);

    /* Is the command line passed? */
    if (CHECK_FLAG (mbi->flags, 2))
        printf ("cmdline = %s\n", (char *) mbi->cmdline);

    /* Are mods_* valid? */
    if (CHECK_FLAG (mbi->flags, 3))
    {
        multiboot_module_t *mod;
        unsigned int i;

        printf ("mods_count = %d, mods_addr = 0x%x\n",
                (int) mbi->mods_count, (int) mbi->mods_addr);
        for (i = 0, mod = (multiboot_module_t *) mbi->mods_addr;
                i < mbi->mods_count;
                i++, mod++)
            printf (" mod_start = 0x%x, mod_end = 0x%x, cmdline = %s\n",
                    (unsigned) mod->mod_start,
                    (unsigned) mod->mod_end,
                    (char *) mod->cmdline);

        unsigned int* modules = (unsigned int*)mbi->mods_addr; 

        if (mbi->mods_count > 0) {
            typedef void (*call_module_t)(void);
            unsigned int addr = modules[0]; 
            call_module_t start_program = (call_module_t)addr;
            printf("addr to start:0x%x\n", start_program);
            start_program();
        }
    }
}
