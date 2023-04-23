# Consigna

Se desea escribir un programa para simular en QEMU, que en el vector de arranque pueda copiarse a si mismo a la dirección 0x70030000. Para tal fin implementar:

```C
void *td3_memcopy(void *destino, const void *origen, unsigned int num_bytes);
```

Una vez que se copia a esa dirección debe saltar a una etiqueta __end__, en donde se espera una instrucción que frene la ejecución del procesador.

La pila debe ubicarse en 0x70020000.

Se sugiere utilizar un archivo de linker script para poder gestionar las áreas de memoria durante el ensamblado del binario.

## Objetivos conceptuales

1. Familiarizarse con los comandos de QEMU y su utilización
2. Familiarizarse con el Linker Script
3. Entender el concepto de ABI para poder utilizar convenientemente C y ASM, aprovechando sus ventajas para cada caso.
4. Entender manejo de pila __R13__
5. Entender como cargar el registro __LR__ para poder retornar de las funciones.
