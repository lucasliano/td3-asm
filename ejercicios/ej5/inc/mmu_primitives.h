#ifndef __MMU_PRIMITIVES_LIB_H
#define __MMU_PRIMITIVES_LIB_H

#include "../inc/mmu_tools.h"
#include "../inc/asm_utils.h"
#include "../inc/kernel.h"

// Estos includes solo sirven para los defines que se usan en 
// la paginateIdentityMapping()
#include "../inc/gic.h"
#include "../inc/timer.h"

// Este define se puede quitar una vez terminado de debuggear
#include "../inc/debug.h"


void paginateIdentityMapping(uint32_t phy_table_addr);
void mapNewSmallPage( uint32_t vmaAddr, uint32_t phyAddr, uint8_t nonExecute, uint8_t privilage);

void initFirstLevelTable(uint32_t* firstLevelBaseAddr);
void initSecondLevelTable(uint32_t* SecondLevelBaseAddr);




#endif // __MMU_PRIMITIVES_LIB_H