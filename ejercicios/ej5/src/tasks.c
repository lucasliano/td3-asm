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


__attribute__((section(".task1"), aligned(4), naked)) void task1(void){
    /* ------------------------- TAREA 1 ------------------------------------
    * Escribe desde 0x90000000 a 0x9000FFFF la palabra 0x11AA22BB en
    * cada posición. Al finalizar la escritura, verifico que el 
    * contenido sea el correcto.
    * 
    * Al finalizar, vacío la memoria y vuelvo a hacer al escritura-lectura.
    * 
    * Nota: Propongo la word 0x11AA22BB porque es más simple de debuggear 
    * desde el gdb-dashboard que printea con otro endianess.
    * ---------------------------------------------------------------------- */

    asm("MOV R0, #1");      // Solo para identificar la tarea 1 en el debugger.


    uint32_t* ptrRam  = (uint32_t*) 0x90000000;


    while(1){

        *ptrRam = 0x11AA22BB;       // copio la palabra a ram
        if((*ptrRam) == 0x11AA22BB) // chequeo que se copie bien
            ptrRam++;  

        if(ptrRam >= (uint32_t*)0x9000FFFF)
            ptrRam = (uint32_t*)0x90000000;            
    }
}


// __attribute__((section(".task2"), aligned(4), naked)) void task2(void){
//     /* ------------------------- TAREA 2 ------------------------------------
//     * Invierto todos los bits de la memoria. Se recorre desde la posición 
//     * 0x70A10000 hasta la posición 0x70A1FFFF.
//     * ---------------------------------------------------------------------- */
    
//     asm("MOV R0, #2");      // Solo para identificar la tarea 1 en el debugger.


//     uint32_t* ptrRam = (uint32_t*)0x70A10000;
    
//     while(1){
        
//         *ptrRam = ~(*ptrRam);
//         ptrRam++;

//         if(ptrRam >= (uint32_t*)0x70A1FFFF)
//             ptrRam = (uint32_t*)0x70A10000;
//     }
// }


// __attribute__((section(".task1")))  __attribute__((naked)) void task1(){

//     while(1)
//     {
//         asm("MOV R0, #1");
//         asm("WFI");
//         contador1++;
//         asm("PUSH {R0}");
//     }
// }


__attribute__((section(".task2"))) __attribute__((naked)) void task2(){

    while(1)
    {
        asm("MOV R0, #2");
        asm("WFI");
        contador2++;
        asm("PUSH {R0,R1}");
    }
}