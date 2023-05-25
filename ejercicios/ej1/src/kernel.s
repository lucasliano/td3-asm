.code 32
/*--- Variables globales --- */
.global _main

/*--- Variables importadas --- */


/*--- Arranca la sección --- */
.section .kernel, "a"  @ El "a" es para que sea alocable, es decir para que se pueda tener una VMA distinta a la LMA.
_main:
    LDR R0, =.
    ADD R1, R1, #1
    BX  R0

.end
