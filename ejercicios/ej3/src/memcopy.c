void memcopy(const void *origen, void *destino, unsigned int num_bytes);


__attribute__((section(".start_code"))) void memcopy(const void *origen, void *destino, unsigned int num_bytes)
{
    int* source  = (int*) origen;
    int* dest = (int*) destino;
    unsigned int n = num_bytes;

    unsigned int i;

    for (i = 0; i < n; i++)
    {
        dest[i] = source[i]; 
    }
}
