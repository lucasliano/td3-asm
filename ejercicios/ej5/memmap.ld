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
    ram      : org = _PUBLIC_RAM_INIT, len = 0x2000 /* 0x40000000 */
}


/* --- Distribución de la RAM --- */
/* >>> GIC Exception Vector <<< */
_PUBLIC_EXCEPTION_VECTOR = _PUBLIC_GIC;

/* >>> ROM (w/ init code) <<< */
/* Se declaró arriba 0x70010000 */

/* >>> Stack TOPS <<< */
/* TODO:    En realidad el stack va a ser distinto para cada tarea  */
/*          pero para poder trabajar vamos a necesitar algun stack  */
_PUBLIC_STACK_INIT = 0x70020000;
_PUBLIC_STACK_SIZE = 512; /* Termina en 0x700201FF */
_FIQ_STACK_TOP = _PUBLIC_STACK_INIT + _PUBLIC_STACK_SIZE; /* Ej: SP_FIQ = 0x70020100 */
_IRQ_STACK_TOP = _FIQ_STACK_TOP + _PUBLIC_STACK_SIZE;
_SVC_STACK_TOP = _IRQ_STACK_TOP + _PUBLIC_STACK_SIZE;
_ABT_STACK_TOP = _SVC_STACK_TOP + _PUBLIC_STACK_SIZE;
_UND_STACK_TOP = _ABT_STACK_TOP + _PUBLIC_STACK_SIZE;
_SYS_STACK_TOP = _UND_STACK_TOP + _PUBLIC_STACK_SIZE;

/* >>> Kernel Code (.handlers + .text) <<< */
_KERNEL_CODE_PHY = _PUBLIC_RAM_INIT; /* 0x70030000 */

/* NOTE: De acá en adelante es dinámico el asunto */

/* >>> Page Tables <<< */
/* _SYSTABLES_PHY se define abajo */
_KERNEL_MAX_TASKS = 16;
_SYSTABLES_PAGE_SIZE = 0x1000;  /* 4KiB */
_SYSTABLES_MAX_L2 = 4; /* Cantidad de tablas L2 por tarea */

_SYSTABLES_PHY = 0x70050000;
/* _SYSTABLES_PHY_SIZE = 0x00010000; /* Solamente vamos a ocupar 64KiB = 16KiB + (48 * 1KiB) */
_SYSTABLES_PHY_SIZE = 0x1000 + (_SYSTABLES_MAX_L2 * 0x400); /* 20KiB (0x2000) = 16KiB + (4 * 1KiB) */

/* >>> Task memory region <<< */
_TASKS_VMA = 0x80000000;
_TASKS_PHY = 0x71000000;
_TASK_MAX_PAGES = _SYSTABLES_MAX_L2 * 256; /* Cada tarea puede tener como máximo 4 tablas L2 (contando identity mapping) */
_TASK_PHY_SIZE = _TASK_MAX_PAGES * _SYSTABLES_PAGE_SIZE; /* 0x100_0000 = _TASK_MAX_PAGES * _SYSTABLES_PAGE_SIZE


/*
    Definicion de las secciones
*/
SECTIONS{

    /* APB & AHB Hardware registers */
    _HARDWARE_REGISTERS_INIT = 0x10000000;
    .hardware _HARDWARE_REGISTERS_INIT :
    {
        /* Empty? */
    }
    _HARDWARE_REGISTERS_END = 0x20000000;


    /* Código en ROM */
    .init _PUBLIC_ROM_INIT : 
    {
        *(.reset_vector_code*)
        *(.start_code*)
    } > rom
    _rom_end = .;

    /* Esto es el código que va en el GIC. Tiene VMA != LMA */
    _PUBLIC_GIC_VECTOR_LMA_INIT = LOADADDR(.init) + SIZEOF(.init);
    .gic_vector _PUBLIC_EXCEPTION_VECTOR : AT(_PUBLIC_GIC_VECTOR_LMA_INIT)
    {
        *(.gic_vector*)
    } > gic
    _PUBLIC_GIC_SIZE = SIZEOF(.gic_vector);
    

    /* Acá estamos reservando espacio para el STACK, pero no ocupa código en LMA */
    .stack _PUBLIC_STACK_INIT :
    {
        /* Preguntamos si el stack size alcanza */
        ASSERT ((_SYS_STACK_TOP > (_rom_end + _PUBLIC_STACK_SIZE * 6)), "Error: No room left for the stack");
    } > ram /* FIXME: Ver que esto no está funcionando porque 0x70020000 no está en RAM */
    _PUBLIC_STACK_END = _SYS_STACK_TOP;

    /* Esto es el código que va en el KERNEL. Tiene VMA != LMA */
    _PUBLIC_KERNEL_LMA_INIT = LOADADDR(.gic_vector) + SIZEOF(.gic_vector);
    .kernel _KERNEL_CODE_PHY : AT (_PUBLIC_KERNEL_LMA_INIT) 
    {
        *(.debug*)       /* TODO: Remove */
        *(.handlers*)
        *(.text*)
        *(.kernel*)
    } > ram
    _PUBLIC_KERNEL_CODE_SIZE = SIZEOF(.kernel);

    /* Estas son las tablas de la MMU para el kernel - 0x7005_0000*/
    .systables _SYSTABLES_PHY :
    {
        /* Empty? */
    } > ram /* FIXME: Nos excedimos de lo que declaramos como RAM y no dice nada */
    _SYSTABLES_END = _SYSTABLES_PHY + _SYSTABLES_PHY_SIZE;

    .data : {               /*Datos*/
        . = ALIGN(4);       /*Salta a la proxima direccion alineada a 4*/
        __data_start__ = .;
        *(.data*)
        __data_end__ = .;
    } > ram

    .bss : {                /*Variables no inicializadas*/
        . = ALIGN(4);
        __bss_start__ = .;
        *(.bss*)
        __bss_end__ = .;
    } > ram 


    /* Región dedicada a las tareas - 0x7006_0000*/
    /* .tasks _TASKS_PHY :  */
    /* {  */
    /* /* Empty? */
    /* } > ram /* FIXME: Nos excedimos de lo que declaramos como RAM y no dice nada */
    /* _tasks_end = _TASKS_PHY + _TASK_PHY_SIZE;  */
    
    _TASK1_LMA = LOADADDR(.bss) + SIZEOF(.bss);
    .task1 0x80000000 : AT (_TASK1_LMA)
    { 
        *(.task1)
    }
    _task1_size = SIZEOF(.task1); 

    _TASK2_LMA = LOADADDR(.task1) + SIZEOF(.task1);
    .task2 0x80000000 : AT (_TASK2_LMA)
    { 
        *(.task2)
    }
    _task2_size = SIZEOF(.task2);  

    _TASK3_LMA = LOADADDR(.task2) + SIZEOF(.task2);
    .task3 0x80000000 : AT (_TASK3_LMA)
    { 
        *(.task3)
    }
    _task3_size = SIZEOF(.task3); 
    
}

