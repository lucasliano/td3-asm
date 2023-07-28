.code 32
/*--- Variables globales --- */
.global _exception_vector_start

/*--- Variables importadas --- */
.extern _RESET_handler
.extern _UND_handler
.extern _SVC_handler
.extern _PREF_handler
.extern _ABT_handler
.extern _IRQ_handler
.extern fiq_kernel_handler

/*--- Arranca la sección --- */
.section .gic_vector, "a"
_exception_vector_start:
    @ .word 0xDEADBEEF            @ NOTE: Buscar en bless como EFBEADDE


    @ NOTE: Ver notas respecto de este código en el obsidian de la clase del 10-05.
    @       Estos son literal pools implicitos.
    @ LDR pc, =_reset_vector
    @ LDR pc, =_UND_handler 
    @ LDR pc, =_SVC_handler 
    @ LDR pc, =_PREF_handler 
    @ LDR pc, =_ABT_handler 
    @ NOP
    @ LDR pc, =_IRQ_handler


    LDR pc, _RESET_vector  @ Le tenes que pasar una instrucción, porque el procesador espera eso
    LDR pc, _UND_vector  
    LDR pc, _SVC_vector  
    LDR pc, _PREF_vector 
    LDR pc, _ABT_vector  
    NOP
    LDR pc, _IRQ_vector  
    @ --- Starts FIQ Handler to avoid breaking the pipeline ---
_FIQ_handler:
    SUB   LR, LR, #4            //Guardo en LR la posición actual - 4, que será la dirección de retorno luego de resolver la interrupción
    STMFD SP!, { R0-R12, LR }   //Guarda de R0 a R12 y LR en la pila, usando SP como base y decrementando automáticamente su valor.
    @ FIXME: Ver si vamos a soportar llamadas anidadas
    MOV   R7, SP                //Se copia el contenido de SP a R7
    MRS   R8, SPSR              //Se copia el contenido de SPSR a R8
    PUSH {R7, R8}               //Se guardan en la pila el valor de R7 y R8

    BL    fiq_kernel_handler    //Llamo al handler de fiq

    POP  {R7, R8}               //Guardo en R7 y R8 los valores del SPSR y SP
    MOV   SP, R7                //Pongo el contenido de R7 (SP anterior) al SP actual
    LDMFD SP!, { R0-R12, PC }^  //Carga de R0 a R12 y el PC desde la pila utilizando SP como base. El ^ hace que se cargue el CPSR desde el SPSR directamente


@ Estos son literal pools explicitos
_RESET_vector:  .word _RESET_handler
_UND_vector:    .word _UND_handler
_SVC_vector:    .word _SVC_handler
_PREF_vector:   .word _PREF_handler
_ABT_vector:    .word _ABT_handler
_IRQ_vector:    .word _IRQ_handler

.end
