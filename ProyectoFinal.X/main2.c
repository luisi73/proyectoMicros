/*Una vez comprobado que el módulo de la UART funciona correctamente, 
escribimos un programa para poder controlar los pines de la tarjeta desde el PC 
o el móvil. Para realizar el programa, en el bucle de scan se llamaráa la función 
getcUART y se irán copiando los caracteres que se vayan recibiendo a una cadena. 

Cuando se reciba el \n se analizarála cadena para interpretar el comando y ejecutarlo, 
devolviendo con putsUART el resultado de dicho comando. */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <xc.h>
#include <unistd.h>
#include <string.h>
#include "Pic32Ini.h"
#include "UART1.h"
#include "I2C.h"

#define INC_90_GRADOS 2502 // cada 139 cuentas se giran 10�
#define BAUDIOS 9600
#define PIN_PULSADOR 5

int main (void)
{
    int pulsador_ant, pulsador_act;
    int t_alto = 2500; // Tiempo en alto de la salida (1 ms)

    InicializarPines(BAUDIOS);
    
    INTCONbits .MVEC = 1; // Modo multivector
    asm("ei"); // Interr. Habilitadas
    //El tamaño del comando mas largo es de 9 caracteres
    char charmander[10], caracter;
    int i = 0; 

    pulsador_ant = (PORTB>>PIN_PULSADOR) & 1;
    while(1)
    {
        //Recepcion 
        caracter = getcUART();

        if(caracter != '\0')
        {
            //Vamos asignando las letras 
            charmander[i] = caracter;
            i++;
        }

        if (charmander[i-1]=='\n')
        {
            //Fin del comando, pasamos a interpretar el resultado
            i = 0; 
            //verif(charmander);
            verif(charmander);
            checkPasswordSystem(charmander);
        }
        
        
        // Se lee el estado del pulsador
        pulsador_act = (PORTB>>PIN_PULSADOR) & 1;
        if( (pulsador_act!=pulsador_ant) && (pulsador_act==0) ){
            // Flanco de bajada en la patilla del pulsador
            t_alto += INC_90_GRADOS;
            if(t_alto>5900){
                t_alto = 2500;
            }
            OC1RS = t_alto;
        }
         pulsador_ant = pulsador_act;
    }
    return 0; 
}