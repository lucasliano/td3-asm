#include "../inc/mmu_tools.h"
#include "../inc/low_level_cpu_access.h"

/**
 * @brief 
 * 
 */
__attribute__((section(".text"))) ID_MMFR0 MMU_Get_ID_MMFR0(void)
{
    ID_MMFR0 mmfr0;

    asm("MRC p15, 0, R0, c0, c1, 4");
    asm("STR R0,%0" : "=m"(mmfr0.mmfr0));

    return mmfr0;
}

/**
 * @brief 
 * 
 */
__attribute__((section(".text"))) ID_MMFR1 MMU_Get_ID_MMFR1(void)
{
    ID_MMFR1 mmfr1;

    asm("MRC p15, 0, R0, c0, c1, 5");
    asm("STR R0,%0" : "=m"(mmfr1.mmfr1));

    return mmfr1;
}

/**
 * @brief 
 * 
 */
__attribute__((section(".text"))) ID_MMFR2 MMU_Get_ID_MMFR2(void)
{
    ID_MMFR2 mmfr2;

    asm("MRC p15, 0, R0, c0, c1, 6");
    asm("STR R0,%0" : "=m"(mmfr2.mmfr2));

    return mmfr2;
}

/**
 * @brief 
 * 
 */
__attribute__((section(".text"))) ID_MMFR3 MMU_Get_ID_MMFR3(void)
{
    ID_MMFR3 mmfr3;

    asm("MRC p15, 0, R0, c0, c1, 7");
    asm("STR R0,%0" : "=m"(mmfr3.mmfr3));

    return mmfr3;
}

/**
 * @brief 
 * 
 */
__attribute__((section(".text"))) TTBCR MMU_Get_TTBCR(void)
{
    TTBCR ttbcr;

    asm("MRC p15, 0, R0, c2, c0, 2");
    asm("STR R0,%0" : "=m"(ttbcr.ttbcr));

    return ttbcr;
}

/**
 * @brief 
 * 
 */
__attribute__((section(".text"))) void MMU_Set_TTBCR(TTBCR ttbcr)
{
    asm("LDR R0,%0" : "=m"(ttbcr.ttbcr));
    asm("ISB");                     // Instruction Synchronization Barrier
    asm("DSB");                     // Data Synchronization Barrier
    asm("MCR p15, 0, R0, c2, c0, 2");
}

/**
 * @brief 
 * 
 */
__attribute__((section(".text"))) DACR MMU_Get_DACR(void)
{
    DACR dacr;

    asm("MRC p15, 0, R0, c3, c0, 0");
    asm("STR R0,%0" : "=m"(dacr.dacr));

    return dacr;
}

/**
 * @brief 
 * 
 */
__attribute__((section(".text"))) void MMU_Set_DACR(DACR dacr)
{
    asm("LDR R0,%0" : "=m"(dacr.dacr));
    asm("ISB");                     // Instruction Synchronization Barrier
    asm("DSB");                     // Data Synchronization Barrier
    asm("MCR p15, 0, R0, c3, c0, 0");
}

/**
 * @brief 
 * 
 */
__attribute__((section(".text"))) TTBR0 MMU_Get_TTBR0()
{
    TTBR0 ttbr0;

    asm("MRC p15, 0, R0, c2, c0, 0");
    asm("STR R0,%0" : "=m"(ttbr0.ttbr0));

    return ttbr0;
}

/**
 * @brief 
 * 
 */
__attribute__((section(".text"))) void MMU_Set_TTBR0(TTBR0 ttbr0)
{       
    
    asm("LDR R0,%0" : "=m"(ttbr0.ttbr0));
    asm("ISB");                     // Instruction Synchronization Barrier
    asm("DSB");                     // Data Synchronization Barrier
    asm("MCR p15, 0, R0, c2, c0, 0");
    
}

/**
 * @brief 
 * 
 */
__attribute__((section(".text"))) TTBR1 MMU_Get_TTBR1()
{
    TTBR1 ttbr1;

    asm("MRC p15, 0, R0, c2, c0, 1");
    asm("STR R0,%0" : "=m"(ttbr1.ttbr1));

    return ttbr1;
}

/**
 * @brief 
 * 
 */
__attribute__((section(".text"))) void MMU_Set_TTBR1(TTBR1 ttbr1)
{
    asm("LDR R0,%0" : "=m"(ttbr1.ttbr1));
    asm("ISB");                     // Instruction Synchronization Barrier
    asm("DSB");                     // Data Synchronization Barrier
    asm("MCR p15, 0, R0, c2, c0, 1");
}

/**
 * @brief 
 * 
 */
