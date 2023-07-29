dashboard memory -output /dev/pts/2

# GIC
dashboard memory watch 0x70000000 16

# ROM
dashboard memory watch 0x70010000 16

# STACK ( ~ _IRQ_STACK_TOP)
#dashboard memory watch 0x700202B0 96

# STACK ( ~ _SYS_STACK_TOP)
dashboard memory watch 0x700205B0 96

# KERNEL CODE / RAM
dashboard memory watch 0x70030000 64

# SYSTABLES
dashboard memory watch 0x70040000 96

# TASKS
dashboard memory watch 0x70050000 96

b* 0x70030748

b* 0x00000004
b* 0x00000008
b* 0x0000000C
b* 0x00000010
b* 0x00000014
b* 0x00000018
b* 0x0000001C

#x/12x 0x70000000
#dashboard expression watch ((long*)0x70000000)[0]@4

