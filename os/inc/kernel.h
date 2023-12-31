#ifndef __KERNEL_LIB_H
#define __KERNEL_LIB_H

// Includes
#include "../inc/asm_utils.h"
#include "../inc/mmu_primitives.h"
#include "../inc/board_init.h"
#include "../inc/debug.h"

// Definitions


// Task Privileges
typedef enum {
    USR,
    SYS,
} TASK_PLX;


// Task Control Block
typedef volatile struct { 
    uint32_t PID;
    uint32_t ticks;
    uint32_t isActive;
    uint32_t privilege;

    uint32_t TTBR0;
    uint32_t L2TablesCount;

    uint32_t taskBaseStackPhy;

    uint32_t SP_FIQ;
    uint32_t SP_SVC;
    uint32_t SP_ABT;
    uint32_t SP_UND;
    uint32_t SP_SYS;
    uint32_t SP_IRQ;

    // NOTE: Agregar cosas nuevas acá abajo, para evitar mover los index de los SP.

    uint32_t taskCodeBasePhy;

    uint32_t dataPageCount;

} TCB_t;


// Definición de los handlers en C
void undef_kernel_handler ( void );
void swi_kernel_handler ( uint32_t svc_code );
void abort_kernel_handler ( void );
void irq_kernel_handler ( void );
void fiq_kernel_handler ( void );


// Definición de funciones
void memcopy(const void *origen, void *destino, unsigned int num_bytes);
void initScheduler (void);
uint32_t getFreePID(void);
void preloadStacks(TCB_t task);
uint32_t loadTask(uint32_t romBasePhy, uint32_t romSize, uint32_t ticks, uint32_t privileges);
uint32_t killTask(uint32_t taskIndex);
void TCB2TTBR0(uint32_t taskIndex);
void kernelInit(void);


// Esto en kernel.s

extern void _TCB2Stacks( TCB_t* task);
extern void _stacks2TCB( TCB_t* task);
extern void _TCB2StacksABT( TCB_t* task);
extern void _stacks2TCBABT( TCB_t* task);

#endif // __KERNEL_LIB_H