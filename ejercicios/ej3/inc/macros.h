#ifndef __MACROS_LIB_H
#define __MACROS_LIB_H

#define reserved_bits(x,y,z) uint8_t reserved##x[ z - y + 1 ];

#endif // __MACROS_LIB_H
