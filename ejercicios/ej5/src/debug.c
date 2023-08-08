#include "../inc/debug.h"


__attribute__((section(".debug"))) void debug( void )
{
    asm("NOP");
}

__attribute__((section(".debug"))) void cannot_create_task_debug( void )
{
    // Esta tarea no hace nada. Simplemente nos sirve para agregar un breakpoint en el gdb
    // y podes debuggear con mayor facilidad si se crearon las tareas o no.
    
    asm("NOP");
}


