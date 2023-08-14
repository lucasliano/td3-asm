.code 32                    @ Arranca el código en modo ARM (32b)
/*--- Variables globales --- */
.global _start              @ Indica al linker donde arranca el codigo

/*--- Variables importadas --- */
.extern rom_memcopy
.extern hardwareInit
.extern _KERNEL_STACKS_PHY

/*--- Definiciones --- */
.include "defines_asm.h"

/*--- Arranca la sección --- */
.section .start_code
_start:
    @ >>> Código seguro que corre en ROM <<<
    @ Inicializo el SP para operar momentaneamente en SYS
    LDR SP, = _KERNEL_STACKS_PHY    /* Dirección aleatoria en RAM */

    @ Copio el Exception Vector a su posición
    LDR R0, = _GIC_VECTOR_LMA_INIT
    LDR R1, = _GIC_PHY 
    LDR R2, = _GIC_SIZE
    BL rom_memcopy

    @ Inicio copio el kernel a la RAM
    LDR R0, = _KERNEL_LMA_INIT   @ Origen
    LDR R1, = _KERNEL_CODE_PHY          @ Destino
    LDR R2, = _KERNEL_CODE_SIZE  @ Cant Bytes
    BL rom_memcopy
    @ >>> Hasta acá el código no puede tener excepciones <<<
    @ >>> De ahora en adelante el código corre en RAM <<<

    @Inicializo los stacks
    BL hardwareInit

.end
