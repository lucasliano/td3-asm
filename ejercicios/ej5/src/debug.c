#include "../inc/debug.h"

extern TCB_t taskVector[16];

uint32_t taskSP_FIQ = 0;
uint32_t taskSP_IRQ = 0;
uint32_t taskSP_SVC = 0;
uint32_t taskSP_ABT = 0;
uint32_t taskSP_UND = 0;
uint32_t taskSP_SYS = 0;

uint32_t taskPID = 0;
uint32_t taskticks = 0;
uint32_t taskisActive = 0;
uint32_t taskprivilege = 0;
uint32_t taskTTBR0 = 0;
uint32_t taskL2TablesCount = 0;
uint32_t taskBaseStackPhy = 0;
uint32_t taskCodeBasePhy = 0;


__attribute__((section(".mydebug"))) __attribute__((naked)) void debugTask( uint32_t index )
{
    taskSP_FIQ = taskVector[index].SP_FIQ;
    taskSP_IRQ = taskVector[index].SP_IRQ;
    taskSP_SVC = taskVector[index].SP_SVC;
    taskSP_ABT = taskVector[index].SP_ABT;
    taskSP_UND = taskVector[index].SP_UND;
    taskSP_SYS = taskVector[index].SP_SYS;

    taskPID             = taskVector[index].PID;
    taskticks           = taskVector[index].ticks;
    taskisActive        = taskVector[index].isActive;
    taskprivilege       = taskVector[index].privilege;
    taskTTBR0           = taskVector[index].TTBR0;
    taskL2TablesCount   = taskVector[index].L2TablesCount;
    taskBaseStackPhy    = taskVector[index].taskBaseStackPhy;
    taskCodeBasePhy     = taskVector[index].taskCodeBasePhy;
    
    
    asm("NOP");
    asm("BX LR");
}


__attribute__((section(".mydebug"))) __attribute__((naked)) void debug( uint32_t valor )
{
    asm("NOP");
    asm("BX LR");
}

__attribute__((section(".mydebug"))) __attribute__((naked)) void cannot_create_task_debug( void )
{
    // Esta tarea no hace nada. Simplemente nos sirve para agregar un breakpoint en el gdb
    // y podes debuggear con mayor facilidad si se crearon las tareas o no.
    
    asm("NOP");
    asm("BX LR");
}


