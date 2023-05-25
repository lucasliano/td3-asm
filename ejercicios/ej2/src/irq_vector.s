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

/*--- Arranca la sección --- */
.section .gic_vector, "a"
_exception_vector_start:
    @ .word 0xDEADBEEF            @ NOTE: Buscar en bless como EFBEADDE


    @ NOTE: Ver notas respecto de este código en el obsidian de la clase del 10-05
    @ LDR pc, =_reset_vector
    @ LDR pc, =_UND_handler 
    @ LDR pc, =_SVC_handler 
    @ LDR pc, =_PREF_handler 
    @ LDR pc, =_ABT_handler 
    @ NOP
    @ LDR pc, =_IRQ_handler


    LDR pc, _RESET_vector_irq  @ Le tenes que pasar una instrucción, porque el procesador espera eso
    LDR pc, _UND_vector_irq  
    LDR pc, _SVC_vector_irq  
    LDR pc, _PREF_vector_irq 
    LDR pc, _ABT_vector_irq  
    NOP
    LDR pc, _IRQ_vector_irq  
    @ --- Starts FIQ Handler to avoid breaking the pipeline ---
_FIQ_hanlder:
    B .


@ Estos son literal pools explicitos
_RESET_vector_irq:  .word _reset_vector
_UND_vector_irq:    .word _UND_handler
_SVC_vector_irq:    .word _SVC_handler
_PREF_vector_irq:   .word _PREF_handler
_ABT_vector_irq:    .word _ABT_handler
_IRQ_vector_irq:    .word _IRQ_handler

.end
