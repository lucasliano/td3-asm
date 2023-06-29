.code 32
/*--- Variables globales --- */
.global _UND_handler
.global _SVC_handler
.global _PREF_handler
.global _ABT_handler
.global _IRQ_handler

/*--- Variables importadas --- */
.extern undef_kernel_handler
.extern swi_kernel_handler
.extern abort_kernel_handler
.extern irq_kernel_handler

/*--- Definiciones --- */
.include "defines_asm.h"

/*--- Arranca la sección --- */
.section .handlers, "a"
_UND_handler: 
    B .

_SVC_handler: 
    SUB   LR, LR, #4
    NOP


    @ MOV R1, #0
    @ MOV R0, pc     @ Esto sería la posición del ADD, teniendo en cuenta que el PC esta en +8 siempre.
    @ NOP            @ Esto es un NOP para que el calculo anterior me de siempre bien.
    @ ADD R1, R1, #1
    @ BX  R0
    @ .word 0xEFBEADDE @ Es 0xDEADBEEF en Little Endian (nuestra arquitectura)

_PREF_handler: 
    B .

_ABT_handler: 
    B .


    @ LDR R5, =0xDEADBEEF  @ Carga la dirección 0xdeadbeef en el registro r0, es una dir inválida
    @ LDR R6, [R5]         @ Con [] intentara acceder a la dirección en r0 (generará Data Abort)

_IRQ_handler:
    @ @ Deshabilito las IRQ y las FIQ hasta terminar de atender esta interrupción
    @ MSR cpsr_c, (1 << I_BIT) | (1 << F_BIT) | (0 << T_BIT) | (IRQ_MODE)        

    @ @ Corrijo la posición del link register.
    @ SUB   LR, LR, #4            @ Esto es porque tengo que volver a la instrucción que sigue a la de mi PC. 
    @                             @ O sea el PC esta en +8 por el pipeline, y quiero ir a una más, entonces resto 4.

    @ @ Algun comportamiento que necesito tener entre llamadas de IRQs:
    @ ADD   R10, R10, #1          @ R10 va a contiene la cantidad de veces que se entro en IRQ.
    

    @ @ Salvamos registros importantes:
    @ STMFD SP!, { R0-R12, LR }   @ Guarda de R0 a R12 y LR en la pila, usando SP como base y decrementando automáticamente su valor.
    @                             @ PUSH es equivalente a esta instrucción.
    
    @ MOV   R7, SP                @ Se copia el contenido de SP a R7.
    @ MRS   R8, SPSR              @ Se copia el contenido de SPSR a R8. Usamos MRS porque SPSR es un registro del sistema.
    @ PUSH {R7, R8}               @ Se guardan en la pila el valor de R7 y R8.

    @ @ Branch al handler del kernel en C:
    @ MOV   R0, SP                @ Copio el SP en R0. NOTE: Preguntar porque necesitamos al SP en R0.
    @ BL    irq_kernel_handler    @ Llamo al handler de irq.

    @ POP  {R7, R8}              @ Guardo en R7 y R8 los valores del SPSR y SP.
    @ MOV   SP, R7               @ Pongo el contenido de R7 (SP anterior) al SP actual.
    @ LDMFD SP!, { R0-R12, PC }^ @ Carga de R0 a R12 y el PC desde la pila utilizando SP como base. El ^ hace que se cargue el CPSR desde el SPSR directamente.
    @ @ MSR cpsr_c, 0x50         @ Habilito nuevamente las IRQ .



    sub   lr, lr, #4
    stmfd sp!, { r0-r12, lr } 
    mov   r7, SP
    mrs   r8, spsr
    push {r7, r8}

    mov   r0, sp
    bl    irq_kernel_handler

    pop {r7, r8}
    mov sp, r7
    ldmfd sp!, { r0-r12, pc }^

.end
