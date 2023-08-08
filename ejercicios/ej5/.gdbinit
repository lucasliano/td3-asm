dashboard memory -output /dev/pts/2

# GIC
# dashboard memory watch 0x00000000 16
# dashboard memory watch 0x70000000 16

# ROM
# dashboard memory watch 0x70010000 16

# STACK ( ~ _IRQ_STACK_TOP)
#dashboard memory watch 0x700202B0 96

# STACK ( ~ _SYS_STACK_TOP)
# dashboard memory watch 0x70020000 16
# dashboard memory watch 0x700205B0 96

# KERNEL CODE / RAM
#dashboard memory watch 0x70030000 64
# dashboard memory watch 0x70030000 16

# SYSTABLES
# L1
dashboard memory watch 0x70050000 4
dashboard memory watch 0x70051c00 4
dashboard memory watch 0x70050400 4
dashboard memory watch 0x70050780 4

# L2
dashboard memory watch 0x70054000 4
dashboard memory watch 0x70054400 4
dashboard memory watch 0x70054480 4
dashboard memory watch 0x700544c0 4
dashboard memory watch 0x70054540 64
dashboard memory watch 0x70054844 4
dashboard memory watch 0x70054c00 4
dashboard memory watch 0x70054c04 4


dashboard memory watch 0x71000ff0 16

# TASKS
# dashboard memory watch 0x70050000 96

# paginateIdentityMapping()
#b* 0x700304f8 

b* 0x70030188

#b* 0x700302f4


# Post MMU_Enable()
#b* 0x70030800


b* 0x00000004
b* 0x00000008
b* 0x0000000C
b* 0x00000010
b* 0x00000014
b* 0x00000018
b* 0x0000001C

#x/4x 0x70040000

dashboard expression watch ((long*)globalDebug)

dashboard expression watch ((long*)vma_global)

dashboard expression watch ((long*)L1Index_global)
dashboard expression watch ((long*)L2Index_global)

dashboard expression watch ((long*)L1Phy_global)
dashboard expression watch ((long*)L2Phy_global)

dashboard expression watch ((long*)L1Descriptor_global)
dashboard expression watch ((long*)L2Descriptor_global)

dashboard expression watch ((long*)debug_global)

dashboard memory watch 0x700540c0 16


# 70030540