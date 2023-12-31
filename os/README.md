# Task Scheduler Simple

Tomar el ejercicio anterior, y agregar dos tareas.

- **Tarea 1:** Deberá corroborar el correcto funcionamiento de la RAM (escribiendo y luego leyendo lo escrito) en el rango 0x70A000000 a 0x70A0FFFF, utilizando el word 0x55AA55AA. Debe resguardar la información antes de escribir cada bloque.

- **Tarea 2:** Deberá recorrer la RAM en el rango 0x70A10000 a 0x70A1FFFF. En cada porción de memoria, deberá hacer un toggle de lo leído, es decir, si lee un bit en 1 lo debe pasar a 0, y viceversa.

La administración de la ejecución de las tareas se realizará desde un scheduler que operará dentro de un timeframe de 100ms. Las tareas se ejecutarán durante 10ms antes de ser suspendidas hasta el próximo timeframe. Las tareas corren indefinidamente.

El tiempo remanente del timeframe lo ocupará una tercer tarea que ponga el procesador en un estado de bajo consumo de energía, hasta el inicio del próximo frame, o hasta que ocurra un evento asincrónico.