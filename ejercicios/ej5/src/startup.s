.code 32                    @ Arranca el código en modo ARM (32b)
/*--- Variables globales --- */
.global _start              @ Indica al linker donde arranca el codigo

/*--- Variables importadas --- */
.extern memcopy
.extern preKernelInit

/*--- Definiciones --- */
.include "defines_asm.h"

/*--- Arranca la sección --- */
.section .start_code
_start:
    @ >>> Código seguro que corre en ROM <<<
    @ Inicializo el SP para el modo actual
    LDR SP, = _SVC_STACK_TOP

    @ Copio el Exception Vector a su posición
    LDR R0, = _PUBLIC_GIC_VECTOR_LMA_INIT
    LDR R1, = _PUBLIC_EXCEPTION_VECTOR 
    LDR R2, = _PUBLIC_GIC_SIZE
    BL memcopy

    @ Inicio copio el kernel a la RAM
    LDR R0, = _PUBLIC_KERNEL_LMA_INIT   @ Origen
    LDR R1, = _KERNEL_CODE_PHY          @ Destino
    LDR R2, = _PUBLIC_KERNEL_CODE_SIZE  @ Cant Bytes
    BL memcopy
    @ >>> Hasta acá el código no puede tener excepciones <<<
    @ >>> De ahora en adelante el código corre en RAM <<<

    @Inicializo los stacks
    BL preKernelInit

.end
