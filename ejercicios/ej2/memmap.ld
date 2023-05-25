/*
    Definiciones necesarias: formato de salida, arquitectura y  punto de entrada
*/ 
OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_reset_vector)


/*
    Definiciones de simbolos necesarios
*/
_PUBLIC_EXCEPTION_VECTOR = 0x70000000;
_PUBLIC_ROM_INIT = 0x70010000;
_PUBLIC_STACK_INIT = 0x70020000;
_PUBLIC_RAM_INIT = 0x70030000;


/*
    Definiciones del mapa de memoria
*/
MEMORY
{
    public_gic      : org = _PUBLIC_EXCEPTION_VECTOR, len = 0x1000
    public_rom      : org = _PUBLIC_ROM_INIT, len = 0x1000
    public_stack    : org = _PUBLIC_STACK_INIT, len = 0x1000
    public_ram      : org = _PUBLIC_RAM_INIT, len = 0x1000
}

/*
    Definicion de las secciones
*/
SECTIONS{
    . = _PUBLIC_ROM_INIT;
    .text : 
    {
        *(.reset_vector_code*)
        *(.start_code*)
        *(.text*)
    } > public_rom

    _PUBLIC_GIC_VECTOR_LMA_INIT = LOADADDR(.text) + SIZEOF(.text);
    .gic_vector _PUBLIC_EXCEPTION_VECTOR : AT(_PUBLIC_GIC_VECTOR_LMA_INIT)
    {
        *(.gic_vector*)
    } > public_gic
    _PUBLIC_GIC_SIZE = SIZEOF(.gic_vector);

    _PUBLIC_KERNEL_LMA_INIT = LOADADDR(.gic_vector) + SIZEOF(.gic_vector);
    .kernel _PUBLIC_RAM_INIT : AT (_PUBLIC_KERNEL_LMA_INIT) 
    {
        *(.handlers*)
        *(.kernel_test*)
    } > public_ram
    _PUBLIC_KERNEL_SIZE = SIZEOF(.kernel);


    .bss : {                /*Variables no inicializadas*/
        . = ALIGN(4);       /*Salta a la proxima direccion alineada a 4*/
        __bss_start__ = .;
        *(.bss*)
        __bss_end__ = .;
    } > public_ram 
}

