set radix 16
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

# # SYSTABLES
# # L1
# dashboard memory watch 0x70050000 4
# dashboard memory watch 0x70051c00 4
# dashboard memory watch 0x70050400 4
# dashboard memory watch 0x70050780 4

# # L2
# dashboard memory watch 0x70054000 4
# dashboard memory watch 0x70054400 4
# dashboard memory watch 0x70054480 4
# dashboard memory watch 0x700544c0 4
# dashboard memory watch 0x70054540 64
# dashboard memory watch 0x70054844 4
# dashboard memory watch 0x70054c00 4
# dashboard memory watch 0x70054c04 4


# dashboard memory watch 0x71000ff0 16



# dashboard memory watch 0x70F02aaa-0x10 16
dashboard memory watch 0x70F05554-0x40 64
# dashboard memory watch 0x70F07ffe-0x10 16
# dashboard memory watch 0x70F0aaa8-0x10 16
# dashboard memory watch 0x70F0d552-0x10 16
# dashboard memory watch 0x70F0fffc-0x10 16

# dashboard memory watch 0x60002aaa-0x10 16
# dashboard memory watch 0x60005554-0x40 64
# dashboard memory watch 0x60007ffe-0x10 16
# dashboard memory watch 0x6000aaa8-0x10 16
# dashboard memory watch 0x6000d552-0x10 16
# dashboard memory watch 0x6000fffc-0x10 16

# dashboard memory watch 0x71112aaa-0x40 64
dashboard memory watch 0x71115554-0x40 64
# dashboard memory watch 0x71117ffe-0x40 64
dashboard memory watch 0x7111aaa8-0x40 64
# dashboard memory watch 0x7111d552-0x40 64
# dashboard memory watch 0x7111fffc-0x40 64


dashboard memory watch 0x50000000 16

dashboard memory watch 0x80000000 16
dashboard memory watch 0x90000000 16
dashboard memory watch 0x90001000 16
dashboard memory watch 0x9000F000 16

# Debug breakpoint
# b* 0x7003023c

# debugTask
# b* 0x70030234



# b* 0x700302fc

# break kernel_handlers.c:45
# break mmu_primitives.c:258


b* 0x80000000
# b* 0x8000000c

# Exception breakpoints
# b* 0x00000004
b* 0x00000008
# b* 0x0000000C
# b* 0x00000010
# b* 0x00000014
b* 0x00000018
# b* 0x0000001C

#x/4x 0x70040000

# dashboard expression watch ((long*)vma_global)
# dashboard expression watch ((long*)TTBR0_global)
# dashboard expression watch ((long*)L1Index_global)
# dashboard expression watch ((long*)L1Phy_global)
# dashboard expression watch ((long*)L2BasePhy_global)
# dashboard expression watch ((long*)L2Index_global)
# dashboard expression watch ((long*)L2Phy_global)
# dashboard expression watch ((long*)L1Descriptor_global)
# dashboard expression watch ((long*)L2Descriptor_global)


# dashboard expression watch ((long*)taskSP_FIQ)
# dashboard expression watch ((long*)taskSP_IRQ)
# dashboard expression watch ((long*)taskSP_SVC)
# dashboard expression watch ((long*)taskSP_ABT)
# dashboard expression watch ((long*)taskSP_UND)
# dashboard expression watch ((long*)taskSP_SYS)

# dashboard expression watch ((long*)taskPID)
# dashboard expression watch ((long*)taskticks)
# dashboard expression watch ((long*)taskisActive)
# dashboard expression watch ((long*)taskprivilege)
# dashboard expression watch ((long*)taskTTBR0)
# dashboard expression watch ((long*)taskL2TablesCount)
# dashboard expression watch ((long*)taskBaseStackPhy)
# dashboard expression watch ((long*)taskCodeBasePhy)


dashboard expression watch ((long*)currTask)
dashboard expression watch ((long*)timeframeCounter)
dashboard expression watch ((long*)currentTaskCounter)
