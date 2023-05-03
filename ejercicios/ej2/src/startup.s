.code 32                    @ Arranca el código en modo ARM (32b)
.global _start              @ Indica al linker donde arranca el codigo


@ Se indica que las variables estan definidas afuera, aunque no hace nada el extern
.extern memcopy
.extern _main

@ Arranca la sección
.section .start_code
_start:
    LDR SP, = _PUBLIC_STACK_INIT

    LDR R0, = _PUBLIC_ROM_INIT      @ Origen
    LDR R1, = _PUBLIC_RAM_INIT      @ Destino
    MOV R2, #0x1000                 @ Cant Bytes

    LDR R10, = memcopy              @ Cargamos donde queremos saltar
    MOV LR, PC                      @ Cargamos donde debemos retornar
    BX  R10                         @ Hacemos el salto

    B _main

.end
