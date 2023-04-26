/*
    Definiciones necesarias: formato de salida, arquitectura y  punto de entrada
*/ 
OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)


/*
    Definiciones de simbolos necesarios
*/
_PUBLIC_RAM_INIT = 0x70010000;
_PUBLIC_STACK_INIT = 0x70020000;
_PUBLIC_CPY_INIT = 0x70030000;

/*
    Definiciones del mapa de memoria
*/
MEMORY
{
    public_ram      : org = _PUBLIC_RAM_INIT, len = 0x1000
    public_stack    : org = _PUBLIC_STACK_INIT, len = 0x1000
    public_cpy      : org = _PUBLIC_CPY_INIT, len = 0x1000
}

/*
    Definicion de las secciones
*/
SECTIONS{
    . = _PUBLIC_RAM_INIT;
    .text : {

        *(.reset_vector_code*)
        *(.start_code*)

    } > public_ram

    .bss : {                /*Variables no inicializadas*/
        . = ALIGN(4);       /*Salta a la proxima direccion alineada a 4*/
        __bss_start__ = .;
        *(.bss*)
        __bss_end__ = .;
    } > public_ram 
}

