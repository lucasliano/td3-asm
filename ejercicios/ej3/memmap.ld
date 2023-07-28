/*
    Definiciones necesarias: 
        - Formato de salida
        - Arquitectura
        - Punto de entrada
*/ 
OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_reset_vector)


/* --- Memorias Reales --- */
_PUBLIC_GIC      = 0x70000000;
_PUBLIC_ROM_INIT = 0x70010000;  /* En AM335x: ROM @ 0x40000000 & Size = 176kB (0x2c000) */
_PUBLIC_RAM_INIT = 0x70030000;  /* Voy a suponer que tenemos Size = 1GB (0x4000_0000) */
MEMORY
{
    gic      : org = _PUBLIC_GIC, len = 0x1000
    rom      : org = _PUBLIC_ROM_INIT, len = 0x1000 /* 0x2c000 */
    ram      : org = _PUBLIC_RAM_INIT, len = 0x1000 /* 0x40000000 */
}


/* --- Distribución de la RAM --- */
/* GIC Exception Vector */
_PUBLIC_EXCEPTION_VECTOR = _PUBLIC_GIC;

/* ROM (w/ init code) */
/* Se declaró arriba 0x70010000 */

/* Stack TOPS */
/* TODO:    En realidad el stack va a ser distinto para cada tarea  */
/*          pero para poder trabajar vamos a necesitar algun stack  */
_PUBLIC_STACK_INIT = 0x70020000;
_PUBLIC_STACK_SIZE = 256; /* Termina en 0x700200FF */
_FIQ_STACK_TOP = _PUBLIC_STACK_INIT + _PUBLIC_STACK_SIZE; /* Ej: SP_FIQ = 0x70020100 */
_IRQ_STACK_TOP = _FIQ_STACK_TOP + _PUBLIC_STACK_SIZE;
_SVC_STACK_TOP = _IRQ_STACK_TOP + _PUBLIC_STACK_SIZE;
_ABT_STACK_TOP = _SVC_STACK_TOP + _PUBLIC_STACK_SIZE;
_UND_STACK_TOP = _ABT_STACK_TOP + _PUBLIC_STACK_SIZE;
_SYS_STACK_TOP = _UND_STACK_TOP + _PUBLIC_STACK_SIZE;

/* Kernel Code (.handlers + .text) */
_KERNEL_CODE_PHY = _PUBLIC_RAM_INIT; /* 0x70030000 */



/*
    Definicion de las secciones
*/
SECTIONS{
    . = _PUBLIC_ROM_INIT;
    .init : 
    {
        *(.reset_vector_code*)
        *(.start_code*)
    } > rom

    _PUBLIC_GIC_VECTOR_LMA_INIT = LOADADDR(.init) + SIZEOF(.init);
    .gic_vector _PUBLIC_EXCEPTION_VECTOR : AT(_PUBLIC_GIC_VECTOR_LMA_INIT)
    {
        *(.gic_vector*)
    } > gic
    _PUBLIC_GIC_SIZE = SIZEOF(.gic_vector);

    _PUBLIC_KERNEL_LMA_INIT = LOADADDR(.gic_vector) + SIZEOF(.gic_vector);
    .kernel _KERNEL_CODE_PHY : AT (_PUBLIC_KERNEL_LMA_INIT) 
    {
        *(.handlers*)
        *(.text*)
        *(.kernel*)
    } > ram
    _PUBLIC_KERNEL_CODE_SIZE = SIZEOF(.kernel);


    .bss : {                /*Variables no inicializadas*/
        . = ALIGN(4);       /*Salta a la proxima direccion alineada a 4*/
        __bss_start__ = .;
        *(.bss*)
        __bss_end__ = .;
    } > ram 
}

