#include "../inc/gic.h"
#include "../inc/timer.h"

void undef_kernel_handler();
void swi_kernel_handler();
void abort_kernel_handler();
void irq_kernel_handler();
void fiq_kernel_handler();

__attribute__((section(".kernel"))) __attribute__((interrupt("undef"))) void undef_kernel_handler(){

}

__attribute__((section(".kernel"))) __attribute__((interrupt("swi"))) void swi_kernel_handler(){

}

__attribute__((section(".kernel"))) __attribute__((interrupt("abort"))) void pref_abort_kernel_handler(){

}

__attribute__((section(".kernel"))) __attribute__((interrupt("abort"))) void data_abort_kernel_handler(){

}

// __attribute__((interrupt("irq"))) <- Esto no funciona (pone mal el PC con el LR).
__attribute__((section(".kernel"))) __attribute__((interrupt("naked"))) void irq_kernel_handler(){
    _gicc_t* const GICC0 = (_gicc_t*)GICC0_ADDR;
    _timer_t* const TIMER0 = (_timer_t*)TIMER0_ADDR;

    uint32_t id = GICC0->IAR;

    switch(id){
        case 36:
            GICC0->EOIR = id;           //Esto da por finalizada la interrupción (WFI funca)
            TIMER0->Timer1IntClr = 0x1; //Al escribir cualquier valor en el registro se limpia la interrupcion del contador
            break;

        default:
            break;
    } 
    
    return;
} 

__attribute__((section(".kernel"))) __attribute__((interrupt("fiq"))) void fiq_kernel_handler(){

}