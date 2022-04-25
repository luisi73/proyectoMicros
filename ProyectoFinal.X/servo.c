#include <xc.h>
#include <stdio.h>
#include <string.h>
#include "servo.h"

void abrirPuerta(void)
{
    int t_alto = 5000; // Tiempo en alto de la salida (1 ms)

    OC1RS = t_alto;
}

void cerrarPuerta(void)
{
    int t_alto = 2500; // Tiempo en alto de la salida (1 ms)

    OC1RS = t_alto;
}
