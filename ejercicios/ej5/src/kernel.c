#include "../inc/kernel.h"
#include "../inc/tasks.h"
#include "../inc/board_init.h"
#include "../inc/debug.h"

// Global Variables
extern int _KERNEL_MAX_TASKS;
extern int _TASKS_PHY;
extern int _TASKS_PHY_SIZE;
extern int _SYSTABLES_PHY;
extern int _SYSTABLES_PHY_SIZE;
extern int _PUBLIC_STACK_SIZE;

TCB_t taskVector[16];





__attribute__((section(".kernel"))) void memcopy(const void *origen, void *destino, unsigned int num_bytes)
{
    int* source  = (int*) origen;
    int* dest = (int*) destino;
    unsigned int n = num_bytes;

    unsigned int i;

    for (i = 0; i < n; i++)
    {
        dest[i] = source[i]; 
    }
}

void initScheduler (void)
{
    uint32_t i = 0;
    TCB_t emptyTask;

    uint32_t taskBasePhy, taskBaseStackPhy, taskBaseCodePhy;
    
    // Configuración igual para todas las tareas
    emptyTask.ticks = 10;
    emptyTask.isActive = 0;

    for (i = 0; i < 16; i++)//(uint32_t) &_KERNEL_MAX_TASKS; i++)
    {
        // FIXME: Generalizar!
        taskBasePhy = 0x71000000 + (i * 0x00100000); //(uint32_t) &_TASKS_PHY +  (i * (uint32_t) &_TASKS_PHY_SIZE);
        
        emptyTask.PID = i;
        emptyTask.TTBR0 = 0x71000000 + (i * 0x00100000); //(uint32_t) &_SYSTABLES_PHY +  (i * (uint32_t) &_SYSTABLES_PHY_SIZE);
        
        
        // NOTE: taskBaseStackVma = 0x6000_0000
        taskBaseStackPhy = taskBasePhy + 0x10000; // TODO: _SYSTABLES_PHY_SIZE

        emptyTask.SP_FIQ = taskBaseStackPhy + 1 * 512; // TODO: _PUBLIC_STACK_SIZE
        emptyTask.SP_IRQ = taskBaseStackPhy + 2 * 512;
        emptyTask.SP_SVC = taskBaseStackPhy + 3 * 512;
        emptyTask.SP_ABT = taskBaseStackPhy + 4 * 512;
        emptyTask.SP_UND = taskBaseStackPhy + 5 * 512;
        emptyTask.SP_SYS = taskBaseStackPhy + 6 * 512;
        emptyTask.SP_USR = taskBaseStackPhy + 7 * 512;

        taskBaseCodePhy = taskBasePhy + 0x20000;
        emptyTask.taskCodeBasePhy = taskBaseCodePhy; // Posición a partir de la cual puede estar el código;

        taskVector[i] = emptyTask;
    }

}

int32_t getFreePID(void)
{
    uint32_t i = 0;
    
    for (i = 0; i < 16 ; i++)
    {
        if (!taskVector[i].isActive)
            return (int32_t) taskVector[i].PID;
    }

    return -1;
}

void preloadStack(uint32_t pid)
{
    asm(".include \"defines_asm.h\"");
    asm("MOV R2, SP");  // Me guardo mi SP

    asm("MSR cpsr_c, (1 << I_BIT) | (1 << F_BIT) | (0 << T_BIT) | (IRQ_MODE)"); // voy a IRQ
    asm("LDR SP,%0" : "=m"(taskVector[pid].SP_IRQ));    // Pongo el SP del IRQ
    asm("LDR R1, =0x80000000 ");                        // Cargo la posición del código (vma)
    asm("SUB R0, SP, #4");
    asm("STR R1, [R0]");                                // Cargo en el SP la posición del LR

    asm("MSR cpsr_c, (1 << I_BIT) | (1 << F_BIT) | (0 << T_BIT) | (SYS_MODE)"); // Vuelvo a SYS
    asm("MOV SP, R2");  // Recupero el valor del stack
}

