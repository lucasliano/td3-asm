.extern _PUBLIC_STACK_INIT  @ Posición del stack

.extern _PUBLIC_RAM_INIT    @ Posición comienzo de RAM
.extern _PUBLIC_CPY_INIT    @ Posición para copiar código
.extern td3_memcopy         @ Posición del código en C

.global _start              @ Indica donde arranca el codigo
.code 32                    @ .code directive marks the beginning of the code segment, where all executable statements in a program are located
.section .text
_start:
    LDR SP, = _PUBLIC_STACK_INIT

    LDR R0, = _PUBLIC_RAM_INIT      @ Origen
    LDR R1, = _PUBLIC_CPY_INIT      @ Destino
    MOV R2, #0x1000                 @ Cant Bytes

    LDR R10, = td3_memcopy          @ Cargamos donde queremos saltar
    MOV LR, PC                      @ Cargamos donde debemos retornar
    BX  R10                         @ Hacemos el salto

    B _end


_end:
    B .

.end