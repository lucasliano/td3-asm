#include "../inc/board_init.h"


__attribute__((section(".text"))) void preKernelInit(){
    //------------------------------------------------------------------------------//
    // En esta función se van a ejecutar todas las configuraciones correspondientes
    // al hardware.
    //------------------------------------------------------------------------------//  

    //------------------------------------------------------------------------------//
    // Configuración del hardware
    //------------------------------------------------------------------------------//    
    
    // Inicializamos el GIC y las interrupciones deseadas
    configureGIC0();

    // Inicializamos y configuramos el Timer
    configureTIMER0();

    // Configuramos los registros asociados a la MMU (sin activarla)
    configureMMU();
    
    //------------------------------------------------------------------------------//
    // Cedemos control al kernel
    //------------------------------------------------------------------------------//
    kernelInit();
}

__attribute__((section(".text"))) void configureMMU(){
    // Variables para configuración de MMU
    TTBCR ttbcr;
    SCTLR sctlr;
    DACR dacr;

    // Esta lectura, limpia las variables
    ttbcr = MMU_Get_TTBCR();
    sctlr = MMU_Get_SCTLR();
    dacr = MMU_Get_DACR();

    // Configuramos el Domain Manager Control Register - Por ahora, todo en Manager domain:
    dacr.dacr = 0xFFFFFFFF;
    MMU_Set_DACR(dacr);

    // Configuramos el TTBCR. Usamos la descripción como "short descriptor" por ende, si bien la estructura
    // dice T0SZ, por compatiblidad con el modo largo, esos mismos bits representan a N en el modo corto. Así
    // lo ponemos en cero para que TTBR0 pueda acceder a toda la memoria direccionable disponible (4 GB)
    ttbcr.T0SZ = 0;
    MMU_Set_TTBCR(ttbcr);

    // Configuramos el SCTLR
    sctlr.TRE = 0;
    sctlr.C = 0;
    sctlr.I = 0;
    sctlr.Z = 0;
    sctlr.AFE = AF_NO;
    MMU_Set_SCTLR(sctlr);
}

__attribute__((section(".text"))) void configureGIC0() {
    //------------------------------------------------------------------------------//
    // Configuracion del GIC
    //------------------------------------------------------------------------------//
    
    //TODO: hacer la funcion generica para todos los GIC
    _gicc_t* const GICC0 = (_gicc_t*)GICC0_ADDR;
    _gicd_t* const GICD0 = (_gicd_t*)GICD0_ADDR;

    GICC0->PMR  = 0x000000F0; //Aplico máscara a las interrupciones de prioridad 0xF, las que tengan prioridar mayor (0x0 a 0xE) no quedan enmascaradas
    GICC0->CTLR = 0x00000001; //Habilito la interfaz de CPU para el GIC0

    GICD0->CTLR          = 0x00000001; //Activa la interrupciones para el GIC0
    GICD0->ISENABLER[1] |= 0x00000010; //GIC_SOURCE_TIMER0 // Habilita las interrupciones del TIMER0
    GICD0->ISENABLER[1] |= 0x00000002; //GIC_SOURCE_SWI // Habilita las interrupciones por SW
}

__attribute__((section(".text"))) void configureTIMER0() {
    //------------------------------------------------------------------------------//
    // Configuracion del timer
    //------------------------------------------------------------------------------//
    
    //TODO: hacer la funcion generica para todos los TIMER
    _timer_t* const TIMER0 = (_timer_t*)TIMER0_ADDR;

    TIMER0->Timer1Load     = 0x00010000; //Le cargo la cantidad de ticks al timer (65536)
    TIMER0->Timer1Ctrl     = 0x00000002; //Modo 32 bit
    TIMER0->Timer1Ctrl    |= 0x00000040; //Modo periódico (cuando termina llega a 0 vuelve a cargar el mismo valor)
    TIMER0->Timer1Ctrl    |= 0x00000020; //Habilita la interrupción por timer
    TIMER0->Timer1Ctrl    |= 0x00000080; //Activa el timer
}
