/**
* Archivo: asm_utils.S
* Función: Funciones en ASM para facilitar la vida.
* Autores: Lucas Liaño
**/

.code 32                @ Arranca el código en modo ARM (32b)
/*--- Variables globales --- */
.global _irq_enable
.global _irq_disable
.global _WRITE_8
.global _WRITE_16
.global _WRITE_32
.global _READ_8
.global _READ_16
.global _READ_32
.global _WRITE_SP
.global _READ_SP

/*--- Variables importadas --- */

/*--- Definiciones --- */

/*--- Arranca la sección --- */
.section .text, "a"

@ Habilita las IRQ.
.align 4             // Alineado a 4 Bytes
_irq_enable:
    DSB
    CPSIE i
    @ @ Esto es un código equivalente:
    @ MRS r1, cpsr
    @ BIC r1, r1, #0x80
    @ MSR cpsr , r1
    DSB
    ISB
    BX LR

@ Deshabilita las IRQ.
.align 4             // Alineado a 4 Bytes
_irq_disable:
    DSB
    CPSID i
    @ @ Esto es un código equivalente:
    @ MRS r1, cpsr
    @ ORR r1, r1, #0x80
    @ MSR cpsr , r1
    DSB
    ISB
    BX LR

@ Habilita las FIQ.
.align 4             // Alineado a 4 Bytes
_fiq_enable:
    DSB
    CPSIE f
    @ @ Esto es un código equivalente:
    @ MRS r1, cpsr
    @ BIC r1, r1, #0x40
    @ MSR cpsr , r1
    DSB
    ISB
    BX LR

@ Deshabilita las FIQ.
.align 4             // Alineado a 4 Bytes
_fiq_disable:
    DSB
    CPSID f
    @ @ Esto es un código equivalente:
    @ MRS r1, cpsr
    @ ORR r1, r1, #0x40
    @ MSR cpsr , r1
    DSB
    ISB
    BX LR


@ >> Sacado del código de Lucho <<
@ Funciones de escritura y lecutra de 8, 16 y 32 bits
@ Para las funciones _WRITE_X, desde C es: _WRITE_X(DIRECCION, VALOR) 
.align 4             // Alineado a 4 Bytes
_WRITE_8:
    STRB R1, [R0]       // STR Byte el contenido de R0 en la posición R1
    BX LR

_WRITE_16:
    STRH R1, [R0]       // STR HalfWord el contenido de R0 en la posición R1
    BX LR

_WRITE_32:
    STR R1, [R0]       // STR Word el contenido de R0 en la posición R1
    BX LR

_READ_8:
    LDRB R0, [R0]
    BX LR

_READ_16:
    LDRH R0, [R0]
    BX LR

_READ_32:
    LDR R0, [R0]
    BX LR

_READ_SP:
    MOV R0, SP
    BX LR

_WRITE_SP:
    MOV SP, R0
    BX LR