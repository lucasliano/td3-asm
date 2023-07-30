import code


def read_bit(reg, bit):
    print(f'Reg: {reg:#034b}')  # Se pone 034b porque son 32 bits + 2 caracteres para el '0b'
    pos = 1 << bit
    print(f'Pos: {pos:#034b}')
    result = (reg >> bit) & 0b1
    print(f'Result: {result}')
    

def descriptorL1(descriptor):
    print(f'Value: {descriptor:#034b}')
    print(f'DESC_TYPE               = {((descriptor)       &  0b11):#03b}')
    print(f'PXN                     = {((descriptor >> 2)  &  0b1):#02b}')
    print(f'NS                      = {((descriptor >> 3)  &  0b1):#02b}')
    print(f'SBZ                     = {((descriptor >> 4)  &  0b1):#02b}')
    print(f'DOMAIN                  = {((descriptor >> 5)  &  0xF):#05b}')
    print(f'IMPLEMENTATION_DEFINED  = {((descriptor >> 9)  &  0b1):#02b}')
    print(f'PAGETABLE_BASE_ADDRESS  = {hex((descriptor)    &  0xFFFFFC00)}')

def descriptorL2(descriptor):
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

if __name__ == '__main__':
    code.interact(local=locals())