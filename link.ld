ENTRY(loader)


SECTIONS {
    . = 0x00100000;

    .text : 
    {
        *(.text)
    } 

    .rodata :
    {
        *(.rodata*) /* all read-only data sections from all files */
    } 
    
    .data :
    {
        *(.data) /* all data sections from all files */
    } 
    
    .bss :
    {
        *(COMMON) /* all COMMON sections from all files */
        *(.bss) /* all bss sections from all files */ 
    }
}
