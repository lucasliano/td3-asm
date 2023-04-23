.extern _PUBLIC_STACK_INIT  @ Posición del stack
.extern td3_memcopy         @ Posición del código en C

.global _start              @ Indica donde arranca el codigo

.code 32                    @ TODO: Consultar que hace esto
.section .text
_start:
    LDR SP, = _PUBLIC_STACK_INIT

    MOV R0, #0x10
    MOV R1, #0x20

    /*B suma_c*/

    LDR R10, = suma_c
    MOV LR, PC
    BX R10 @ Branch con posibilidad de cambio de ARM a thumb

    LDR R10, = mult_c
    MOV LR, PC
    BX R10

    B .


end:
    B .


.end