.code 32
/*--- Variables globales --- */
.global _RESET_handler
.global _UND_handler
.global _SVC_handler
.global _PREF_handler
.global _ABT_handler
.global _IRQ_handler

/*--- Variables importadas --- */
.extern _start
.extern undef_kernel_handler
.extern swi_kernel_handler
.extern pref_abort_kernel_handler
.extern data_abort_kernel_handler
.extern irq_kernel_handler

/*--- Definiciones --- */
.include "defines_asm.h"

/*--- Arranca la sección --- */
.section .handlers, "a"

_RESET_handler:
    @ Este handler tendría que hacer otras cosas como apagar la MMU
    B _start

_UND_handler:
    //No modifco el LR porque se debe continuar en la instrucción siguiente a la que genero la excepción
    STMFD SP!, { R0-R12, LR } //Guarda de R0 a R12 y LR en la pila, usando SP como base y decrementando automáticamente su valor.
    MOV   R7, SP              //Se copia el contenido de SP a R7
    MRS   R8, SPSR            //Se copia el contenido de SPSR a R8
    PUSH {R7, R8}             //Se guardan en la pila el valor de R7 y R8

    BL undef_kernel_handler
    
    POP  {R7, R8}              //Guardo en R7 y R8 los valores del SPSR y SP
    MOV   SP, R7               //Pongo el contenido de R7 (SP anterior) al SP actual
    LDMFD SP!, { R0-R12, PC }^ //Carga de R0 a R12 y el PC desde la pila utilizando SP como base. El ^ hace que se cargue el CPSR desde el SPSR directamente 

_SVC_handler:
    //No modifco el LR porque se debe continuar en la instruccion siguiente a la que genero la excepción
    STMFD SP!, { R0-R12, LR } //Guarda de R0 a R12 y LR en la pila, usando SP como base y decrementando automáticamente su valor.
    MOV   R7, SP              //Se copia el contenido de SP a R7
    MRS   R8, SPSR            //Se copia el contenido de SPSR a R8
    PUSH {R7, R8}             //Se guardan en la pila el valor de R7 y R8

    BL swi_kernel_handler

    POP  {R7, R8}              //Guardo en R7 y R8 los valores del SPSR y SP
    MOV   SP, R7               //Pongo el contenido de R7 (SP anterior) al SP actual
    LDMFD SP!, { R0-R12, PC }^ //Carga de R0 a R12 y el PC desde la pila utilizando SP como base. El ^ hace que se cargue el CPSR desde el SPSR directamente 

_PREF_handler:
    SUB   LR, LR, #4          //Guardo en LR la posición actual - 4, que será la dirección de retorno luego de resolver la interrupción
    STMFD SP!, { R0-R12, LR } //Guarda de R0 a R12 y LR en la pila, usando SP como base y decrementando automáticamente su valor.
    MOV   R7, SP              //Se copia el contenido de SP a R7
    MRS   R8, SPSR            //Se copia el contenido de SPSR a R8
    PUSH {R7, R8}             //Se guardan en la pila el valor de R7 y R8

    BL pref_abort_kernel_handler

    POP  {R7, R8}              //Guardo en R7 y R8 los valores del SPSR y SP
    MOV   SP, R7               //Pongo el contenido de R7 (SP anterior) al SP actual
    LDMFD SP!, { R0-R12, PC }^ //Carga de R0 a R12 y el PC desde la pila utilizando SP como base. El ^ hace que se cargue el CPSR desde el SPSR directamente
 

_ABT_handler:
    SUB   LR, LR, #8            //Guardo en LR la posición actual - 8, que será la dirección de retorno luego de resolver la interrupción
    STMFD SP!, { R0-R12, LR }   //Guarda de R0 a R12 y LR en la pila, usando SP como base y decrementando automáticamente su valor.
    MOV   R7, SP                //Se copia el contenido de SP a R7
    MRS   R8, SPSR              //Se copia el contenido de SPSR a R8
    PUSH {R7, R8}               //Se guardan en la pila el valor de R7 y R8

    BL data_abort_kernel_handler
    
    POP  {R7, R8}               //Guardo en R7 y R8 los valores del SPSR y SP
    MOV   SP, R7                //Pongo el contenido de R7 (SP anterior) al SP actual
    LDMFD SP!, { R0-R12, PC }^  //Carga de R0 a R12 y el PC desde la pila utilizando SP como base. El ^ hace que se cargue el CPSR desde el SPSR directamente

_IRQ_handler:
    @ Deshabilito las IRQ y las FIQ hasta terminar de atender esta interrupción
    @ MSR cpsr_c, (1 << I_BIT) | (0 << F_BIT) | (0 << T_BIT) | (IRQ_MODE) 
    @ TODO: Ver dónde debería volverse a activar la IRQ.       

    @ Corrijo la posición del link register.
    SUB   LR, LR, #4            @ Esto es porque tengo que volver a la instrucción que sigue a la de mi PC. 
                                @ O sea el PC esta en +8 por el pipeline, y quiero ir a una más, entonces resto 4.

    @ Algun comportamiento que necesito tener entre llamadas de IRQs:
    ADD   R10, R10, #1          @ R10 va a contiene la cantidad de veces que se entro en IRQ.
    

    @ Salvamos registros importantes:
    STMFD SP!, { R0-R12, LR }   @ Guarda de R0 a R12 y LR en la pila, usando SP como base y decrementando automáticamente su valor.
                                @ PUSH es equivalente a esta instrucción.
    
    @ TODO: Preguntar si esto es para interrupciones anidadas
    MOV   R7, SP                @ Se copia el contenido de SP a R7.
    MRS   R8, SPSR              @ Se copia el contenido de SPSR a R8. Usamos MRS porque SPSR es un registro del sistema.
    PUSH {R7, R8}               @ Se guardan en la pila el valor de R7 y R8.

    @ Branch al handler del kernel en C:
    BL    irq_kernel_handler    @ Llamo al handler de irq.
    
    ADD   R9, R9, #2            @ Nótese que esto no se almacena en ningún lugar porque no se hace STR/PUSH en ningun momento.


    @ TODO: Lo mismo acá.. Esto debería ser para interrupciones anidadas
    POP  {R7, R8}              @ Guardo en R7 y R8 los valores del SPSR y SP.
    MOV   SP, R7               @ Pongo el contenido de R7 (SP anterior) al SP actual.
    @ TODO: ¿Por qué no se utiliza el R8 (que almacenó el SPSR)?
    LDMFD SP!, { R0-R12, PC }^ @ Carga de R0 a R12 y el PC desde la pila utilizando SP como base. El ^ hace que se cargue el CPSR desde el SPSR directamente.


@ NOTE: _FIQ_handler está directamente debajo del gic_vector.s

.end
