int suma_c (int, int);
int mult_c (int, int);

__attribute__((section(".text"))) int suma_c(int a, int b){
    int c;
    c = a + b;
    return c;
}

__attribute__((section(".text"))) int mult_c(int a, int b){
    int c;
    c = a * b;
    return c;
}
