.code 32                    @ Arranca el código en modo ARM (32b)
/*--- Variables globales --- */
.global _start              @ Indica al linker donde arranca el codigo

/*--- Variables importadas --- */
.extern memcopy
.extern _main

/*--- Arranca la sección --- */
.section .start_code
_start:
    LDR SP, = _PUBLIC_STACK_INIT

    @ Inicio copio el kernel a la RAM
    LDR R0, = _PUBLIC_KERNEL_LMA_INIT   @ Origen
    LDR R1, = _PUBLIC_RAM_INIT          @ Destino
    MOV R2, #0x1000                     @ Cant Bytes

    LDR R10, = memcopy                  @ Cargamos donde queremos saltar
    MOV LR, PC                          @ Cargamos donde debemos retornar
    BX  R10                             @ Hacemos el salto

    @ Copio el Exception Vector a su posición
    LDR R0, = _PUBLIC_HANDLERS_LMA_INIT
    LDR R1, = _PUBLIC_EXCEPTION_VECTOR 
    MOV R2, #0x1C
    LDR R10, = memcopy
    MOV LR, PC
    BX  R10


    B _main

.end
