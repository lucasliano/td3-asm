.code 32                    @ Arranca el código en modo ARM (32b)
.extern _start              @ Indica al linker donde arranca el codigo
.section .reset_vector_code

_reset_vector:
    B _start


.end