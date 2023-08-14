#include "../inc/mmu_tools.h"

__attribute__((section(".text"))) TTBCR MMU_Get_TTBCR(void)
{
    TTBCR ttbcr;

    asm("MRC p15, 0, R0, c2, c0, 2");
    asm("STR R0,%0" : "=m"(ttbcr.ttbcr));

    return ttbcr;
}

__attribute__((section(".text"))) void MMU_Set_TTBCR(TTBCR ttbcr)
{
    asm("LDR R0,%0" : "=m"(ttbcr.ttbcr));
    asm("ISB");                     // Instruction Synchronization Barrier
    asm("DSB");                     // Data Synchronization Barrier
    asm("MCR p15, 0, R0, c2, c0, 2");
}

__attribute__((section(".text"))) TTBR0 MMU_Get_TTBR0()
{
    TTBR0 ttbr0;

    asm("MRC p15, 0, R0, c2, c0, 0");
    asm("STR R0,%0" : "=m"(ttbr0.ttbr0));

    return ttbr0;
}

__attribute__((section(".text"))) void MMU_Set_TTBR0(TTBR0 ttbr0)
{       
    
    asm("LDR R0,%0" : "=m"(ttbr0.ttbr0));
    asm("ISB");                     // Instruction Synchronization Barrier
    asm("DSB");                     // Data Synchronization Barrier
    asm("MCR p15, 0, R0, c2, c0, 0");
    
}

__attribute__((section(".text"))) SCTLR MMU_Get_SCTLR()
{
    SCTLR sctlr;

    asm("MRC p15, 0, R0, c1, c0, 0");
    asm("STR R0,%0" : "=m"(sctlr.sctlr));

    return sctlr;
}

__attribute__((section(".text"))) void MMU_Set_SCTLR(SCTLR sctlr)
{
    asm("LDR R0,%0" : "=m"(sctlr.sctlr));
    asm("ISB");                     // Instruction Synchronization Barrier
    asm("DSB");                     // Data Synchronization Barrier
    asm("MCR p15, 0, R0, c1, c0, 0");
}

__attribute__((section(".text"))) DACR MMU_Get_DACR(void)
{
    DACR dacr;

    asm("MRC p15, 0, R0, c3, c0, 0");
    asm("STR R0,%0" : "=m"(dacr.dacr));

    return dacr;
}

__attribute__((section(".text"))) void MMU_Set_DACR(DACR dacr)
{
    asm("LDR R0,%0" : "=m"(dacr.dacr));
    asm("ISB");                     // Instruction Synchronization Barrier
    asm("DSB");                     // Data Synchronization Barrier
    asm("MCR p15, 0, R0, c3, c0, 0");
}

__attribute__((section(".text"))) void MMU_Enable()
{
    /**
    * @brief Esta función es un wrapper para habilitar el uso de la MMU
    * 
    */
    SCTLR sctlr = MMU_Get_SCTLR();
    sctlr.M = MMUENABLE_YES;
    MMU_Set_SCTLR(sctlr);
    asm("DSB");
}

__attribute__((section(".text"))) void MMU_Disable()
{
    /**
    * @brief Esta función es un wrapper para deshabilitar el uso de la MMU
    * 
    */
    SCTLR sctlr = MMU_Get_SCTLR();
    sctlr.M = MMUENABLE_NO;
    MMU_Set_SCTLR(sctlr);
    asm("DSB");
}

__attribute__((section(".text"))) void MMU_Invalidate_TLB(void)
{
    /**
    * @brief Invalida todas las entradas de la TLB siguiendo el TRM B3.18.7
    * 
    */
    asm("MCR p15, 0, R0, c8, c6, 0"); // Invalidate entire DATA TLB
    asm("MCR p15, 0, R0, c8, c5, 0"); // Invalidate entire INSTRUCTION TLB
    asm("MCR p15, 0, R0, c8, c7, 0"); // Invalidate entire UNIFIED TLB
}

