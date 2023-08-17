#include "../inc/tasks.h"


// FIXME: Esto está en .bss que está en 0x7003_0000
uint32_t contador1 = 0;
uint32_t contador2 = 0;

__attribute__((section(".task0"))) __attribute__((naked)) void kernelIdle(void)
{   
    // Esta es una tarea que debería ejecutar algúna tarea de housekeeping
    
    // NOTE: Esta línea genera un data abort
    // uint32_t myvar = *((uint32_t*) 0x80000000); 

    // NOTE: Esta línea genera un prefetch abort
    // asm("B 0x80000000");    


    while(1)
    {
        asm("MOV R0, #0");
        asm("WFI");
    }

    return; // Should never get here
}

__attribute__((section(".task1")))  __attribute__((naked)) void task1(){

    while(1)
    {
        asm("MOV R0, #1");
        asm("WFI");
        contador1++;
        asm("PUSH {R0}");
    }
}


__attribute__((section(".task2"))) __attribute__((naked)) void task2(){

    while(1)
    {
        asm("MOV R0, #2");
        asm("WFI");
        contador2++;
        asm("PUSH {R0,R1}");
    }
}