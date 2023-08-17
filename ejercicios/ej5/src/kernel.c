#include "../inc/kernel.h"
#include "../inc/tasks.h"

#include "../inc/debug.h"

// --- Externs ---
extern int _KERNEL_MAX_TASKS;
extern int _TASKS_PHY;
extern int _TASKS_PHY_SIZE;
extern int _SYSTABLES_PHY;
extern int _SYSTABLES_PHY_SIZE;
extern int _STACK_SIZE;

extern int _SYSTABLES_PAGE_SIZE;

// --- ROM de las tareas ---
extern int _TASK0_LMA;
extern int _task0_size;
extern int _TASK1_LMA;
extern int _task1_size;
extern int _TASK2_LMA;
extern int _task2_size;


// --- Global Variables ---
TCB_t taskVector[16];

// |  Region |  Offset |          Size         |   hex(Size)   |
// |---------|---------|-----------------------|---------------|
// |  Tables |   0x0   |         64 kiB        |  0x0001_0000  |
// |  Stacks | 0x10000 | 60 KiB (sobran 4 KiB) |  0x0000_fffc  |
// |   Code  | 0x20000 |         15 MiB        |  0x00fe_0000  |



void memcopy(const void *origen, void *destino, unsigned int num_bytes)
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
    uint32_t taskBaseStackVMA;
    
    // Configuración igual para todas las tareas
    emptyTask.ticks = 0;
    emptyTask.isActive = 0;
    emptyTask.L2TablesCount = 0;
    emptyTask.privilege = USR;


    for (i = 0; i < 16; i++)//(uint32_t) &_KERNEL_MAX_TASKS; i++)
    {
        //  NOTE: Cada tarea puede paginar más de su tamaño en memoria.
        taskBasePhy = (uint32_t) &_TASKS_PHY +  (i * (uint32_t) &_TASKS_PHY_SIZE); //0x71000000 + (i * 0x00100000);
        
        emptyTask.PID = i + 1; // La tarea 5 tiene PID 1
        emptyTask.TTBR0 = taskBasePhy;
        
        
        taskBaseStackPhy = taskBasePhy + (uint32_t) &_SYSTABLES_PHY_SIZE; // 0x10000
        emptyTask.taskBaseStackPhy = taskBaseStackPhy;


        taskBaseStackVMA = 0x60000000; // Pongo la VMA

        emptyTask.SP_FIQ = taskBaseStackVMA + (1 * (uint32_t) &_STACK_SIZE);
        emptyTask.SP_IRQ = taskBaseStackVMA + (2 * (uint32_t) &_STACK_SIZE) - 15*4; // Tenemos en consideración el preload {r0-r12,pc,spsr}
        emptyTask.SP_SVC = taskBaseStackVMA + (3 * (uint32_t) &_STACK_SIZE);
        emptyTask.SP_ABT = taskBaseStackVMA + (4 * (uint32_t) &_STACK_SIZE);
        emptyTask.SP_UND = taskBaseStackVMA + (5 * (uint32_t) &_STACK_SIZE);
        emptyTask.SP_SYS = taskBaseStackVMA + (6 * (uint32_t) &_STACK_SIZE);


        // NOTE: _STACK_SIZE * 6 < 0x10000
        taskBaseCodePhy = taskBaseStackPhy + 0x10000;
        emptyTask.taskCodeBasePhy = taskBaseCodePhy; // Posición a partir de la cual puede estar el código;

        taskVector[i] = emptyTask;
    }

}

uint32_t getFreePID(void)
{
    uint32_t i;
    
    for (i = 0; i < 16 ; i++)
    {
        if (!taskVector[i].isActive)
            return taskVector[i].PID;
    }

    return 0;
}

void preloadStacks(TCB_t task)
{
    uint32_t* sp;

    // Load IRQ_SP pointer
    sp = (uint32_t*) (task.TTBR0 + (uint32_t) &_SYSTABLES_PHY_SIZE + (2 * (uint32_t) &_STACK_SIZE));

    sp -= 1; // Me muevo para cargar el LR
    *(sp) = 0x80000000;
    
    sp -= 14; // Me muevo para cargar el SPSR
    if (task.privilege == USR)
    {
        *(sp) = (0 << I_BIT) | (0 << F_BIT) | (0 << T_BIT) | (USR_MODE);
    }else{
        // SYS
        *(sp) = (0 << I_BIT) | (0 << F_BIT) | (0 << T_BIT) | (SYS_MODE);
    }

    return;
}

