#include "../inc/kernel.h"

void kernel_main(void)
{   
    // NOTE: Esta línea genera un data abort
    // uint32_t myvar = *((uint32_t*) 0x80000000); 

    // NOTE: Esta línea genera un prefetch abort
    // asm("B 0x80000000");    


    while(1)
    {
        asm("WFI");
    }

    return; // Should never get here
}