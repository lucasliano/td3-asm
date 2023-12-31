#ifndef __MMU_PRIMITIVES_LIB_H
#define __MMU_PRIMITIVES_LIB_H

#include "../inc/mmu_tools.h"
#include "../inc/asm_utils.h"

// Estos includes solo sirven para los defines que se usan en 
// la paginateIdentityMapping()
#include "../inc/gic.h"
#include "../inc/timer.h"

// Este define se puede quitar una vez terminado de debuggear
#include "../inc/debug.h"


// Definición de funciones
void initFirstLevelTable(uint32_t* firstLevelBaseAddr);
void initSecondLevelTable(uint32_t* SecondLevelBaseAddr);
uint32_t getNewSecondLevelAddress(uint32_t taskIndex);
void mapNewSmallPage( uint32_t taskIndex, uint32_t vmaAddr, uint32_t phyAddr, uint8_t nonExecute, uint8_t privilage);
void paginateIdentityMapping(uint32_t phy_table_addr);


#endif // __MMU_PRIMITIVES_LIB_H