.code 32
/*--- Variables globales --- */
.global _main

/*--- Variables importadas --- */


/*--- Arranca la sección --- */
.section .kernel, "a"  @ El "a" es para que sea alocable, es decir para que se pueda tener una VMA distinta a la LMA.
_main:
    @ MOV R1, #0
    @ MOV R1, #1
    @ SWI #0
    @ MOV R1, #2
    @ MOV R1, #3
    @ MOV R1, #4

    WFI @ Pone al procesador en modo bajo consumo hasta que se detecte un wake-up event.
    
.end
