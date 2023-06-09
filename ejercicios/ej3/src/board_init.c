#include "../inc/gic.h"
#include "../inc/timer.h"

void preKernelInit();
void configureGIC0();
void configureTIMER0();

__attribute__((section(".text"))) void preKernelInit(){
    configureGIC0();
    configureTIMER0();
    
    return;
}


/*Configuracion del GIC*/
__attribute__((section(".text"))) void configureGIC0() {
    //TODO: hacer la funcion generica para todos los GIC
    _gicc_t* const GICC0 = (_gicc_t*)GICC0_ADDR;
    _gicd_t* const GICD0 = (_gicd_t*)GICD0_ADDR;

    GICC0->PMR  = 0x000000F0; //Aplico máscara a las interrupciones de prioridad 0xF, las que tengan prioridar mayor (0x0 a 0xE) no quedan enmascaradas
    GICC0->CTLR = 0x00000001; //Habilito la interfaz de CPU para el GIC0

    GICD0->CTLR          = 0x00000001; //Activa la interrupciones para el GIC0
    GICD0->ISENABLER[1] |= 0x00000010; //GIC_SOURCE_TIMER0 // Habilita las interrupciones del TIMER0
    GICD0->ISENABLER[1] |= 0x00000002; //GIC_SOURCE_SWI // Habilita las interrupciones por SW
}

/*Configuracion del timer*/
__attribute__((section(".text"))) void configureTIMER0() {
    //TODO: hacer la funcion generica para todos los TIMER
    _timer_t* const TIMER0 = (_timer_t*)TIMER0_ADDR;

    TIMER0->Timer1Load     = 0x00010000; //Le cargo la cantidad de ticks al timer (65536)
    TIMER0->Timer1Ctrl     = 0x00000002; //Modo 32 bit
    TIMER0->Timer1Ctrl    |= 0x00000040; //Modo periódico (cuando termina llega a 0 vuelve a cargar el mismo valor)
    TIMER0->Timer1Ctrl    |= 0x00000020; //Habilita la interrupción por timer
    TIMER0->Timer1Ctrl    |= 0x00000080; //Activa el timer
}

