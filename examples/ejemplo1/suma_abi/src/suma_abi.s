.extern suma_c
.extern mult_c
@ .extern _div_c
.extern _PUBLIC_STACK_INIT

.global _start @Indica donde arranca el codigo

.code 32
.section .text
_start:
    LDR SP, = _PUBLIC_STACK_INIT

    MOV R0, #0x10
    MOV R1, #0x20

    /*B suma_c*/

    LDR R10, = suma_c
    MOV LR, PC
    BX R10 @Branch con posibilidad de cambio de ARM a thumb

    LDR R10, = mult_c
    MOV LR, PC
    BX R10

    B .
.end
