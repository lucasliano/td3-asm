#ifndef __MMU_TOOLS_LIB_H
#define __MMU_TOOLS_LIB_H

#include <stdint.h>

// Defines para determinar valores de propiedades de los descriptores

// Descriptor types
#define INVALID_DESCRIPTOR 0b00
#define PAGETABLE 0b01
#define RESERVED 0b11
#define SECTION 0b1
#define LARGEPAGE 0b01
#define SMALLPAGE 0b1

// PAGE TABLES defines
#define NS_SECURE 0b1
#define NS_NONSECURE 0b0

#define XN_BLOCKEXECUTION 0b1
#define XN_ALLOWEXECUTION 0b0

#define S_SHAREABLE 0b1
#define S_NONSHAREABLE 0b0

#define NG_GLOBAL 0b0
#define NG_NOTGLOBAL 0b1


// TODO: Ver que son ENABLECP15BEN y WRITEALLOC
/**
 * @brief Enum para determinar si se usan o no las Data and instruction barrier operations
 * 
 */
typedef enum {
    CP15BEN_YES = 0b1,
    CP15BEN_NO = 0b0
} ENABLECP15BEN;

/**
 * @brief Enum para ayudar a la configuración de los bits TEX, C y B de los descriptores
 * 
 */
typedef enum {
    WA_YES,
    WA_NO,
    WA_NA
} WRITEALLOC;



/**
 * @brief Enum para ayudar a la configuración del registro DACR
 * 
 */
typedef enum {
    DOMAIN_NO_ACCESS = 0b00,
    DOMAIN_CLIENT = 0b01,
    DOMAIN_RESERVED = 0b10,
    DOMAIN_MANAGER = 0b11
} DOMAIN_AP;

/**
 * @brief Enum para determinar que privilegio tiene acceso a una página
 * 
 */
typedef enum {
    NO_ACCESS,
    PLX_R,
    PL0_RW,
    PL0_R,
    PL1_RW,
    PL1_R,
} AP_PLX;


/**
 * @brief Enum para determinar si los descriptores usan o no el Access Flag
 * 
 */
typedef enum {
    AF_YES = 0b1,
    AF_NO = 0b0
} ENABLEAF;

/**
 * @brief Enum para determinar si se usa o no TEX remap
 * 
 */
typedef enum {
    TEXREMAP_YES = 0b1,
    TEXREMAP_NO = 0b0
} ENABLETEXREMAP;

/**
 * @brief Enum para determinar si se usa o no la MMU
 * 
 */
typedef enum {
    MMUENABLE_YES = 0b1,
    MMUENABLE_NO = 0b0
} MMUENABLE;

/**
 * @brief Translation Table Base Control Register
 * 
 * Ver TRM B4.1.153 para un detalle de cada campo
 * 
 */
typedef union {
    uint32_t ttbcr;
    struct {
        uint32_t T0SZ : 3;
        uint32_t RESERVED0 : 1;
        uint32_t PD0 : 1;
        uint32_t PD1 : 1;
        uint32_t RESERVED1 : 1;
        uint32_t EPD0 : 1;
        uint32_t IRGN0 : 2;
        uint32_t ORGN0 : 2;
        uint32_t SH0 : 2;
        uint32_t RESERVED2 : 2;
        uint32_t T1SZ : 3;
        uint32_t RESERVED3 : 3;
        uint32_t A1 : 1;
        uint32_t EPD1 : 1;
        uint32_t IRGN1 : 2;
        uint32_t ORGN1 : 2;
        uint32_t SH1 : 2;
        uint32_t IDF : 1;
        uint32_t EAE : 1;
    };
} TTBCR;

/**
 * @brief Translation Table Base Register 0
 * 
 * Ver TRM B4.1.154 para un detalle de cada campo
 * 
 */
typedef union {
    uint32_t ttbr0;
    struct {
        uint32_t IRGN_1 : 1;
        uint32_t S : 1;
        uint32_t IMP : 1;
        uint32_t RGN : 2;
        uint32_t NOS : 1;
        uint32_t IRGN_2 : 1;
        uint32_t TTB0A : 25;
    };
} TTBR0;

