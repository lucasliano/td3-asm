.code 32                    @ Arranca el código en modo ARM (32b)
.global _start              @ Indica al linker donde arranca el codigo
.section .start_code

_start:
    B .

    LDR SP, = _PUBLIC_STACK_INIT

    LDR R0, = _PUBLIC_RAM_INIT      @ Origen
    LDR R1, = _PUBLIC_CPY_INIT      @ Destino
    MOV R2, #0x1000                 @ Cant Bytes

    LDR R10, = memcopy              @ Cargamos donde queremos saltar
    MOV LR, PC                      @ Cargamos donde debemos retornar
    BX  R10                         @ Hacemos el salto

    B _end


_end:
    B .



.end