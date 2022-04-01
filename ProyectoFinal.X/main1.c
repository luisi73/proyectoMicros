/*
Para ello puede usar la función putsUART y enviar una cadena con un sólo carácter o bien crear
una función, denominada por ejemplo putcUART que introduzca un sólo carácter en la cola y active las
interrupciones para que se envíe.
*/

#include <xc.h> 
#include "Pic32Ini.h"
#include "UART1.h"

#define BAUDIOS 9600

int main(void)
{
    InicializarUART1(BAUDIOS);
    InicializarReloj();

    INTCONbits.MVEC = 1; 
    asm("   ei");

    //Generamos el caracter 
    char s[2];
    s[1] = '\0';

    while(1)
    {
        //Recepcion + Emision 
        s[0] = getcUART();
        if (s[0]!='\0')
            putsUART(s);
    }

    return 0; 
}