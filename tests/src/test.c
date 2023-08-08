#include <stdint.h>
#include <stdio.h>
#include <unistd.h> // For the sleep




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

TCB_t taskVector[16];


// --------- Estó ya está en otros archivos -----------------
void memcopy(const void *origen, void *destino, unsigned int num_bytes)
{
    // printf("taskVector[%d].isActive: 0x%08x\n", i, taskVector[i].isActive);
}

void mapNewSmallPage(uint32_t vmaAddr, uint32_t phyAddr, uint8_t nonExecute, uint8_t privilage)
{
    // printf("--- mapNewSmallPage ---\n");
    // printf("vma: 0x%08x\n", vmaAddr);
    // printf("phy: 0x%08x\n", phyAddr);
}

void paginateIdentityMapping(uint32_t ttbr)
{
    // printf("--- paginateIdentityMapping ---\n");
    // printf("ttbr0: 0x%08x\n", ttbr);
}


void print_TCB (TCB_t task)
{
    // printf("task.PID: 0x%08x\n", task.PID);
    // printf("task.ticks: 0x%08x\n", task.ticks);
    printf("task.isActive: 0x%08x\n", task.isActive);
    
    // printf("task.TTBR0: 0x%08x\n", task.TTBR0);
    
    // printf("task.SP_FIQ: 0x%08x\n", task.SP_FIQ);
    // printf("task.SP_IRQ: 0x%08x\n", task.SP_IRQ);
    // printf("task.SP_SVC: 0x%08x\n", task.SP_SVC);
    // printf("task.SP_ABT: 0x%08x\n", task.SP_ABT);
    // printf("task.SP_UND: 0x%08x\n", task.SP_UND);
    // printf("task.SP_SYS: 0x%08x\n", task.SP_SYS);
    // printf("task.SP_USR: 0x%08x\n", task.SP_USR);

    // printf("task.taskCodeBasePhy: 0x%08x\n", task.taskCodeBasePhy);
}

// ------------------------------------------------------------







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
    // asm("MOV R2, SP");  // Me guardo mi SP

    // asm("MSR cpsr_c, (1 << I_BIT) | (1 << F_BIT) | (0 << T_BIT) | (IRQ_MODE)"); // voy a IRQ
    // asm("LDR SP,%0" : "=m"(taskVector[pid].SP_IRQ));    // Pongo el SP del IRQ
    // asm("LDR R1, =0x80000000 ");                        // Cargo la posición del código (vma)
    // asm("SUB R0, SP, #4");
    // asm("STR R1, [R0]");                                // Cargo en el SP la posición del LR

    // asm("MSR cpsr_c, (1 << I_BIT) | (1 << F_BIT) | (0 << T_BIT) | (SYS_MODE)"); // Vuelvo a SYS
    // asm("MOV SP, R2");  // Recupero el valor del stack

    printf("Stack: Ponemos 0x80000000 en 0x%08x\n", taskVector[pid].SP_IRQ);
}


int32_t loadTask(uint32_t romBasePhy, uint32_t romSize, uint32_t ticks)
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
    printf("PID: 0x%08x\n", pid);
    

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
    taskVector[pid].isActive = 0;
}



int main ( void )
{
    uint32_t i = 0;
    printf("\n\n");

    // Completamos las tablas del scheduler
    initScheduler();    // Hay que tener una variable global: TCB_t taskVector[16];


    for (i = 0; i < 18; i++)
    {
        if (loadTask(0x12345678, 0x2000, 10) == -1)
            printf("Cannot create new task\n");
    }

    for (i = 0; i < 16; i++)
    {
        printf("Task %d\n", i);
        print_TCB(taskVector[i]);
        printf("\n");
    }

    killTask(5);

    for (i = 0; i < 16; i++)
    {
        printf("Task %d\n", i);
        print_TCB(taskVector[i]);
        printf("\n");
    }


    if (loadTask(0x12345678, 0x2000, 10) == -1)
        printf("Cannot create new task\n");

    for (i = 0; i < 16; i++)
    {
        printf("Task %d\n", i);
        print_TCB(taskVector[i]);
        printf("\n");
    }


    printf("\n\n");
    return 0;  
    
}