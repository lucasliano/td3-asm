#include "../inc/kernel.h"


// --- Externs ---
// Regiones del .ld
extern int _GIC_PHY;
extern int _GIC_SIZE;
extern int _HARDWARE_REGISTERS_INIT;
extern int _HARDWARE_REGISTERS_END;
extern int _KERNEL_CODE_PHY;
extern int _KERNEL_CODE_SIZE;
extern int _DATA_START;
extern int _DATA_END;
extern int _BSS_START;
extern int _BSS_END;
extern int _KERNEL_STACKS_PHY;
extern int _KERNEL_STACKS_END;

// Definiciones del .ld
extern int _SYSTABLES_PAGE_SIZE;

// Variable del kernel
extern TCB_t taskVector[16];


// NOTE: Solo para facilitar el debugging
uint32_t vma_global = 0;
uint32_t TTBR0_global = 0;
uint32_t L1Index_global = 0;
uint32_t L1Phy_global = 0;
uint32_t L2BasePhy_global = 0;
uint32_t L2Index_global = 0;
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

uint32_t getNewSecondLevelAddress(uint32_t taskIndex)
{
    //==============================================================================//
    // La siguiente función averigua la posición de la siguiente tabla de L2 para
    // una determinada tarea. 
    //==============================================================================//
    uint32_t address;
    
    address = (taskVector[taskIndex].TTBR0 + 0x4000);       // Calculo la dirección base de la 1er L2
    address += 0x400 * taskVector[taskIndex].L2TablesCount; // Le sumo un offset por cada L2 asignada antes
    
    
    // FIXME: Checkear que no superemos _SYSTABLES_MAX_L2
    taskVector[taskIndex].L2TablesCount += 1;
    
    initSecondLevelTable((uint32_t*) address);
    
    return address;
}

