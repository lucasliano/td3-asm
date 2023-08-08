#include "../inc/mmu_primitives.h"


// --- Externs ---
extern int _HARDWARE_REGISTERS_INIT;
extern int _HARDWARE_REGISTERS_END;
extern int _PUBLIC_EXCEPTION_VECTOR;
extern int _PUBLIC_GIC_SIZE;
extern int _PUBLIC_STACK_INIT;
extern int _PUBLIC_STACK_END;
extern int _KERNEL_CODE_PHY;
extern int _PUBLIC_KERNEL_CODE_SIZE;
extern int _SYSTABLES_PHY;
extern int _SYSTABLES_END;
extern int __bss_start__;
extern int __bss_end__;

extern int _SYSTABLES_PAGE_SIZE;


// NOTE: Solo para facilitar el debugging
uint32_t globalDebug = 0;
uint32_t vma_global = 0;
uint32_t L1Index_global = 0;
uint32_t L2Index_global = 0;
uint32_t L1Phy_global = 0;
uint32_t L2Phy_global = 0;
uint32_t L1Descriptor_global = 0;
uint32_t L2Descriptor_global = 0;


void initFirstLevelTable(uint32_t* firstLevelBaseAddr)
{
    //------------------------------------------------------------------------------//
    // La siguiente función completa la L1 con descriptores inválidos 
    //------------------------------------------------------------------------------//
    uint32_t index;

    for (index = 0; index < 4096; index ++)
    {
        firstLevelBaseAddr[index] = INVALID_DESCRIPTOR;
    }
    
    return;
} 

void initSecondLevelTable(uint32_t* SecondLevelBaseAddr)
{
    //------------------------------------------------------------------------------//
    // La siguiente función completa la L2 con descriptores inválidos 
    //------------------------------------------------------------------------------//
    uint32_t index;

    for (index = 0; index < 256; index ++)
    {
        SecondLevelBaseAddr[index] = INVALID_DESCRIPTOR;
    }
    
    return;
}

uint32_t getNewSecondLevelAddress()
{
    // Tiene que ser múltiplo de 1KiB = 0x400
    static uint32_t address = (uint32_t)&_SYSTABLES_PHY + 0x4000 - 0x400;  // La primera que retorna va a ser 0x70055000
    address += 0x400;
    initSecondLevelTable((uint32_t*) address);

    globalDebug = address;
    
    return address;
}