/**
 * @brief System Control Register VMSA
 * 
 * Ver TRM B4.1.130 para un detalle de cada campo
 * 
 */
typedef union {
    uint32_t sctlr;
    struct {
        uint32_t M : 1;
        uint32_t A : 1;
        uint32_t C : 1; // Cache Enable
        uint32_t RESERVED0 : 2;
        uint32_t CP15BEN : 1;
        uint32_t RESERVED1 : 1;
        uint32_t B : 1;
        uint32_t RESERVED2 : 2;
        uint32_t SW : 1;
        uint32_t Z : 1; // Branch Prediction Enable
        uint32_t I : 1; // Instruction Cache Enable
        uint32_t V : 1;
        uint32_t RR : 1;
        uint32_t RESERVED3 : 2;
        uint32_t HA : 1;
        uint32_t RESERVED4 : 1;
        uint32_t WXN : 1;
        uint32_t UWXN : 1;
        uint32_t FI : 1;
        uint32_t U : 1;
        uint32_t RESERVED5 : 1;
        uint32_t VE : 1;
        uint32_t EE : 1;
        uint32_t RESERVED6 : 1;
        uint32_t NMFI : 1;
        uint32_t TRE : 1;
        uint32_t AFE : 1;
        uint32_t TE : 1;
        uint32_t RESERVED7 : 1;
    };
} SCTLR;

/**
 * @brief Domain Access Control Register, VMSA
 * 
 * Ver TRM B4.1.43 para un detalle de cada campo
 * 
 */
typedef union {
    uint32_t dacr;
    struct{
        uint32_t d0 : 2;
        uint32_t d1 : 2;
        uint32_t d2 : 2;
        uint32_t d3 : 2;
        uint32_t d4 : 2;
        uint32_t d5 : 2;
        uint32_t d6 : 2;
        uint32_t d7 : 2;
        uint32_t d8 : 2;
        uint32_t d9 : 2;
        uint32_t d10 : 2;
        uint32_t d11 : 2;
        uint32_t d12 : 2;
        uint32_t d13 : 2;
        uint32_t d14 : 2;
        uint32_t d15 : 2;
    };
} DACR;

/**
 * @brief Short-descriptor first-level descriptor for a page table entry
 * 
 * Ver TRM B3.5.1 para un detalle de cada campo
 * 
 */
typedef union {
    uint32_t descriptor;
    struct {
        uint32_t DESC_TYPE : 2;
        uint32_t PXN : 1;
        uint32_t NS : 1;
        uint32_t SBZ : 1;
        uint32_t DOMAIN : 4;
        uint32_t IMPLEMENTATION_DEFINED : 1;
        uint32_t PAGETABLE_BASE_ADDRESS : 22;
    };
} FIRSTLEVEL_PAGETABLE_DESCRIPTOR;

/**
 * @brief Short-descriptor second-level descriptor for a small page entry
 * 
 * Ver TRM B3.5.1 para un detalle de cada campo
 * 
 */
typedef union {
    uint32_t descriptor;
    struct {
        uint32_t XN : 1;
        uint32_t DESC_TYPE : 1;
        uint32_t B : 1;
        uint32_t C : 1;
        uint32_t AP_10 : 2;
        uint32_t TEX : 3;
        uint32_t AP_2 : 1;
        uint32_t S : 1;
        uint32_t NG : 1;
        uint32_t SMALLPAGE_BASE_ADDRESS : 20;
    };
} SECONDLEVEL_SMALLPAGE_DESCRIPTOR;




// --- Translation Table Base Control Register ---
TTBCR MMU_Get_TTBCR(void);
void MMU_Set_TTBCR(TTBCR);

// --- Translation Table Base Register ---
TTBR0 MMU_Get_TTBR0(void);
void MMU_Set_TTBR0(TTBR0);

// --- System Control Register ---
SCTLR MMU_Get_SCTLR(void);
void MMU_Set_SCTLR(SCTLR);

// --- Domain Access Control Register ---
DACR MMU_Get_DACR(void);
void MMU_Set_DACR(DACR);


// >>> Funciones operativas <<<
void MMU_Enable();
void MMU_Disable();
void MMU_Invalidate_TLB(void);

#endif // __MMU_TOOLS_LIB_H