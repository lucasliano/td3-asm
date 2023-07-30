dashboard memory -output /dev/pts/2

# GIC
dashboard memory watch 0x00000000 16

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

# L2
dashboard memory watch 0x70055000 96

# TASKS
# dashboard memory watch 0x70050000 96

# paginateIdentityMapping()
b* 0x700304f8 

#b* 0x70030284
b* 0x70030260


# Post MMU_Enable()
b* 0x70030810

#TODO: Esta buggeado cuando agregas más pags

b* 0x00000004
b* 0x00000008
b* 0x0000000C
b* 0x00000010
b* 0x00000014
b* 0x00000018
b* 0x0000001C

#x/4x 0x70040000

dashboard expression watch ((long*)globalVariable)