void mapNewSmallPage(
    uint32_t taskIndex,
    uint32_t vmaAddr,
    uint32_t phyAddr,
    uint8_t nonExecute,
    uint8_t privilage
){
    //==============================================================================//
    // La siguiente función agrega inteligentemente una nueva página al esquema de
    // paginación existente. Debe tenerse en cuenta que la MMU debe estar apagada
    // para poder crear una nueva página.
    //==============================================================================//
    uint32_t L1Index, L2Index;
    uint32_t *phyL1BaseAddr, *phyL2BaseAddr;
    FIRSTLEVEL_PAGETABLE_DESCRIPTOR firstLevelDescriptor;
    SECONDLEVEL_SMALLPAGE_DESCRIPTOR secondLevelDescriptor;    

    //------------------------------------------------------------------------------//
    // Extraigo los indices de L1/L2 y el offset de la VMA
    //------------------------------------------------------------------------------//
    L1Index = (vmaAddr >> 20) & 0xFFF;  //      Saco 0xAAA-_----
    L2Index = (vmaAddr >> 12) & 0xFF;   //      Saco 0x---B_B---
                                        // No se usa 0x----_-CCC.

    //------------------------------------------------------------------------------//
    // >>> Verificamos el descriptor de la tabla L1 <<<
    //------------------------------------------------------------------------------//
    phyL1BaseAddr = (uint32_t*) taskVector[taskIndex].TTBR0;
    firstLevelDescriptor.descriptor = (uint32_t) phyL1BaseAddr[L1Index];
    if (firstLevelDescriptor.DESC_TYPE != PAGETABLE)
    {
        // Si no existe descriptor en la L1, la creamos
        phyL2BaseAddr = (uint32_t*) getNewSecondLevelAddress(taskIndex);

        firstLevelDescriptor.DESC_TYPE = PAGETABLE;                         // Descriptor Type
        firstLevelDescriptor.PXN = 0;                                       // Privileged eXecute Never
        firstLevelDescriptor.NS = NS_NONSECURE;                             // Non Secure
        firstLevelDescriptor.SBZ = 0;                                       // Should Be Zero
        firstLevelDescriptor.DOMAIN = 0;                                    // Domain (Son todos manager)
        firstLevelDescriptor.IMPLEMENTATION_DEFINED = 0;                    // Implementation Defined
        
        // Base Addr
        firstLevelDescriptor.descriptor = ((uint32_t) phyL2BaseAddr & 0xFFFFFC00) | firstLevelDescriptor.descriptor;
        
        // Guardamos el descriptor en la tabla
        phyL1BaseAddr[L1Index] = (uint32_t) firstLevelDescriptor.descriptor;
        
    }else{
        // Si existe desacriptor en la L1, sacamos la posición de la L2
        phyL2BaseAddr = (uint32_t*) (firstLevelDescriptor.descriptor & 0xFFFFFC00);        
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
    phyL2BaseAddr[L2Index] = (uint32_t) secondLevelDescriptor.descriptor;

    // FIXME: Remove, just for debug
    vma_global = vmaAddr;
    TTBR0_global = (uint32_t) phyL1BaseAddr;
    L2BasePhy_global = (uint32_t) phyL2BaseAddr;
    L1Index_global = L1Index;
    L2Index_global = L2Index;
    L1Phy_global =  ((uint32_t) phyL1BaseAddr + (L1Index*4));
    L2Phy_global =  ((uint32_t) phyL2BaseAddr + (L2Index*4));
    L1Descriptor_global = (uint32_t) (firstLevelDescriptor.descriptor);
    L2Descriptor_global = (uint32_t) (secondLevelDescriptor.descriptor);

    return;
}

// NOTE: No aclaro __attribute__((section(".text"))) porque es la región por defecto.
void paginateIdentityMapping(uint32_t taskIndex)
{
    //===================================================================================//
    // Esta función pagina toda la región de identity mapping a partir de conocer TTBR0
    //===================================================================================//
    uint32_t i;

    // Creo la tabla de primer nivel
    initFirstLevelTable((uint32_t*) taskVector[taskIndex].TTBR0);    // Limpio la región de memoria donde va la L1


    // === Comienza la creación de tablas ===
    // --- GIC ---
    for (i = 0x0; i < 0x0 + (uint32_t)&_GIC_SIZE; i+= (uint32_t)&_SYSTABLES_PAGE_SIZE)
    {
        mapNewSmallPage(
            taskIndex,              // Tarea para la cual estamos creando el mapa
            i,                      // VMA
            i,                      // PHY
            XN_ALLOWEXECUTION,      // Permiso de ejecución
            PL1_RW                  // Privilegios de acceso
        );

    }

    for (i = (uint32_t)&_GIC_PHY; i < (uint32_t)&_GIC_PHY + (uint32_t)&_GIC_SIZE; i+= (uint32_t)&_SYSTABLES_PAGE_SIZE)
    {
        mapNewSmallPage(taskIndex, i, i, XN_ALLOWEXECUTION, PL1_RW);
    }

    // --- Kernel ---
    for (i = (uint32_t)&_KERNEL_CODE_PHY; i < (uint32_t)&_KERNEL_CODE_PHY + ((uint32_t)&_KERNEL_CODE_SIZE); i+= (uint32_t)&_SYSTABLES_PAGE_SIZE)
    {
        mapNewSmallPage(taskIndex, i, i, XN_ALLOWEXECUTION, PL1_RW);
    }
 
    // --- Hardware Registers ---
    // NOTE: La manera correcta sería mappear todo el bus APB y AHB.
    //       Haciendo esto tendríamos acceso a todos los posibles registros.
    //       Pero eso sería mappear desde 0x1000_0000 hasta 0x2000_0000
    //       que es equivalente a 256 tablas L2.  
    // for (i = (uint32_t)&_HARDWARE_REGISTERS_INIT; i < (uint32_t)&_HARDWARE_REGISTERS_END; i+= (uint32_t)&_SYSTABLES_PAGE_SIZE)
    // {
    //     mapNewSmallPage(taskIndex, i, i, XN_ALLOWEXECUTION, PL1_RW);
    // }

    // NOTE: Para evitar esto solo vamos a mappear los registros que utilizamos
    //       Lo que nos va a ocupar solo 2 páginas
    mapNewSmallPage(taskIndex, TIMER0_ADDR, TIMER0_ADDR, XN_ALLOWEXECUTION, PL1_RW);   // 0x100-_----
    mapNewSmallPage(taskIndex, GICC0_ADDR, GICC0_ADDR, XN_ALLOWEXECUTION, PL1_RW);     // 0x1E0-_----


    // >>> Variables globales <<< :
    for (i = (uint32_t)&_DATA_START; i < (uint32_t)&_DATA_END ; i+= (uint32_t)&_SYSTABLES_PAGE_SIZE)
    {
        mapNewSmallPage(taskIndex, i, i, XN_ALLOWEXECUTION, PL1_RW);
    }

    for (i = (uint32_t)&_BSS_START; i < (uint32_t)&_BSS_END ; i+= (uint32_t)&_SYSTABLES_PAGE_SIZE)
    {
        mapNewSmallPage(taskIndex, i, i, XN_ALLOWEXECUTION, PL1_RW);
    }


    // --- Kernel Stack ---
    for (i = (uint32_t)&_KERNEL_STACKS_PHY; i < (uint32_t)&_KERNEL_STACKS_END ; i+= (uint32_t)&_SYSTABLES_PAGE_SIZE)
    {
        mapNewSmallPage(taskIndex, i, i, XN_ALLOWEXECUTION, PL1_RW);
    }

    return;
}
