.code 32
/*--- Variables globales --- */
.global _exception_vector_start

/*--- Variables importadas --- */
.extern _reset_vector

/*--- Arranca la sección --- */
.section .handlers, "a"
_exception_vector_start:
    .word 0xDEADBEEF            @ NOTE: Buscar en bless como EFBEADDE


    LDR pc, =_reset_vector_irq  @ Le tenes que pasar una instrucción, porque el procesador espera eso
    LDR pc, =_segundo_irq       @ Va a ser un literal pool. Hay que ver el disassembly.
    B .


_reset_vector_irq:
    .word _reset_vector_handler

_segundo_irq:
    .word _segundo_handler



_reset_vector_handler:
    B =_reset_vector        @ Si sacamos el = tira error 'relocation truncated to fit: R_ARM_JUMP24 against symbol `_reset_vector''

_segundo_handler:
    B .

.end
