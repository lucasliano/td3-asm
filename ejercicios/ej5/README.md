# Repositorio para trabajar con MMU de Cortex A8

Los archivos que les paso cumplen varias funciones:
- Facilitarles código para trabajar,
- Organizarles un poco el código,
- Darles el puntapie inicial a fin de que puedan ver de donde leer para continuar.

Van a ver que son funciones para `leer` y `escribir` registros necesarios para la MMU.

Una pequeña ayudita más: 


```c
// Variables para configuración de MMU
TTBCR ttbcr;
TTBR0 ttbr0;
SCTLR sctlr;
DACR dacr;
uint32_t vbar, i;

uint32_t pa;
uint32_t sp;

// Esta lectura, limpia las variables
ttbcr = MMU_Get_TTBCR();
sctlr = MMU_Get_SCTLR();
ttbr0 = MMU_Get_TTBR0();
dacr = MMU_Get_DACR();
vbar = MMU_Get_VBAR();

//------------------------------------------------------------------------------//
// Inicializamos el GIC y las interrupciones deseadas
//------------------------------------------------------------------------------//

//------------------------------------------------------------------------------//
// Inicializamos y configuramos el Timer
//------------------------------------------------------------------------------//

//------------------------------------------------------------------------------//
// Copiamos nuestro código a las zonas de memoria requeridas
//------------------------------------------------------------------------------//

//------------------------------------------------------------------------------//
// Configuramos nuestro mapa de memoria
//------------------------------------------------------------------------------//
// Configuramos el Domain Manager Control Register - Por ahora, todo en Manager domain:
dacr.dacr = 0xFFFFFFFF;
MMU_Set_DACR(dacr);

// Configuramos el TTBCR. Usamos la descripción como "short descriptor" por ende, si bien la estructura
// dice T0SZ, por compatiblidad con el modo largo, esos mismos bits representan a N en el modo corto. Así
// lo ponemos en cero para que TTBR0 pueda acceder a toda la memoria direccionable disponible (4 GB)
ttbcr.T0SZ = 0;
MMU_Set_TTBCR(ttbcr);

// Configuramos el SCTLR
sctlr.TRE = 0;
sctlr.C = 0;
sctlr.I = 0;
sctlr.Z = 0;
sctlr.AFE = AF_NO;
MMU_Set_SCTLR(sctlr);

// Invalidamos la TLB, según documentación
MMU_Invalidate_TLB();
```


Luego de esto, deberían setear el primer nivel de tabla de traducción, algo así:

```c
// Ahora, empezamos a armar el mapa de memoria del Kernel
// Para eso, colocamos la TTBR0 del kernel, que como es el primero, lo tomamos como la primer dirección física disponible para la tabla de sistemas
MMU_Mi_Funcion_Que_Setea_La_FirstLevelTranslation((uint32_t)&__SYSTABLES_PHYSICAL);
        
```

Dónde pasamos por ejemplo, la dirección física de dónde comienza nuestra tabla de primer nivel. 

Luego, comienza la tarea de completar las tablas de segundo nivel dónde se mapean las páginas. 

Finalmente, no olviden que para que el procesador (una vez `activada la MMU`) pueda seguir accediendo a los periféricos esa región de memoria, debe estar mapeada por la MMU :). Vean el mapa de memoria de la plataforma que estamos emulando y definan esa región. 