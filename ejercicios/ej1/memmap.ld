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
    . = _PUBLIC_RAM_INIT; /*El . es el location counter, será donde arranca el programa */
    .text : {/*Salida, ene ste ejemplo se llama igual a la entrada*/
        /*El * es un comomdín, será todo lo que comience con .text
          La idea es que todos las secciones esten una debajo de otra. 
          Sino se pusiese el * la pondría en otro lado y desde ese punto hasta 
          la dirección de ram no se sabría cuanto espacio hay entonces el binario puede 
          terminar siendo muy pesado
        */
        *(.text*) /*Tiene que coincidir con la seccion en el código para que sirva de entrada*/
    } > public_ram

    .data : {*(.data*)} > public_ram /*Ensambla todas las secciones data en una seccion data de salida*/

    .bss : { /*Variables no inicializadas*/
        . = ALIGN(4); /*Salta a la proxima direccion alineada a 4*/
        __bss_start__ = .;
        *(.bss*)
        __bss_end__ = .;
    } > public_ram 
}

