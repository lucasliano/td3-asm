#ifndef __KERNEL_LIB_H
#define __KERNEL_LIB_H

// Includes
#include "../inc/asm_utils.h"


// Definitions
// typedef enum {
//     IDLE,
//     NORMAL,
//     HIGH
// } task_priority;

// Task Control Block
typedef volatile struct { 
    uint32_t PID;
    // uint32_t priority;
    uint32_t ticks;
    uint32_t isActive;

    uint32_t TTBR0;

    uint32_t SP_FIQ;
    uint32_t SP_IRQ;
    uint32_t SP_SVC;
    uint32_t SP_ABT;
    uint32_t SP_UND;
    uint32_t SP_SYS;
    uint32_t SP_USR;

    uint32_t taskCodeBasePhy;

} TCB_t;

// Definición de funciones
void memcopy(const void *origen, void *destino, unsigned int num_bytes);
void initScheduler (void);
int32_t getFreePID(void);
void preloadStack(uint32_t pid);
int32_t loadTask(void* romBasePhy, uint32_t romSize, uint32_t ticks);
int32_t killTask(uint32_t pid);
void kernelInit(void);
void kernelIdle(void);

#endif // __KERNEL_LIB_H