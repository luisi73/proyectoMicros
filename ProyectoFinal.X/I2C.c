
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <xc.h>
#include <unistd.h>
#include <string.h>
#include "I2C.h"


//Antes de arrancar cualquier tipo de comunicación, 
//debemos de habilitar el modulo I2C del micro-controlador. 
//Para ello basta con poner a 1 el bit 15 (ON) del registro 

//Si usamos el bus a 100 kHz, es necesario poner a uno el bit 9 
//Para inhabilitar el slew rate. ????????? Que es eso??


void I2C1Inicializa(int velocidad)
{
    if(velocidad == 1) //Velocidad Rapida 400 kb/s
    {
        //Calculos Respectivos: (1/2*f_scl -104 nanos)*5 MHz -2 donde f_scl = 400 *10^3
        I2C1BRG = 4; //3.73 -> 4
        I2C1CON = 0X8000; //I2C ON, con control de slew rate 100kb/s
    }
    else //Velocidad estandar de 100 kb/s
    {
        //Calculos Respectivos:  (1/2*f_scl -104 nanos)*5 MHz -2 donde f_scl = 100 *10^3
        I2C1BRG = 23; //22.48
        I2C1CON = 0X8200; //I2C ON, sin control de slew rate
    }

    IFS1bits.I2C1MIF = 0;//Borramos el flag por si acaos 
}


/*Debemos de generar primero una condición de start en el bus, 
lo cual se hace simplemente poniendo a 1 el bit 0 (SEN) del registro 
I2CxCON*/

/*Al acabarse de envíar dicha condición, el módulo generará una interrupción, 
por lo que si estamos manejando el módulo mediante polling, basta cone esperar a que el 
I2CXMIF se ponga a 1. */

void I2C1GenerStart(void)
{
    I2C1CONbits.SEN = 1; //Genera la condición de start en el bus 
    while(IFS1bits.I2C1MIF == 0); // Espera al final de la condición 
    IFS1bits.I2C1MIF = 0; //Borramos el flag
}


int I2C1EscribeByte(uint8_t dato)
{
	I2C1TRN = dato; //Metemos el byte en el reg. apropiado
	while(IFS1bits.I2C1MIF == 0);
	IFS1bits.I2C1MIF=0; //Borro el flag 

	return I2C1STATbits.ACKSTAT; //Devolver el estado
}


uint8_t I2C1LeeByte(int ack) { 
	// Lectura
	uint8_t dato; 
	I2C1CONbits.RCEN = 1; // Se activa la recepción del dato (reloj)
	while(IFS1bits.I2C1MIF==0);// Espera el final de la recepción 
	IFS1bits.I2C1MIF = 0; // Borro el flag
	dato = I2C1RCV;
	
	// Envio ACK
	I2C1CONbits.ACKDT = ack&1; // Bit 5 del I2CxCON
	I2C1CONbits.ACKEN = 1; // Se envía el ACK 
	while(IFS1bits.I2C1MIF==0);// Espera el envío del ACK 
	IFS1bits.I2C1MIF = 0; // Borro el flag
	
return dato;
}

void I2C1GeneraReStart(void) {
	I2C1CONbits.RSEN = 1; // Genera condición de repeated
	// start en el bus
	while(IFS1bits.I2C1MIF==0); // Espera el final de la condición
	IFS1bits.I2C1MIF = 0; // Borro el flag
}

void I2C1GeneraStop(void) { 
	I2C1CONbits.PEN = 1; // Genera condición de stop en el bus
	while(IFS1bits.I2C1MIF==0) ; // Espera el final de la condición 
	IFS1bits.I2C1MIF = 0; // Borro el flag
}