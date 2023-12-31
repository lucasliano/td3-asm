#ifndef __BOARD_INIT_LIB_H
#define __BOARD_INIT_LIB_H

// Includes
#include "../inc/gic.h"
#include "../inc/timer.h"
#include "../inc/mmu_tools.h"
#include "../inc/asm_utils.h"
#include "../inc/kernel.h"


// Definición de funciones
void hardwareInit(void);
void configureGIC0(void);
void configureTIMER0(void);
void configureMMU(void);


#endif // __BOARD_INIT_LIB_H