#include "../inc/kernel.h"

extern TCB_t taskVector[16];

__attribute__((section(".handlers"))) __attribute__((interrupt("undef"))) void undef_kernel_handler(){

}

__attribute__((section(".handlers"))) __attribute__((interrupt("swi"))) void swi_kernel_handler(){

}

__attribute__((section(".handlers"))) __attribute__((interrupt("abort"))) void pref_abort_kernel_handler(){

}

__attribute__((section(".handlers"))) __attribute__((interrupt("abort"))) void data_abort_kernel_handler(){

}

// __attribute__((interrupt("irq"))) <- Esto no funciona (pone mal el PC con el LR).

// TODO: Ver que chota pasa acá con el naked.. Ver que tiene los "" así que puede ser por eso.
//       El problema que estoy teniendo es que a pesar del cambio de contexto, el chabon recuepra
//       el stack pointer a partir del link register.


__attribute__((section(".handlers"))) __attribute__((naked)) void irq_kernel_handler(){
    _gicc_t* const GICC0 = (_gicc_t*)GICC0_ADDR;
    _timer_t* const TIMER0 = (_timer_t*)TIMER0_ADDR;

    // static uint32_t timeframeCounter = 0;
    // static uint32_t currentTaskCounter = 0;

    uint32_t id = GICC0->IAR;

    
    // NOTE: Es una parte muy delicada del código porque como hay un posible cambio de contexto
    //       no podemos jugar con los registros libremente.

    // debug();

    switch(id){
        case 36: // Timer interrupt
            GICC0->EOIR = id;           //Esto da por finalizada la interrupción (WFI funca)
            TIMER0->Timer1IntClr = 0x1; //Al escribir cualquier valor en el registro se limpia la interrupcion del contador

            
            // Necesito salvar el LR antes de cambiar de contexto
            asm("MOV R2, LR");
            // TCB2TTBR0(0);                // Setteo la TTBR0
            _TCB2Stacks(&taskVector[0]); // Cargamos los SP
            asm("MOV LR, R2");

            break;

        default:
            break;
    } 
    
    // debug();

    asm("BX LR");
} 

__attribute__((section(".handlers"))) __attribute__((interrupt("fiq"))) void fiq_kernel_handler(){

}