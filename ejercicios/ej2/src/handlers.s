.code 32
/*--- Variables globales --- */
.global _UND_handler
.global _SVC_handler
.global _PREF_handler
.global _ABT_handler
.global _IRQ_handler

/*--- Variables importadas --- */

/*--- Arranca la sección --- */
.section .handlers, "a"
_UND_handler: 
    B .

_SVC_handler: 
    MOV R1, #0
    MOV R0, pc     @ Esto sería la posición del ADD, teniendo en cuenta que el PC esta en +8 siempre.
    NOP            @ Esto es un NOP para que el calculo anterior me de siempre bien.
    ADD R1, R1, #1
    BX  R0
    .word 0xEFBEADDE @ Es 0xDEADBEEF en Little Endian (nuestra arquitectura)

_PREF_handler: 
    B .

_ABT_handler: 
    B .

_IRQ_handler:
    B .


.end
