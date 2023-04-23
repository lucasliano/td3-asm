#define NULL ((void*)0)

void *td3_memcopy(const void *origen, void *destino, unsigned int num_bytes);


__attribute__((section(".text"))) void *td3_memcopy(const void *origen, void *destino, unsigned int num_bytes)
{
    // FIXME: Ver si hacen falta las primeras 3 líneas.
    int* source  = (int*) origen;
    int* dest = (int*) destino;
    unsigned int n = num_bytes;

    unsigned int i;

    for (i = 0; i < n; i++)
    {
        dest[i] = source[i]; 
    }
    return NULL;
}
