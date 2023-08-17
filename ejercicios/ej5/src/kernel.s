.code 32                @ Arranca el código en modo ARM (32b)
/*--- Variables globales --- */
.global _READ_SP
.global _WRITE_SP
.global _TCB2Stacks
.global _stacks2TCB
.global _preloadStack

/*--- Variables importadas --- */
.include "../inc/defines_asm.h"

/*--- Definiciones --- */

/*--- Arranca la sección --- */
.section .text, "a"

.align 4    
_TCB2Stacks:
    //------------------------------------------------------------------------------//
    // Copia los valores del TCB en los SP. Se deshabilitan las FIQ/IRQ.
    // Recibe un TCB_t* task
    // De esta manera task = task.PID y (task + 4) = task.ticks
    //------------------------------------------------------------------------------//

    MOV R1, LR  @ Guardo en R1 porque R0 está ocupado

    @ --- FIQ ---
    MSR cpsr_c, (1 << I_BIT) | (1 << F_BIT) | (0 << T_BIT) | (FIQ_MODE)
    LDR SP, [R0, #28] @ El offset de SP_FIQ es 6*4 

    @ --- SVC ---
    MSR cpsr_c, (1 << I_BIT) | (1 << F_BIT) | (0 << T_BIT) | (SVC_MODE)
    LDR SP, [R0, #32] @ El offset de SP_SVC es 7*4

    @ --- ABT ---
    MSR cpsr_c, (1 << I_BIT) | (1 << F_BIT) | (0 << T_BIT) | (ABT_MODE)
    LDR SP, [R0, #36] @ El offset de SP_ABT es 8*4

    @ --- UND ---
    MSR cpsr_c, (1 << I_BIT) | (1 << F_BIT) | (0 << T_BIT) | (UND_MODE)
    LDR SP, [R0, #40] @ El offset de SP_UND es 9*4

    @ --- SYS ---
    MSR cpsr_c,(1 << I_BIT) | (1 << F_BIT) | (0 << T_BIT) | (SYS_MODE)
    LDR SP, [R0, #44] @ El offset de SP_SYS es 10*4

    @ --- IRQ ---
    MSR cpsr_c, (1 << I_BIT) | (1 << F_BIT) | (0 << T_BIT) | (IRQ_MODE)
    LDR SP, [R0, #48] @ El offset de SP_IRQ es 11*4

    MOV LR, R1
    BX LR

    @ Siempre termino corriendo en IRQ

_stacks2TCB:
    //------------------------------------------------------------------------------//
    // Copia los valores de los SP en el TCB. Se deshabilitan las FIQ/IRQ.
    // Recibe un TCB_t* task
    // De esta manera task = task.PID y (task + 4) = task.ticks
    //------------------------------------------------------------------------------//

    MOV R1, LR  @ Guardo en R1 porque R0 está ocupado

    @ --- FIQ ---
    MSR cpsr_c, (1 << I_BIT) | (1 << F_BIT) | (0 << T_BIT) | (FIQ_MODE)    
    STR SP, [R0, #28] @ El offset de SP_FIQ es 6*4 

    @ --- SVC ---
    MSR cpsr_c, (1 << I_BIT) | (1 << F_BIT) | (0 << T_BIT) | (SVC_MODE)
    STR SP, [R0, #32] @ El offset de SP_SVC es 7*4

    @ --- ABT ---
    MSR cpsr_c, (1 << I_BIT) | (1 << F_BIT) | (0 << T_BIT) | (ABT_MODE)
    STR SP, [R0, #36] @ El offset de SP_ABT es 8*4

    @ --- UND ---
    MSR cpsr_c, (1 << I_BIT) | (1 << F_BIT) | (0 << T_BIT) | (UND_MODE)
    STR SP, [R0, #40] @ El offset de SP_UND es 9*4

    @ --- SYS ---
    MSR cpsr_c,(1 << I_BIT) | (1 << F_BIT) | (0 << T_BIT) | (SYS_MODE)
    STR SP, [R0, #44] @ El offset de SP_SYS es 10*4

    @ --- IRQ ---
    MSR cpsr_c, (1 << I_BIT) | (1 << F_BIT) | (0 << T_BIT) | (IRQ_MODE)
    STR SP, [R0, #48] @ El offset de SP_IRQ es 11*4

    MOV LR, R1
    BX LR

    @ Siempre termino corriendo en IRQ

.end