uint32_t loadTask(uint32_t romBasePhy, uint32_t romSize, uint32_t ticks, uint32_t privileges)
{
    uint32_t i;
    uint32_t vmaOffset;
    uint32_t pid;
    uint32_t taskIndex;

    // === Me fijo si hay espacio disponible ===
    pid = getFreePID();
    if (!pid){
        return FALSE;  // Si no hay espacio, te devuelve un 0.
    }
    taskIndex = pid - 1; // El task.PID = i + 1. Ver initScheduler().
    

    // === Cargo la tarea en taskVector ===
    taskVector[taskIndex].isActive = 1;
    taskVector[taskIndex].ticks = ticks;
    taskVector[taskIndex].privilege = privileges;

    // === Desactivo paginación ===
    // TODO: Averiguar si la MMU estaba prendida para saber si apagarla y volver a prender dsps
    // TODO: Agregar TLB clean
    // MMU_Disable();
    
    // === Copio de ROM a RAM la tarea ===
    memcopy((const void *) romBasePhy, (void *) taskVector[taskIndex].taskCodeBasePhy , romSize);


    // === Creo una tabla nueva de paginación en la nueva TTBR0 ===
    paginateIdentityMapping(taskIndex);

    // Creo páginas para la systable
    // NOTE: Esto no es obligatorio. Lo mapeamos en una dirección falopa nomas
    for (i = taskVector[taskIndex].TTBR0; i < taskVector[taskIndex].TTBR0 + (uint32_t) &_SYSTABLES_PHY_SIZE ; i+= (uint32_t)&_SYSTABLES_PAGE_SIZE)
    {
        vmaOffset = i - (taskVector[taskIndex].TTBR0);
        mapNewSmallPage(
            taskIndex,              // Tarea para la cual estamos creando el mapa
            0x50000000 + vmaOffset, // VMA
            i,                      // PHY
            XN_ALLOWEXECUTION,      // Permiso de ejecución
            PL0_R                   // Privilegios de acceso
        );
    }

    // Creo páginas para los stacks de la tarea
    // TODO: Reemplazar 0x10000 por _SYSTABLES_PHY_SIZE
    for (i = taskVector[taskIndex].TTBR0 + 0x10000; i < taskVector[taskIndex].TTBR0 + 0x20000 ; i+= (uint32_t)&_SYSTABLES_PAGE_SIZE)
    {
        vmaOffset = i - (taskVector[taskIndex].TTBR0 + 0x10000);
        mapNewSmallPage(taskIndex, 0x60000000 + vmaOffset, i, XN_ALLOWEXECUTION, PL0_RW);
    }

    // Creo paginas del taskCode
    for (i = taskVector[taskIndex].taskCodeBasePhy; i < taskVector[taskIndex].taskCodeBasePhy + romSize ; i+= (uint32_t)&_SYSTABLES_PAGE_SIZE)
    {
        vmaOffset = i - (taskVector[taskIndex].taskCodeBasePhy);
        mapNewSmallPage(taskIndex, 0x80000000 + vmaOffset, i, XN_ALLOWEXECUTION, PL0_RW);
    }

    // === Precago el stack en el modo IRQ ===
    // Le paso la dirección PHY donde del SP_IRQ
    // _preloadStack(taskVector[taskIndex].TTBR0 + (uint32_t) &_SYSTABLES_PHY_SIZE + (2 * (uint32_t) &_STACK_SIZE));
    preloadStacks(taskVector[taskIndex]);

    // === Reactivo paginación ===
    // MMU_Enable();


    return TRUE;
}

uint32_t killTask(uint32_t taskIndex)
{
    // TODO: Por ahora no se usa
    taskVector[taskIndex].isActive = 0;


    return TRUE;
}

void TCB2TTBR0(uint32_t taskIndex)
{
    TTBCR ttbcr = MMU_Get_TTBCR();
    TTBR0 ttbr0 = MMU_Get_TTBR0();
    uint32_t phyL1;
    uint8_t tt_size = ttbcr.T0SZ;

    if(tt_size == 0)
    {
        // Solamente se usa TTBR0 para la traducción, ver TRM B3.5.5

        phyL1 = taskVector[taskIndex].TTBR0;
        ttbr0.ttbr0 = (phyL1 & 0xFFFFC000);
        MMU_Set_TTBR0(ttbr0);
    }
}




void kernelInit(void)
{
    //------------------------------------------------------------------------------//
    // Inicializo el scheduler
    //------------------------------------------------------------------------------//    
    initScheduler();

    //------------------------------------------------------------------------------//
    // Inicializo las tareas con las que amanece el sistem
    //------------------------------------------------------------------------------//
    
    // FIXME: Yo quería que puedas pasarle un ptr a función, pero el tenes que pasar la LMA
    // if (loadTask(&kernelBackground, 16, 10, SYS)) // Siempre tenemos que amanecer con alguna tarea
    
    // La primer tarea es el KernelIdle
    if (loadTask((uint32_t) &_TASK0_LMA, (uint32_t) &_task0_size, 1, SYS))
        cannot_create_task_debug();
    
    if (loadTask((uint32_t) &_TASK1_LMA, (uint32_t) &_task1_size, 1, SYS))
        cannot_create_task_debug();

    if (loadTask((uint32_t) &_TASK2_LMA, (uint32_t) &_task2_size, 1, USR))
        cannot_create_task_debug();



    // TODO: No funciona el cambio de estado.. Por algún motivo no popea bien
    //       Cuando tiene que cambiar de tarea, a pesar de que está bien cargado el stack.



    
    //------------------------------------------------------------------------------//
    // Activamos la MMU (Contexto de la tarea 0)
    //------------------------------------------------------------------------------//
    TCB2TTBR0(0);           // Setteo la TTBR0

    MMU_Invalidate_TLB();
    MMU_Enable();
    MMU_Invalidate_TLB();

    //------------------------------------------------------------------------------//
    // Habilitamos las interrupciones
    //------------------------------------------------------------------------------//
    _irq_enable();

    //------------------------------------------------------------------------------//
    // Largamos el kernel
    //------------------------------------------------------------------------------//
    asm("WFI");
}