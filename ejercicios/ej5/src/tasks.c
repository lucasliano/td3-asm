#include "../inc/tasks.h"


// FIXME: Esto está en .bss que está en 0x7003_0000
uint32_t contador1 = 0;
uint32_t contador2 = 0;
uint32_t contador3 = 0;

// IRQ_Handler_c(void)
// {
//     asm("SUB LR, LR, #4");
//     asm("SUB SP, SP, #60");
//     asm("STMIA SP, {R0-R12, SP, LR}"); // Salvo contexto

//     // Ahora, podemos hacer lo mismo que en el handler de assembler o
//     // directamente trabajar acá identificando que IRQ fue, y demás.
//     // Listo - Volvemos a dónde fue interrumpido el uP

//     asm("LDMIA SP, {R0-R12, SP, LR}^");
//     asm("ADD SP, SP, #60");
//     asm("MOVS PC, LR");
// }

__attribute__((section(".task1")))  __attribute__((naked)) void task1(){
    
    while(1)
    {
        asm("WFI");
        contador1++;
    }
}


__attribute__((section(".task2"))) __attribute__((naked)) void task2(){

    while(1)
    {
        asm("WFI");
        contador2++;
    }
}

__attribute__((section(".task3"))) __attribute__((naked)) void task3(){

    while(1)
    {
        asm("WFI");
        contador3++;
    }
}