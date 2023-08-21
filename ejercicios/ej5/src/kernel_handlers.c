#include "../inc/kernel.h"

extern int _KERNEL_STACKS_PHY;
extern int _STACK_SIZE;

extern TCB_t taskVector[16];


uint32_t currTask = 0;

uint32_t timeframeCounter = 0;
uint32_t currentTaskCounter = 0;

__attribute__((section(".handlers"))) __attribute__((interrupt("undef"))) void undef_kernel_handler()
{

}

__attribute__((section(".handlers"))) __attribute__((interrupt("swi"))) void swi_kernel_handler()
{

}

__attribute__((section(".handlers"))) __attribute__((interrupt("abort"))) void pref_abort_kernel_handler()
{

}

__attribute__((section(".handlers"))) void data_abort_kernel_handler()
{
    /* ------------------------- Funcionamiento actual ------------------------------------
    * El programa va darte tanta memoria como pueda en la 0x8000_0000.
    * FIXME: Debería verificar que la posición de memoria que tira data abort está
    *        en el rango de direcciones válidos para task_data (0x9i00_0000 a 0x9iFF_FFFF)
    *        donde i es el índice de la tarea.
    * ------------------------------------------------------------------------------------- */
    
    // Desactivo la MMU
    MMU_Disable();


    DFSR dfsr = MMU_Get_DFSR(); // Por ahora no verifico nada. Solo lo hago para ver que fue.

    DFAR dfar = MMU_Get_DFAR(); // Saco el address que me generó la excepción.


    // Creo una nueva página en la 0x8000_0000
    mapNewSmallPage(
        currTask,
        dfar.data,
        0x80000000 + ( 0x01000000 * currTask) + ( 0x1000 * taskVector[currTask].dataPageCount),
        XN_BLOCKEXECUTION,
        PL0_RW
    );

    taskVector[currTask].dataPageCount++;


    // Reactivo la MMU
    MMU_Invalidate_TLB();
    MMU_Enable();
}


uint32_t findNextAvailableTask( uint32_t currIndex )
{
    //-----------------------------------------------------//
    // Devuelve el index de la siguiente tarea cuando hay
    // sino devuelve 0, que se tiene que interpretar como
    // una llamdada al kernelIdle
    //-----------------------------------------------------//
    

    uint32_t i;

    for (i = currIndex + 1; i < 16 ; i++)
    {
        if (taskVector[i].isActive)
            return i;
    }

    return 0;
}


__attribute__((section(".handlers"))) __attribute__((naked)) void irq_kernel_handler(){
    _gicc_t* const GICC0 = (_gicc_t*)GICC0_ADDR;
    _timer_t* const TIMER0 = (_timer_t*)TIMER0_ADDR;

    // static uint32_t timeframeCounter = 0;
    // static uint32_t currentTaskCounter = 0;
    uint32_t nextTask;

    uint32_t id = GICC0->IAR;
    
    // NOTE: Es una parte muy delicada del código porque como hay un posible cambio de contexto
    //       no podemos jugar con los registros libremente. Vamos a utilizar el R10 como LR.
    //       Tener cuidado con las llamadas a función. Si algo no funciona es muy posible que el
    //       fp se haya ido de viaje.

    switch(id){
        case 36: // Timer interrupt
            GICC0->EOIR = id;           //Esto da por finalizada la interrupción (WFI funca)
            TIMER0->Timer1IntClr = 0x1; //Al escribir cualquier valor en el registro se limpia la interrupcion del contador

            // Necesito salvar el LR antes de cambiar de contexto
            asm("MOV R10, LR");
            debugTask(currTask);

            // --- Lógica de scheduling ---
            if (timeframeCounter)
            {
                // Acá estamos dentro del timeframe
                timeframeCounter -= 1;

                if (currentTaskCounter)
                {
                    // Estamos dentro de los ticks de la tarea
                    currentTaskCounter -= 1;
                }else{
                    // Cambiamos entre tareas dentro del time frame

                    _stacks2TCB(&taskVector[currTask]); // Guardamos los SP
                    
                    nextTask = findNextAvailableTask (currTask);
                    if (nextTask) // Si hay una tarea para llamar, sino IDLE
                    {
                        currTask = nextTask;
                        currentTaskCounter = taskVector[nextTask].ticks - 1;
                    } else {
                        currTask = 0;
                        currentTaskCounter = timeframeCounter; // Me quedo el tiempo restante del frame acá.
                    }

                    MMU_Invalidate_TLB();
                    TCB2TTBR0(currTask);                // Setteo la TTBR0
                    _TCB2Stacks(&taskVector[currTask]); // Cargamos los SP
                }
            }else{
                // Saltamos de kernelIdle a task0
                timeframeCounter = 5 - 1;
                
                // FIXME: La tarea 1 siempre tiene que estar activa
                currTask = 1;
                currentTaskCounter = taskVector[currTask].ticks - 1;
                
                MMU_Invalidate_TLB();
                TCB2TTBR0(currTask);                // Setteo la TTBR0
                _TCB2Stacks(&taskVector[currTask]); // Cargamos los SP
            }

            debugTask(currTask);
            asm("MOV LR, R10");

            break;

        default:
            break;
    } 
    

    asm("BX LR");
} 

__attribute__((section(".handlers"))) __attribute__((interrupt("fiq"))) void fiq_kernel_handler(){

}