void mapNewSmallPage(
    uint32_t vmaAddr,
    uint32_t phyAddr,
    uint8_t nonExecute,
    uint8_t privilage
){
    //==============================================================================//
    // La siguiente función analiza el contenido de la región que se desea mappear 
    //==============================================================================//
    uint32_t L1Index, L2Index;
    uint32_t *L1BaseAddr, *L2BaseAddr;
    FIRSTLEVEL_PAGETABLE_DESCRIPTOR firstLevelDescriptor;
    SECONDLEVEL_SMALLPAGE_DESCRIPTOR secondLevelDescriptor;

    //------------------------------------------------------------------------------//
    // Saco la base de TTBR0
    //------------------------------------------------------------------------------//
    L1BaseAddr = (uint32_t*) MMU_Get_FirstLevelTranslationTable_PhysicalAddress();
    
    //------------------------------------------------------------------------------//
    // Extraigo los indices de L1/L2 y el offset de la VMA
    //------------------------------------------------------------------------------//
    L1Index = (vmaAddr >> 20) & 0xFFF;  //      Saco 0xAAA-_----
    L2Index = (vmaAddr >> 12) & 0xFF;   //      Saco 0x---B_B---
                                        // No se usa 0x----_-CCC.

    //------------------------------------------------------------------------------//
    // >>> Verificamos el descriptor de la tabla L1 <<<
    //------------------------------------------------------------------------------//
    firstLevelDescriptor.descriptor = (uint32_t) L1BaseAddr[L1Index];
    if (firstLevelDescriptor.DESC_TYPE != PAGETABLE)
    {
        // Si no existe descriptor en la L1, la creamos
        L2BaseAddr = (uint32_t*) getNewSecondLevelAddress();

        firstLevelDescriptor.DESC_TYPE = PAGETABLE;                         // Descriptor Type
        firstLevelDescriptor.PXN = 0;                                       // Privileged eXecute Never
        firstLevelDescriptor.NS = NS_NONSECURE;                             // Non Secure
        firstLevelDescriptor.SBZ = 0;                                       // Should Be Zero
        firstLevelDescriptor.DOMAIN = 0;                                    // Domain (Son todos manager)
        firstLevelDescriptor.IMPLEMENTATION_DEFINED = 0;                    // Implementation Defined
        
        // Base Addr
        firstLevelDescriptor.descriptor = ((uint32_t) L2BaseAddr & 0xFFFFFC00) | firstLevelDescriptor.descriptor;
        
        // Guardamos el descriptor en la tabla
        L1BaseAddr[L1Index] = (uint32_t) firstLevelDescriptor.descriptor;
        
    }else{
        // Si existe desacriptor en la L1, sacamos la posición de la L2
        L2BaseAddr = (uint32_t*) (firstLevelDescriptor.descriptor & 0xFFFFFC00);        
    }

    //------------------------------------------------------------------------------//
    // >>> Creamos la página L2 <<<
    //------------------------------------------------------------------------------//
    secondLevelDescriptor.DESC_TYPE = SMALLPAGE;    // Smallpage
    secondLevelDescriptor.XN = nonExecute;          // Non eXecute bit
    

    //  NOTE:   Una región Non-shareable (NSH) solo puede ser accedido por un único agente. 
    //          Por tanto no hay que coordinar los accesos a memoria. Toda la memoria de la beaglebone es non-shareable.
    // Forzamos NSH -> Memory Type = Device
    secondLevelDescriptor.TEX   = 0b010;            
    secondLevelDescriptor.B     = 0;                
    secondLevelDescriptor.C     = 0;                // No Cacheability
    secondLevelDescriptor.S     = 0;                // No Shareability
    secondLevelDescriptor.NG    = 0;                // Non Global = 0 -> TLB global

    switch (privilage)
    {
    case PLX_R:
        secondLevelDescriptor.AP_2  = 0b01;
        secondLevelDescriptor.AP_10 = 0b11;
        break;
    case PL0_RW:
        secondLevelDescriptor.AP_2  = 0b00;
        secondLevelDescriptor.AP_10 = 0b11;
        break;
    case PL0_R:
        secondLevelDescriptor.AP_2  = 0b00;
        secondLevelDescriptor.AP_10 = 0b10;
        break;
    case PL1_RW:
        secondLevelDescriptor.AP_2  = 0b00;
        secondLevelDescriptor.AP_10 = 0b01;
        break;
    case PL1_R:
        secondLevelDescriptor.AP_2  = 0b01;
        secondLevelDescriptor.AP_10 = 0b01;
        break;
    default:    // NO_ACCESS
        secondLevelDescriptor.AP_2  = 0b00;
        secondLevelDescriptor.AP_10 = 0b00;
        break;
    }

    // Base Addr
    secondLevelDescriptor.descriptor = ((uint32_t) phyAddr & 0xFFFFF000) | (secondLevelDescriptor.descriptor & 0xFFF);

    // Guardamos el descriptor en la tabla
    L2BaseAddr[L2Index] = (uint32_t) secondLevelDescriptor.descriptor;

    // FIXME: Remove, just for debug
    vma_global = vmaAddr;
    L1Index_global = L1Index;
    L2Index_global = L2Index;
    L1Phy_global =  ((uint32_t) L1BaseAddr + (L1Index*4));
    L2Phy_global =  ((uint32_t) L2BaseAddr + (L2Index*4));
    L1Descriptor_global = (uint32_t) (firstLevelDescriptor.descriptor);
    L2Descriptor_global = (uint32_t) (secondLevelDescriptor.descriptor);

    return;
}


