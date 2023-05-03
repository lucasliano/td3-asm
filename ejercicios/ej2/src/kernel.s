.code 32
.global _main
.section .kernel_test


_main:
    LDR R0, =.
    ADD R1, R1, #1
    BX  R0

.end
