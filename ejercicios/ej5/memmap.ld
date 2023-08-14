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
_GIC_INIT = 0x00000000;  /* Hardwired con 0x7000_0000 */
_ROM_INIT = 0x70010000;  /* En AM335x: ROM @ 0x40000000 & Size = 176kB (0x2c000) */
_RAM_INIT = 0x70030000;  /* Voy a suponer que tenemos Size = 1GB (0x4000_0000) */
MEMORY
{
    gic      : org = _GIC_INIT, len = 0x1000
    rom      : org = _ROM_INIT, len = 0x20000 /* 0x2c000 */
    ram      : org = _RAM_INIT, len = 0x20000 /* 0x40000000 */
}


/* --- Distribución de la RAM --- */
/* >>> GIC Exception Vector <<< */
_GIC_PHY = 0x70000000;  /* Hace falta ponerlo en la 0x7000_0000 para no tener error por saltos muy grandes en los handlers */

/* >>> ROM (w/ init code) <<< */
/* Se declaró arriba 0x70010000 */

/* >>> Kernel Code (.handlers + .text) <<< */
_KERNEL_CODE_PHY = _RAM_INIT; /* 0x70030000 */

/* --- NOTE: De acá en adelante es dinámico el asunto --- */
/* >>> Stack TOPS <<< */
_KERNEL_STACKS_PHY = 0x70F00000;
_STACK_SIZE = 0x2aaa; /* 10kiB por modo 60kiB (0xfffc) en total */

/* >>> Page Tables <<< */
/* _SYSTABLES_PHY se define abajo */
_KERNEL_MAX_TASKS = 16;
_SYSTABLES_PAGE_SIZE = 0x1000;  /* 4KiB */
_SYSTABLES_MAX_L2 = 48; /* Cantidad de tablas L2 por tarea */
_SYSTABLES_PHY_SIZE = 0x4000 + (_SYSTABLES_MAX_L2 * 0x400); /* 64KiB (0x10000) = 16KiB + (48 * 1KiB) */

/* >>> Task memory region <<< */
_TASKS_VMA = 0x80000000;
_TASKS_PHY = 0x71000000;

_TASKS_PHY_SIZE = 0x00100000;   /* Definimos un tamaño máximo para cada tarea */

_TASK_MAX_PAGES = _SYSTABLES_MAX_L2 * 256; /* Cada tarea puede tener como máximo 4 tablas L2 (contando identity mapping) */
_TASK_MAX_ALLOCABLE_MEMORY_SIZE = _TASK_MAX_PAGES * _SYSTABLES_PAGE_SIZE; /* 0x100_0000 = _TASK_MAX_PAGES * _SYSTABLES_PAGE_SIZE


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
    .init _ROM_INIT : 
    {
        *(.reset_vector_code*)
        *(.start_code*)
    } > rom
    _rom_end = .;

    /* Esto es el código que va en el GIC. */
    _GIC_VECTOR_LMA_INIT = LOADADDR(.init) + SIZEOF(.init);
    .gic_vector _GIC_PHY : AT(_GIC_VECTOR_LMA_INIT)
    {
        *(.gic_vector*)
    } /* > gic NOTE: si bien está en gic, como tenemos el hardwire me tira error */
    _GIC_SIZE = SIZEOF(.gic_vector);

    /* Esto es el código que va en el KERNEL. Tiene VMA != LMA */
    _KERNEL_LMA_INIT = LOADADDR(.gic_vector) + SIZEOF(.gic_vector);
    .kernel _KERNEL_CODE_PHY : AT (_KERNEL_LMA_INIT) 
    {
        *(.mydebug*)       /* TODO: Remove */
        *(.handlers*)
        *(.text*)
    } > ram
    _KERNEL_CODE_SIZE = SIZEOF(.kernel);

    .data : {               /*Datos*/
        . = ALIGN(4);       /*Salta a la proxima direccion alineada a 4*/
        _DATA_START = .;
        *(.data*)
        _DATA_END = .;
    } > ram

    .bss : {                /*Variables no inicializadas*/
        . = ALIGN(4);
        _BSS_START = .;
        *(.bss*)
        _BSS_END = .;
    } > ram 

    /* Kernel Stack before tasks */
    .kernel_stacks _KERNEL_STACKS_PHY :
    {
        /* Empty */
    }
    _KERNEL_STACKS_END = _KERNEL_STACKS_PHY + (6 * _STACK_SIZE);


    /* Región dedicada a las tareas - 0x7100_0000*/
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

