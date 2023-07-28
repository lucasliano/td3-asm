.code 32
/*--- Variables globales --- */
.global _main

/*--- Variables importadas --- */


/*--- Arranca la sección --- */
.section .kernel, "a"  @ El "a" es para que sea alocable, es decir para que se pueda tener una VMA distinta a la LMA.
_main:
    LDR R0, =.
    WFI @ Pone al procesador en modo bajo consumo hasta que se detecte un wake-up event.
    BX  R0

.end
