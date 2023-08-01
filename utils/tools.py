import code


def read_bit(reg, bit):
    print(f'Reg: {reg:#034b}')  # Se pone 034b porque son 32 bits + 2 caracteres para el '0b'
    pos = 1 << bit
    print(f'Pos: {pos:#034b}')
    result = (reg >> bit) & 0b1
    print(f'Result: {result}')
    

def descriptor_L1(descriptor):
    print(f'Value: {descriptor:#034b}')
    print(f'DESC_TYPE               = {((descriptor)       &  0b11):#03b}')
    print(f'PXN                     = {((descriptor >> 2)  &  0b1):#02b}')
    print(f'NS                      = {((descriptor >> 3)  &  0b1):#02b}')
    print(f'SBZ                     = {((descriptor >> 4)  &  0b1):#02b}')
    print(f'DOMAIN                  = {((descriptor >> 5)  &  0xF):#05b}')
    print(f'IMPLEMENTATION_DEFINED  = {((descriptor >> 9)  &  0b1):#02b}')
    print(f'PAGETABLE_BASE_ADDRESS  = {hex((descriptor)    &  0xFFFFFC00)}')

def descriptor_L2(descriptor):
    print(f'Value: {descriptor:#034b}')
    print(f'XN                      = {((descriptor)       &  0b1):#02b}')
    print(f'DESC_TYPE               = {((descriptor >> 1)  &  0b1):#02b}')
    print(f'B                       = {((descriptor >> 2)  &  0b1):#02b}')
    print(f'C                       = {((descriptor >> 3)  &  0b1):#02b}')
    print(f'AP_10                   = {((descriptor >> 4)  &  0b11):#03b}')
    print(f'TEX                     = {((descriptor >> 6)  &  0b111):#04b}')
    print(f'AP_2                    = {((descriptor >> 9)  &  0b1):#02b}')
    print(f'S                       = {((descriptor >> 10) &  0b1):#02b}')
    print(f'NG                      = {((descriptor >> 11) &  0b1):#02b}')
    print(f'SMALLPAGE_BASE_ADDRESS  = {hex((descriptor)    &  0xFFFFFF00)}')

def vma2addr(vma):
    print(f'VMA: {hex(vma)}')

    index_l1 = (vma >> 20) & 0xFFF
    print(f'L1 - Index: {hex(index_l1)} - Offset: {hex(index_l1 * 4)}')

    index_l2 = (vma >> 12) & 0xFF
    print(f'L2 - Index: {hex(index_l2)} - Offset: {hex(index_l2 * 4)}')
    

def create_L1(baseAddrL2):
    DESC_TYPE               = (0b01     << 0)
    PXN                     = (0b0      << 2)
    NS                      = (0b0      << 3)
    SBZ                     = (0b0      << 4)
    DOMAIN                  = (0b0      << 5)
    IMPLEMENTATION_DEFINED  = (0b0      << 9)
    PAGETABLE_BASE_ADDRESS  = (baseAddrL2   &  0xFFFFFC00)

    final_descriptor = PAGETABLE_BASE_ADDRESS | IMPLEMENTATION_DEFINED | DOMAIN | SBZ | NS | PXN | DESC_TYPE
    print(f'New L1: {hex(final_descriptor)}')

def create_L2(basePhy):
    XN                      = (0b0      << 0)
    DESC_TYPE               = (0b1      << 1)
    B                       = (0b0      << 2)
    C                       = (0b0      << 3)
    AP_10                   = (0b01     << 4)
    TEX                     = (0b010    << 6)
    AP_2                    = (0b0      << 9)
    S                       = (0b0      << 10)
    NG                      = (0b0      << 11)
    SMALLPAGE_BASE_ADDRESS  = (basePhy &  0xFFFFFF00)

    final_descriptor = SMALLPAGE_BASE_ADDRESS | NG | S | AP_2 | TEX | AP_10 | C | B | DESC_TYPE | XN
    print(f'New L2: {hex(final_descriptor)}')



if __name__ == '__main__':
    code.interact(local=locals())