// NOTE: No aclaro __attribute__((section(".text"))) porque es la región por defecto.
void paginateIdentityMapping(uint32_t phy_table_addr)
{
    //===================================================================================//
    // Esta función pagina toda la región de identity mapping a partir de conocer TTBR0
    //===================================================================================//
    uint32_t i;

    // Setteo la TTBR0
    MMU_Set_FirstLevelTranslationTable_PhysicalAddress((uint32_t)&_SYSTABLES_PHY);
    initFirstLevelTable((uint32_t* )&_SYSTABLES_PHY);    // Limpio la región de memoria donde va la L1


    // === Comienza la creación de tablas ===
    // --- GIC ---
    for (i = 0x0; i < 0 + (uint32_t)&_PUBLIC_GIC_SIZE; i+= (uint32_t)&_SYSTABLES_PAGE_SIZE)
    {
        mapNewSmallPage(i, i, XN_ALLOWEXECUTION, PL1_RW);
    }

    for (i = (uint32_t)&_PUBLIC_EXCEPTION_VECTOR; i < (uint32_t)&_PUBLIC_EXCEPTION_VECTOR + (uint32_t)&_PUBLIC_GIC_SIZE; i+= (uint32_t)&_SYSTABLES_PAGE_SIZE)
    {
        mapNewSmallPage(i, i, XN_ALLOWEXECUTION, PL1_RW);
    }

    // // --- STACK ---
    // for (i = (uint32_t)&_PUBLIC_STACK_INIT; i < (uint32_t)&_PUBLIC_STACK_END; i+= (uint32_t)&_SYSTABLES_PAGE_SIZE)
    // {
    //     mapNewSmallPage(i, i, XN_ALLOWEXECUTION, PL1_RW);
    // }

    // --- Kernel ---
    for (i = (uint32_t)&_KERNEL_CODE_PHY; i < (uint32_t)&_KERNEL_CODE_PHY + ((uint32_t)&_PUBLIC_KERNEL_CODE_SIZE); i+= (uint32_t)&_SYSTABLES_PAGE_SIZE)
    {
        mapNewSmallPage(i, i, XN_ALLOWEXECUTION, PL1_RW);
    }

    // // --- Systables Code ---
    // for (i = (uint32_t)&_SYSTABLES_PHY; i < (uint32_t)&_SYSTABLES_END ; i+= (uint32_t)&_SYSTABLES_PAGE_SIZE)
    // {
    //     mapNewSmallPage(i, i, XN_ALLOWEXECUTION, PL1_RW);
    // }
 
    // --- Hardware Registers ---
    // NOTE: La manera correcta sería mappear todo el bus APB y AHB.
    //       Haciendo esto tendríamos acceso a todos los posibles registros.
    //       Pero eso sería mappear desde 0x1000_0000 hasta 0x2000_0000
    //       que es equivalente a 256 tablas L2.  
    // for (i = (uint32_t)&_HARDWARE_REGISTERS_INIT; i < (uint32_t)&_HARDWARE_REGISTERS_END; i+= (uint32_t)&_SYSTABLES_PAGE_SIZE)
    // {
    //     mapNewSmallPage(i, i, XN_ALLOWEXECUTION, PL1_RW);
    // }

    // NOTE: Para evitar esto solo vamos a mappear los registros que utilizamos
    //       Lo que nos va a ocupar solo 2 páginas
    mapNewSmallPage(TIMER0_ADDR, TIMER0_ADDR, XN_ALLOWEXECUTION, PL1_RW);   // 0x100-_----
    mapNewSmallPage(GICC0_ADDR, GICC0_ADDR, XN_ALLOWEXECUTION, PL1_RW);     // 0x1E0-_----

    // >>> Variables globales <<< :
    for (i = (uint32_t)&__bss_start__; i < (uint32_t)&__bss_end__ ; i+= (uint32_t)&_SYSTABLES_PAGE_SIZE)
    {
        mapNewSmallPage(i, i, XN_ALLOWEXECUTION, PL1_RW);
    }

    return;
}