int32_t loadTask(void* romBasePhy, uint32_t romSize, uint32_t ticks)
{
    uint32_t i;
    uint32_t vmaOffset;
    int32_t temp;
    uint32_t pid;

    // === Me fijo si hay espacio disponible ===
    temp = getFreePID();
    if (temp == -1){
        return -1;  // Si no hay espacio, te devuelve un -1.
    }
    pid = (uint32_t) temp;
    

    // === Cargo la tarea en taskVector ===
    taskVector[pid].isActive = 1;
    taskVector[pid].ticks = ticks;

    // === Desactivo paginación ===
    //MMU_Disable();
    
    // === Copio de ROM a RAM la tarea ===
    memcopy((const void *) romBasePhy, (void *) taskVector[pid].taskCodeBasePhy , romSize);


    // === Creo una tabla nueva de paginación en la nueva TTBR0 ===
    // Creo una tabla L1 a partir de TTBR0
    paginateIdentityMapping(taskVector[pid].TTBR0);

    // Creo páginas para la systable
    // NOTE: Esto no es obligatorio. Lo mapeamos en una dirección falopa nomas
    // TODO: Reemplazar 0x10000 por _SYSTABLES_PHY_SIZE
    for (i = taskVector[pid].TTBR0; i < taskVector[pid].TTBR0 + 0x10000 ; i+= 0x1000)//(uint32_t)&_SYSTABLES_PAGE_SIZE)
    {
        vmaOffset = i - taskVector[pid].TTBR0;
        mapNewSmallPage(0x50000000 + vmaOffset, i, 0,0);//XN_ALLOWEXECUTION, PL1_RW);
    }

    // Creo páginas para los stacks de la tarea
    // TODO: Reemplazar 0x10000 por _SYSTABLES_PHY_SIZE
    for (i = taskVector[pid].TTBR0 + 0x10000; i < taskVector[pid].TTBR0 + 0x20000 ; i+= 0x1000)//(uint32_t)&_SYSTABLES_PAGE_SIZE)
    {
        mapNewSmallPage(0x60000000, i, 0,0);//XN_ALLOWEXECUTION, PL1_RW);
    }

    // Creo paginas del taskCode
    for (i = taskVector[pid].taskCodeBasePhy; i < taskVector[pid].taskCodeBasePhy + romSize ; i+= 0x1000)//(uint32_t)&_SYSTABLES_PAGE_SIZE)
    {
        vmaOffset = i - taskVector[pid].taskCodeBasePhy;
        mapNewSmallPage(0x80000000 + vmaOffset, i, 0,0);//XN_ALLOWEXECUTION, PL1_RW);
    }

    // === Precago el stack en el modo IRQ ===
    preloadStack(pid);

    // === Reactivo paginación ===
    // MMU_Enable();


    return 0;
}

int32_t killTask(uint32_t pid)
{
    // TODO: Por ahora no se usa
    taskVector[pid].isActive = 0;


    return 0;
}


void loadStacks( void )
{
    // TODO: Mejorar
    //------------------------------------------------------------------------------//
    // Inicialización de los stack pointers
    //------------------------------------------------------------------------------//
    
    // NOTE: Ver que solamente se están alterando los bits [7:0] por usar el _c.
    //       Esto se puede ver mejor en la sección B9.3.11 del ARM Architecture Reference Manual.
    
    // Inicialización del Stack Pointer de cada modo. Dejamos deshabilitadas las interrupciones.
    asm(".include \"defines_asm.h\"");
    // --- FIQ ---
    asm("MSR cpsr_c, (1 << I_BIT) | (1 << F_BIT) | (0 << T_BIT) | (FIQ_MODE)");
    asm("LDR SP, =_FIQ_STACK_TOP");

    // --- IRQ ---
    asm("MSR cpsr_c, (1 << I_BIT) | (1 << F_BIT) | (0 << T_BIT) | (IRQ_MODE)");
    asm("LDR SP, =_IRQ_STACK_TOP");

    // --- SVC ---
    asm("MSR cpsr_c, (1 << I_BIT) | (1 << F_BIT) | (0 << T_BIT) | (SVC_MODE)");
    asm("LDR SP, =_SVC_STACK_TOP");

    // --- ABT ---
    asm("MSR cpsr_c, (1 << I_BIT) | (1 << F_BIT) | (0 << T_BIT) | (ABT_MODE)");
    asm("LDR SP, =_ABT_STACK_TOP");

    // --- UND ---
    asm("MSR cpsr_c, (1 << I_BIT) | (1 << F_BIT) | (0 << T_BIT) | (UND_MODE)");
    asm("LDR SP, =_UND_STACK_TOP");

    // --- SYS ---
    asm("MSR cpsr_c,(1 << I_BIT) | (1 << F_BIT) | (0 << T_BIT) | (SYS_MODE)");
    asm("LDR SP, =_SYS_STACK_TOP");

    // --- USR ---
    asm("MSR cpsr_c,(1 << I_BIT) | (1 << F_BIT) | (0 << T_BIT) | (USR_MODE)");
    asm("LDR SP, =_USR_STACK_TOP");
    // Nos quedamos en el USR corriendo
}






void kernelInit(void)
{
    initScheduler();


    //------------------------------------------------------------------------------//
    // Inicializo las tareas con las que amanece el sistem
    //------------------------------------------------------------------------------//
    
    // NOTE: loadTask(void* romBasePhy, uint32_t romSize, uint32_t ticks)

    if (loadTask(&kernelIdle, 16, 10) == -1)        // La tarea 0 es siempre IDLE
        cannot_create_task_debug();
    
    if (loadTask(&task1, 16, 10) == -1)
        cannot_create_task_debug();

    if (loadTask(&task2, 16, 10) == -1)
        cannot_create_task_debug();


    //------------------------------------------------------------------------------//
    // Activamos la MMU
    //------------------------------------------------------------------------------//
    MMU_Invalidate_TLB();
    MMU_Enable();
    MMU_Invalidate_TLB();

    //------------------------------------------------------------------------------//
    // Habilitamos las interrupciones
    //------------------------------------------------------------------------------//
    _irq_enable();


    kernelIdle();
}


__attribute__((interrupt("naked"))) void kernelIdle(void)
{   
    // NOTE: Esta línea genera un data abort
    // uint32_t myvar = *((uint32_t*) 0x80000000); 

    // NOTE: Esta línea genera un prefetch abort
    // asm("B 0x80000000");    


    while(1)
    {
        asm("WFI");
    }

    return; // Should never get here
}