__attribute__((section(".text"))) SCTLR MMU_Get_SCTLR()
{
    SCTLR sctlr;

    asm("MRC p15, 0, R0, c1, c0, 0");
    asm("STR R0,%0" : "=m"(sctlr.sctlr));

    return sctlr;
}

/**
 * @brief 
 * 
 */
__attribute__((section(".text"))) void MMU_Set_SCTLR(SCTLR sctlr)
{
    asm("LDR R0,%0" : "=m"(sctlr.sctlr));
    asm("ISB");                     // Instruction Synchronization Barrier
    asm("DSB");                     // Data Synchronization Barrier
    asm("MCR p15, 0, R0, c1, c0, 0");
}

/**
 * @brief 
 * 
 */
__attribute__((section(".text"))) uint32_t MMU_Get_VBAR()
{
    uint32_t vbar;

    asm("MRC p15, 0, R0, c12, c0, 0");
    asm("STR R0,%0" : "=m"(vbar));

    return vbar;
}

/**
 * @brief 
 * 
 */
__attribute__((section(".text"))) void MMU_Set_VBAR(uint32_t vbar)
{
    asm("LDR R0,%0" : "=m"(vbar));
    asm("MCR p15, 0, R0, c12, c0, 0");
}

/**
 * @brief Esta función obtiene el valor de la dirección física de la primer tabla de traducción de la MMU
 * 
 */
__attribute__((section(".text"))) uint32_t MMU_Get_FirstLevelTranslationTable_PhysicalAddress(void)
{
    TTBCR ttbcr = MMU_Get_TTBCR();
    TTBR0 ttbr0 = MMU_Get_TTBR0();
    //TTBR1 ttbr1 = MMU_Get_TTBR1();
    uint8_t tt_size = ttbcr.T0SZ;
    uint32_t tt_ph_addr = 0;

    if(tt_size == 0)
    {
        // Solamente se usa TTBR0 para la traducción, ver TRM B3.5.5
        tt_ph_addr = ttbr0.ttbr0 & 0xFFFFC000;
    }

    return tt_ph_addr;
}

/**
 * @brief Esta función setea la dirección física de la primer tabla de traducción de la MMU
 * 
 */
__attribute__((section(".text"))) void MMU_Set_FirstLevelTranslationTable_PhysicalAddress(uint32_t ph_addr)
{
    TTBCR ttbcr = MMU_Get_TTBCR();
    TTBR0 ttbr0 = MMU_Get_TTBR0();
    //TTBR1 ttbr1 = MMU_Get_TTBR1();
    uint8_t tt_size = ttbcr.T0SZ;

    if(tt_size == 0)
    {
        // Solamente se usa TTBR0 para la traducción, ver TRM B3.5.5
        ttbr0.ttbr0 = (ph_addr & 0xFFFFC000);
        MMU_Set_TTBR0(ttbr0);
    }
}

/**
 * @brief Esta función es un wrapper para habilitar o no el uso del access flag en los descriptores
 * 
 */
__attribute__((section(".text"))) void MMU_Enable_AccessFlag(ENABLEAF af)
{
    SCTLR sctlr = MMU_Get_SCTLR();

    sctlr.AFE = af;

    MMU_Set_SCTLR(sctlr);
}

/**
 * @brief Esta función es un wrapper para habilitar o no el uso de la MMU
 * 
 */
__attribute__((section(".text"))) void MMU_Enable(MMUENABLE mmuenable)
{
    SCTLR sctlr = MMU_Get_SCTLR();

    sctlr.M = mmuenable;

    // dbg("\r\n\tEscribiendo SCTLR con 0x%x. Chan chan...",sctlr.sctlr);
    MMU_Set_SCTLR(sctlr);
    // dbg("\r\n\tEureka!");
}

/**
 * @brief Invalida todas las entradas de la TLB siguiendo el TRM B3.18.7
 * 
 */
__attribute__((section(".text"))) void MMU_Invalidate_TLB(void)
{
    asm("MCR p15, 0, R0, c8, c6, 0"); // Invalidate entire DATA TLB
    asm("MCR p15, 0, R0, c8, c5, 0"); // Invalidate entire INSTRUCTION TLB
    asm("MCR p15, 0, R0, c8, c7, 0"); // Invalidate entire UNIFIED TLB
}

__attribute__((section(".text"))) void MMU_MapNewPage(uint32_t address, uint32_t address2, uint8_t pageSize, uint8_t blockExecution, uint8_t memoryType, uint8_t memoryDescription, uint8_t pageShareable, uint8_t privilage)
{
    return;
}