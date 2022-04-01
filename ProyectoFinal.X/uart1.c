/*En primer lugar ha de crear un módulo, denominado UART1 en el que se incluirán todas las funciones
para el manejo de la UART usando colas e interrupciones. Dicho módulo ha de incluir:
Dos colas, una para el transmisor y otra para el receptor. 

Para que el código esté más claro, cada
una de estas colas ha de definirse como una estructura de datos junto con sus índices.
Una función para inicializar el módulo con el prototipo:

void putsUART ( char s []) ;

Una función para extraer un carácter de la cola.

char getcUART ( void ) ;

La rutina de atención a la interrupción.

Recuerde que todas estas funciones han sido vistas en clase. Tan sólo tendrá que modificar la función
de inicialización para incluir la selección de la velocidad deseada.
Para probar este módulo se creará un programa que haga el eco de los caracteres recibidos por la
UART. */

#include <xc.h>
#include <stdio.h>
#include "UART1.h"

#define TAM_COLA 100

#define PIN_PULSADOR 5

#define PIN_RX 13
#define PIN_TX 7

uint8_t puerta_abierta = 0;
uint8_t polis=0;

typedef struct
{
    int icabeza;
    int icola;
    char cola[TAM_COLA];
} cola_t;

static cola_t tx, rx;

void InicializarPines(int baudios)
{
    rx.icabeza = 0;
    rx.icola = 0;
    tx.icabeza = 0;
    tx.icola = 0;

    /*
    void InicializarUART1 (int baudios ) ;

    La funcion sera similar a la expuesta en clase pero añadiendo la posibilidad de elegir la velocidad
    de la linea serie. Tenga en cuenta que para velocidades superiores a 38 400 baudios hay que usar
    el divisor por 4 en lugar del divisor por 16, pues de lo contrario se cometer demasiado error.
    Una funcion para introducir cadenas de caracteres en la cola:
    */
    if (baudios < 38400)
    {
        U1BRG = 5 * (1000000) / (16 * baudios) -1;
        U1MODEbits.BRGH = 0; 
    }
    else
    {
        U1BRG = 5 * (1000000) / (4 * baudios) - 1;
        U1MODEbits.BRGH = 1;
    }

    //Procedemos a activar las interrupciones del receptor
    IFS1bits.U1RXIF = 0; //Borramos el flag
    IFS1bits.U1TXIF = 0; //Borramos el flag del transmitor
    IEC1bits.U1RXIE = 1; //Habilitamos interrupciones
    IPC8bits.U1IP = 3;   //Prio 3
    IPC8bits.U1IS = 1;   //Subprio 1

    //Conectamos U1RX y U1TX a los sus pines respectivos de la placa del micro-proc
    ANSELB &= ~((1 << PIN_RX) | (1 << PIN_TX)); // Pines digitales

    TRISA = 0;
    TRISB |= ((1 << PIN_RX) |(1 << PIN_PULSADOR)) ; //Receptor es una entrada en el micro-proc
    TRISC = 0;

    LATA = 0;
    LATB |= 1 << PIN_TX; //A 1 si el transmisor esta inhabilitado
    LATC = 0xF;          //Apagamos todos los LEDS (Activos a nivel Bajo)

    SYSKEY = 0xAA996655; //Desbloqueamos los regs.
    SYSKEY = 0x556699AA;
    U1RXR = 3; //Asignamos los pines, Receptor a RB13 y Transmisor a RB7 (Uso de tabla, para sacar 3 y 1)
    RPB7R = 1;
    RPB15R = 5; // OC1 conectado a RB15
    SYSKEY = 0x1CA11CA1;

    //Enable del STA
    U1STAbits.URXISEL = 0; //Interrupcion cuando llegue 1 char
    U1STAbits.UTXISEL = 2; //Interrupcion que salte cuando se vacie la FIFO
    U1STAbits.URXEN = 1;   //Habilitamos receptor
    U1STAbits.UTXEN = 1;   //Habilitamos transmisor

    U1MODE = 0x8000; //Arrancamos la UART, solo esta el BIT ON activado (mirar tabla)
    
    OC1CON = 0;
    OC1R = 2500; // Tiempo en alto de 1 ms inicial
    OC1RS = 2500;
    OC1CON = 0x8006; // OC ON, modo PWM sin faltas

    T2CON = 0;
    TMR2 = 0;
    PR2 = 49999; // Periodo de 20 ms
    T2CON = 0x8010; // 2 ON, Div = 2
    
    T3CON = 0x0000; //Se apaga el timer
    TMR3 = 0;
    PR3 = 2499; // Se configura el timer para que termine la cuenta en 500 us/0,5ms
    IPC3bits.T3IP = 2; // Se configura la prioridad de la interrupci�on
    IPC3bits.T3IS = 0; // Subprioridad 0
    IFS0bits.T3IF = 0; // Se borra el flag de interrupci�on por si estaba pendiente
    IEC0bits.T3IE = 1; // y por �ultimo se habilita su interrupci�on
    T3CON = 0x8000; // Se arranca el timer una vez configurada la interrupci�on
    
}

 
    __attribute__ (( vector (12), interrupt (IPL2SOFT), nomips16 ))
