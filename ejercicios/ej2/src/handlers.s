.code 32
.section .handlers

@ TODO: Hacer pruebas con un bootloader

_table_start:
    LDR pc, _primer_irq     @ Le tenes que pasar una instrucción, porque el procesador espera eso
    LDR pc, _segundo_irq    @ Va a ser un literal pool. Hay que ver el disassembly.
    B .


_primer_irq:
    .word _primer_handler

_segundo_irq:
    .word _segundo_handler



_primer_handler:
    B .

_segundo_handler:
    B .

.end
