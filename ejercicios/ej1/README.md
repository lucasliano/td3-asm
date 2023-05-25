# Consigna

Se desea escribir un programa para simular en QEMU, que en el vector de arranque pueda copiarse a si mismo a la dirección 0x70030000. Para tal fin implementar:

```C
void *td3_memcopy(void *destino, const void *origen, unsigned int num_bytes);
```

Una vez que se copia esta sección, el programa en el vector de arranque debe saltar al kernel (sección copiada) y debe quedar en un ciclo infinito.

La pila debe ubicarse en 0x70020000.

Las dos secciones de código deben quedar ensambladas una atrás de la otra. Para tal fin se sugiere utilizar un archivo de linker script de modo de gestionar las áreas de memoria durante el ensamblado del binario.