void InterrupcionT3 (void) {
    static uint32_t tick=0,ticks = 0, seg = 0,segu=0;
    
    IFS0bits.T3IF = 0;
    if ( Ah n == 1) { // Solo cuenta si abierta
        ticks ++;
        if (ticks >= 2000) { // 1 seg, ticks
            seg ++;
            ticks = 0;
        }
    } else { // Mantengo todo a 0 en caso contrario
        ticks = 0;
        seg = 0;
    }
    
    if(polis==1){
        tick++;
        if(tick>=2000){
            segu++;
            tick=0;
            LATCINV=0xF;
        }
    }
    
    if (seg >= 30) { //30s
        cerrarPuerta();
    }
}
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

//Rutina de la interrupcion que va a permitir la comunicacion de la UART
__attribute__((vector(32), interrupt(IPL3SOFT), nomips16)) 
void InterrupcionUART1(void)
{
    if (IFS1bits.U1RXIF == 1)
    {
        if ((rx.icabeza + 1 == rx.icola) || (rx.icabeza + 1 == TAM_COLA && rx.icola == 0))
        {
            //Break FIFO LLENO
        }
        else
        {
            rx.cola[rx.icabeza] = U1RXREG;
            rx.icabeza++;

            if (rx.icabeza == TAM_COLA)
            {
                rx.icabeza = 0;
            }
        }
        IFS1bits.U1RXIF = 0;
    }

    if (IFS1bits.U1TXIF == 1)
    {
        if (tx.icola != tx.icabeza)
        {
            //Recepcion de nuevos datos
            U1TXREG = tx.cola[tx.icola];
            tx.icola++;
            if (tx.icola == TAM_COLA)
            {
                tx.icola = 0;
            }
        }
        else
        {                        //Se ha vaciado la cola
            IEC1bits.U1TXIE = 0; //Para evitar bucle infinito
        }
        IFS1bits.U1TXIF = 0;
    }
}

char getcUART(void)
{
    char c;
    if (rx.icola != rx.icabeza)
    {
        c = rx.cola[rx.icola];
        rx.icola++;
        if (rx.icola == TAM_COLA)
        {
            rx.icola = 0;
        }
    }
    else
    {
        c = '\0';
    }
    return c;
}

void putsUART(char *s)
{
    while (*s != '\0')
    {
        if ((tx.icabeza + 1 == tx.icola) || (tx.icabeza + 1 == TAM_COLA && tx.icola == 0))
        {
            break;
        }
        else
        {
            tx.cola[tx.icabeza] = *s;
            s++;
            tx.icabeza++;
            if (tx.icabeza == TAM_COLA)
            {
                tx.icabeza = 0;
            }
        }

        IEC1bits.U1TXIE = 1;
    }
}

//==============================================
// <?> Seccion Correspondiente al Bluetooth Bit-Whacker
//==============================================
static int pin, valor, estado;
static char puerto;
char response[10];

//Apuntes pendientes aqui
int charToInt(char c)
{
    if (c - 48 < 10)
        return c - 48;
    else if (c > 64 && c < 71)
        return c - 55;
    else
        return -1;
}

/* Comandos a cubrir:
    PD, <puerto>, <pin>, <direccion> \n
          A/B/C   , 0 - F, 0/1
    En caso de que los parametros sean correctos, "OK\n", else "Error\n"
    Encargado de asignar la direccion de un pin como entrada o salida 

    PI, <puerto>, <pin> \n
        A/B/C   ,  0 - F
    Devuelve el estado de un pin (Es decir, que hace un PORTB >> pin & 0x1)
    En formato PI, 1 \n <- Si esta encendido 

    PO, <puerto>, <pin>, <valor> \n
        A/B/C       0-F  1/0
    Cambiamos el estado de un pin, si valor = 1 <- LATxSET, en cambio si es 0, LATxCLR 
*/
static int error_counter;

void verif(char s[])
{
    //Codigos de PINES Codigos de ejemplo: 1234A, 2151B

    char *pines_acceso[] = {"1234A", "2151B"};
    int len = sizeof(pines_acceso) / sizeof(pines_acceso[0]);
    int i;
    char s_sub5[4];
    
    for(i=0; i<5;i++){
        s_sub5[i]=s[i];
    }
    
    asm("di");
    puerta_abierta=0;
    polis=0;
    asm("ei");
    
    for (i = 0; i < len; i++)
    {
        if (!strcmp(pines_acceso[i], s_sub5)) //Se devuelve un 0 si los strings son iguales
        {
            LATCSET = 0xF;
            // Do your stuff
            putsUART("\nCodigo veridico, pase a casa");
            if(strcmp(pines_acceso[0],s_sub5) == 0 )
            {
                putsUART("\nBienvenido a casa, YAGO");
            }
            else
            {
                putsUART("\nBienvenido a casa, LUIS");
            }
            abrirPuerta();
            asm("di");
            puerta_abierta=1;
            asm("ei");
            break;
        }
        else
        {
            if (getErrorCounter() == 3)
            {
                putsUART("\nYa es la 3� vez que metes el codigo mal. Llamando a la policia");
                setErrorCounter(0);
                cerrarPuerta();     
                asm("di");
                polis=1;
                asm("ei");
                //Insertar codigo de Buzzer y cosa externa
                break;
            }else if(i==(len-1))
            {
                putsUART("\nCodigo Incorrecto, a la siguiente llamo a la policia");
                plusErrorCounter(1);
                LATCCLR = 0xF;
                cerrarPuerta();
                 
                break;
            }
            
        }
    }
}

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