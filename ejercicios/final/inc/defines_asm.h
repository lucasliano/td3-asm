// Referencia: https://downloads.ti.com/docs/esd/SLAU131R/Content/SLAU131R_HTML/sharing-c-c-header-files-with-assembly-source.html

// --- Program Status Register --- //

.equ I_BIT, 7   // Bit para deshabilitar IRQ
.equ F_BIT, 6   // Bit para deshabilitar FIQ
.equ T_BIT, 5   // Bit para habilitar modo Thumb

.equ FIQ_MODE, 0b10001 @ mode = 0x11
.equ IRQ_MODE, 0b10010 @ mode = 0x12
.equ SVC_MODE, 0b10011 @ mode = 0x13
.equ ABT_MODE, 0b10111 @ mode = 0x17
.equ UND_MODE, 0b11011 @ mode = 0x1B
.equ SYS_MODE, 0b11111 @ mode = 0x1F
