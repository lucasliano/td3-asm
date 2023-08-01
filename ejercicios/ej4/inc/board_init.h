#ifndef __BOARD_INIT_LIB_H
#define __BOARD_INIT_LIB_H

// Includes
#include "../inc/gic.h"
#include "../inc/timer.h"
#include "../inc/mmu_tools.h"
#include "../inc/asm_utils.h"
#include "../inc/kernel.h"


// Definición de funciones
void preKernelInit(void);
void configureGIC0(void);
void configureTIMER0(void);
void configureMMU(void);
void paginateIdentityMapping(uint32_t phy_table_addr);
void mapNewPage( uint32_t vmaAddr, uint32_t phyAddr, uint8_t pageSize, uint8_t blockExecution, uint8_t memoryType, uint8_t memoryDescription, uint8_t pageShareable, uint8_t privilage);

void initFirstLevelTable(uint32_t* firstLevelBaseAddr);
void initSecondLevelTable(uint32_t* SecondLevelBaseAddr);



#endif // __BOARD_INIT_LIB_H