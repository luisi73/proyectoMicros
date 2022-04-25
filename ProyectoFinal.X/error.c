#include <xc.h>
#include <stdio.h>
#include <string.h>
#include "error.h"

// ============ FUNCIONES PARA MANEJAR LOS ERRORES DEL PIN =======
void setErrorCounter(int counter)
{
    asm("   di");
    error_counter = counter;
    asm("   ei");
}

void plusErrorCounter(int add_num)
{
    asm("   di");
    error_counter += add_num;
    asm("   ei");
}

int getErrorCounter(void)
{
    int c_error_counter;
    asm("   di");
    c_error_counter = error_counter;
    asm("   ei");
    return c_error_counter;
}