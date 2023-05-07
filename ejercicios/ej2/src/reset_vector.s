.code 32
/*--- Variables globales --- */
.global _reset_vector

/*--- Variables importadas --- */
.extern _start              @ Indica al linker donde arranca el codigo

/*--- Arranca la sección --- */
.section .reset_vector_code
_reset_vector:
    B _start


.end
