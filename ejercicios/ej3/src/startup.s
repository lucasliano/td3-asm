.code 32                    @ Arranca el código en modo ARM (32b)
/*--- Variables globales --- */
.global _start              @ Indica al linker donde arranca el codigo

/*--- Variables importadas --- */
.extern memcopy
.extern preKernelInit
.extern _main

/*--- Definiciones --- */
.include "defines_asm.h"

/*--- Arranca la sección --- */
.section .start_code
_start:
    @ Inicializo el SP para el modo actual
    LDR SP, = _SVC_STACK_TOP

    @ Copio el Exception Vector a su posición
    LDR R0, = _PUBLIC_GIC_VECTOR_LMA_INIT
    LDR R1, = _PUBLIC_EXCEPTION_VECTOR 
    LDR R2, = _PUBLIC_GIC_SIZE
    @ LDR R10, = memcopy
    @ BLX R10
    BLX memcopy

    @ Inicialización del Stack Pointer de cada modo. Dejamos deshabilitadas las interrupciones.
    
    @ NOTE: Ver que solamente se están alterando los bits [7:0] por usar el _c.
    @       Esto se puede ver mejor en la sección B9.3.11 del ARM Architecture Reference Manual.
    @ --- FIQ ---
    MSR cpsr_c, (1 << I_BIT) | (1 << F_BIT) | (0 << T_BIT) | (FIQ_MODE)
    LDR SP, =   _FIQ_STACK_TOP

    @ --- IRQ ---
    MSR cpsr_c, (1 << I_BIT) | (1 << F_BIT) | (0 << T_BIT) | (IRQ_MODE)
    LDR SP, =   _IRQ_STACK_TOP

    @ --- SVC ---
    MSR cpsr_c, (1 << I_BIT) | (1 << F_BIT) | (0 << T_BIT) | (SVC_MODE)
    LDR SP, =   _SVC_STACK_TOP

    @ --- ABT ---
    MSR cpsr_c, (1 << I_BIT) | (1 << F_BIT) | (0 << T_BIT) | (ABT_MODE)
    LDR SP, =   _ABT_STACK_TOP

    @ --- UND ---
    MSR cpsr_c, (1 << I_BIT) | (1 << F_BIT) | (0 << T_BIT) | (UND_MODE)
    LDR SP, =   _UND_STACK_TOP

    @ --- SYS ---
    MSR cpsr_c, (1 << I_BIT) | (1 << F_BIT) | (0 << T_BIT) | (SYS_MODE)
    LDR SP, =   _SYS_STACK_TOP
    @ Nos quedamos en el sys corriendo

    @Inicializo la placa
    @ LDR R9, preKernelInit
    @ MOV LR, PC
    @ BX  R9
    @ BLX preKernelInit


    @ Inicio copio el kernel a la RAM
    LDR R0, = _PUBLIC_KERNEL_LMA_INIT   @ Origen
    LDR R1, = _PUBLIC_RAM_INIT          @ Destino
    LDR R2, = _PUBLIC_KERNEL_SIZE       @ Cant Bytes
    @ LDR R10, = memcopy                  @ Cargamos donde queremos saltar
    @ MOV LR, PC                          @ Cargamos donde debemos retornar
    @ BX  R10                             @ Hacemos el salto
    BLX memcopy

    B _main

.end
