#include "../inc/gic.h"
#include "../inc/timer.h"
#include "../inc/mmu_tools.h"
#include "../inc/low_level_cpu_access.h"

void preKernelInit();
void configureGIC0();
void configureTIMER0();

extern int memcopy(const void *origen, void *destino, unsigned int num_bytes);
extern int _SYSTABLES_PHYSICAL;
extern int _PUBLIC_KERNEL_LMA_INIT;
extern int _PUBLIC_RAM_INIT;
extern int _PUBLIC_KERNEL_SIZE;
extern int _PUBLIC_STACK_INIT;
extern int _SYS_STACK_TOP;
extern int _SYSTABLES_VMA;
extern int _PUBLIC_EXCEPTION_VECTOR;
extern int _PUBLIC_GIC_SIZE;


__attribute__((section(".text"))) void _irq_disable(){
	asm("mrs r1, cpsr");
	asm("orr r1, r1, #0x80");
	asm("msr cpsr , r1");
}

__attribute__((section(".text"))) void _irq_enable(){
	asm("mrs r1, cpsr");
	asm("bic r1, r1, #0x80");
	asm("msr cpsr , r1");
}

__attribute__((section(".text"))) void preKernelInit(){
    // Variables para configuración de MMU
    TTBCR ttbcr;
    TTBR0 ttbr0;
    SCTLR sctlr;
    DACR dacr;
    uint32_t vbar, i;

    uint32_t pa;
    uint32_t sp;

    // Esta lectura, limpia las variables
    ttbcr = MMU_Get_TTBCR();
    sctlr = MMU_Get_SCTLR();
    ttbr0 = MMU_Get_TTBR0();
    dacr = MMU_Get_DACR();
    vbar = MMU_Get_VBAR();

    //------------------------------------------------------------------------------//
    // Inicializamos el GIC y las interrupciones deseadas
    //------------------------------------------------------------------------------//
    configureGIC0();

    //------------------------------------------------------------------------------//
    // Inicializamos y configuramos el Timer
    //------------------------------------------------------------------------------//
    configureTIMER0();

    //------------------------------------------------------------------------------//
    // Copiamos nuestro código a las zonas de memoria requeridas
    //------------------------------------------------------------------------------//
    memcopy((const void *) &_PUBLIC_KERNEL_LMA_INIT, (void *) &_PUBLIC_RAM_INIT, (unsigned int) &_PUBLIC_KERNEL_SIZE);

    
    //------------------------------------------------------------------------------//
    // Configuramos nuestro mapa de memoria
    //------------------------------------------------------------------------------//
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

    // Invalidamos la TLB, según documentación
    MMU_Invalidate_TLB();


    // --- Mapa de memoria del Kernel ---
    MMU_Set_FirstLevelTranslationTable_PhysicalAddress((uint32_t)&_SYSTABLES_PHYSICAL);

    // --- Código del Kernel ---
    for (i = (uint32_t)&_PUBLIC_RAM_INIT; i < (uint32_t)&_PUBLIC_RAM_INIT + (uint32_t)&_PUBLIC_KERNEL_SIZE; i+= 0x1000)
    {
        MMU_MapNewPage(i, i, _4K, XN_ALLOWEXECUTION, _NORMAL, NONCACHEABLE, _SBIT, PL1);
    }

    // --- STACK ---
    for (i = (uint32_t)&_PUBLIC_STACK_INIT; i < (uint32_t)&_SYS_STACK_TOP; i+= 0x1000)
    {
        MMU_MapNewPage(i, i, _4K, XN_ALLOWEXECUTION, _NORMAL, NONCACHEABLE, _SBIT, PL1);
    }

    // --- Sys tables VMA ---
    for (i = (uint32_t)& _SYSTABLES_VMA; i < (uint32_t)&_SYSTABLES_VMA + 0x500000; i+= 0x1000)
    {
        MMU_MapNewPage(i, i, _4K, XN_ALLOWEXECUTION, _NORMAL, NONCACHEABLE, _SBIT, PL1);
    }

    // --- Registros del procesador ---
    for (i = (uint32_t)& _PUBLIC_EXCEPTION_VECTOR; i < (uint32_t)&_PUBLIC_EXCEPTION_VECTOR + (uint32_t)&_PUBLIC_GIC_SIZE; i+= 0x1000)
    {
        MMU_MapNewPage(i, i, _4K, XN_ALLOWEXECUTION, _NORMAL, NONCACHEABLE, _SBIT, PL1);
    }

    // --- Registros del procesador - Mirror ---
    // for (i = (uint32_t)& _PUBLIC_EXCEPTION_VECTOR; i < (uint32_t)&_PUBLIC_EXCEPTION_VECTOR + (uint32_t)&_PUBLIC_KERNEL_SIZE; i+= 0x1000)
    // {
    //     MMU_MapNewPage(i, i, _4K, XN_ALLOWEXECUTION, _NORMAL, NONCACHEABLE, _SBIT, PL1);
    // }

    //------------------------------------------------------------------------------//
    // Activamos la MMU
    //------------------------------------------------------------------------------//
    MMU_Invalidate_TLB();
    asm("MCR p15, 0, r0, c1, c0, 0");
    asm("orr r0, r0, #1");
    asm("MCR p15, 0, r0, c1, c0, 0");
    asm("DSB");
    MMU_Invalidate_TLB();

    //------------------------------------------------------------------------------//
    // Habilitamos las interrupciones
    //------------------------------------------------------------------------------//
    _irq_enable();
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

