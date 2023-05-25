# Consigna

Se desea continuar el ejercicio anterior e incorporar la atención de interrupciones/excepciones.

Para tal fin, se deberá definir un handler genérico para atender la interrupción __SVC__. 

Para corroborar su funcionamiento, debemos poder llamar a la instrucción __SWI__ y validar que atendemos el request correctamente.

## Objetivos conceptuales

1. Familiarizarse con la inicialización de interrupciones/excepciones.
2. Entender el concepto de servicio de sistema operativo, que luego será usado por el código de usuario para acceder a acciones privilegiadas, como hace un S.O. real.