# Consigna

Se requiere continuar el ejercicio anterior habilitando únicamente la IRQ asociada al temporizador y la atención de excepciones de los siguientes tipos:

* invalid instructions and trap exceptions
* Memory accesses
* Exception-generating instructions-> Supervisor Call (SVC)

Cada rutina de atención de excepción deberá almacenar en __r10__ los caracteres del tipo que la generó ('INV', 'MEM', 'SVC'), y permitir que el procesador pueda continuar con la ejecución normal de código.

Cada excepción se debe generar por una función específica. Se sugiere que la ejecución de dichas funciones pueda des/habilitarse mediante una opción de construcción del binario, como por ejemplo _make exceptions_.

Se debe configurar el TIMER0 de forma que gener una interrupción cada 10ms y la rutina de atención a la IRQ asociada (#36) debe almacenar en __r10__ la cantidad de veces que fue invocada. Recordar configurar adecuadamente el GIC0.

Una vez finalizada la configuración de todos los recursos de HW descriptos y generadas las excepciones, según la metodología de construcción del binario escogida, el código debe quedarse en un bucle que suspenda la ejecución (detener el reloj principal del core) hasta recibir un evento IRQ.

## Objetivos conceptuales

1. Familiarizarse con IRQ vs FIQ.
2. Comprende rla atención de excepciones.
3. Comprender la importancia de poner el procesador en alta impedancia para reducir el consumo de energía mientras esperamos por eventos.