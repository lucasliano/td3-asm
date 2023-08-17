#ifndef __ASM_UTILS_H
#define __ASM_UTILS_H

#include <stdint.h>

#define reserved_bits(x,y,z) uint8_t reserved##x[ z - y + 1 ];

#define TRUE (uint32_t) 1
#define FALSE (uint32_t) 0


#define I_BIT 7   // Bit para deshabilitar IRQ
#define F_BIT 6   // Bit para deshabilitar FIQ
#define T_BIT 5   // Bit para habilitar modo Thumb

#define FIQ_MODE 0b10001 // mode = 0x11
#define IRQ_MODE 0b10010 // mode = 0x12
#define SVC_MODE 0b10011 // mode = 0x13
#define ABT_MODE 0b10111 // mode = 0x17
#define UND_MODE 0b11011 // mode = 0x1B
#define SYS_MODE 0b11111 // mode = 0x1F
#define USR_MODE 0b10000 // mode = 0x10


typedef union
{
    uint32_t x_psr;
    struct 
        {
            uint32_t M : 5;
            uint32_t T : 1;
            uint32_t F : 1;
            uint32_t I : 1;
            uint32_t A : 1;
            uint32_t E : 1;
            uint32_t IT_7_2 : 6;
            uint32_t GE : 4;
            uint32_t RESERVED1 : 4;
            uint32_t J : 1;
            uint32_t IT_1_0 : 2;
            uint32_t Q : 1;
            uint32_t V : 1;
            uint32_t C : 1;
            uint32_t Z : 1;
            uint32_t N : 1;
        };
} xPSR;

extern void _irq_enable(void);
extern void _irq_disable(void);
extern void _change_mode (uint32_t mode);


extern void _WRITE_8(uint32_t , uint8_t );
extern void _WRITE_16(uint32_t , uint16_t );
extern void _WRITE_32(uint32_t , uint32_t );
extern uint8_t _READ_8(uint32_t );
extern uint16_t _READ_16(uint32_t );
extern uint32_t _READ_32(uint32_t );

extern uint32_t _READ_SP( void );
extern void _WRITE_SP( uint32_t newsp);

#endif /* defined(__ASM_UTILS_H) */