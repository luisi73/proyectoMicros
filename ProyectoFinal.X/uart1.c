#include <xc.h>
#include <stdio.h>
#include <string.h>
#include "UART1.h"

#define TAM_COLA 100

#define PIN_PULSADOR 5

#define PIN_PRESENCIA 14

#define PIN_ZUMBADOR 1

#define PIN_RX 13
#define PIN_TX 7

// Son los PINES EN RC
#define PIN_R 7
#define PIN_G 8
#define PIN_B 9

uint8_t puerta_abierta = 0;
uint8_t polis = 0;

typedef struct
{
    int icabeza;
    int icola;
    char cola[TAM_COLA];
} cola_t;

static cola_t tx, rx;

//============ SETUP INICIALIZACION =======================
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
        U1BRG = 5 * (1000000) / (16 * baudios) - 1;
        U1MODEbits.BRGH = 0;
    }
    else
    {
        U1BRG = 5 * (1000000) / (4 * baudios) - 1;
        U1MODEbits.BRGH = 1;
    }

    // Procedemos a activar las interrupciones del receptor
    IFS1bits.U1RXIF = 0; // Borramos el flag
    IFS1bits.U1TXIF = 0; // Borramos el flag del transmitor
    IEC1bits.U1RXIE = 1; // Habilitamos interrupciones
    IPC8bits.U1IP = 3;   // Prio 3
    IPC8bits.U1IS = 1;   // Subprio 1

    // Conectamos U1RX y U1TX a los sus pines respectivos de la placa del micro-proc
    ANSELB &= ~((1 << PIN_RX) | (1 << PIN_TX) | (1 << PIN_PRESENCIA)); // Pines digitales
    ANSELA |= (1 << PIN_ZUMBADOR);
    ANSELC &= ~((1 << PIN_R) | (1 << PIN_G) | (1 << PIN_B));

    TRISA = 0;
    TRISB |= ((1 << PIN_RX) | (1 << PIN_PULSADOR) | (1 << PIN_PRESENCIA)); // Receptor es una entrada en el micro-proc
    TRISC = 0;

    LATA &= ~(1 << PIN_ZUMBADOR);
    LATB |= 1 << PIN_TX; // A 1 si el transmisor esta inhabilitado
    LATC = ~(0x380);     // Apagamos todos los LEDS (Activos a nivel Bajo)

    SYSKEY = 0xAA996655; // Desbloqueamos los regs.
    SYSKEY = 0x556699AA;
    U1RXR = 3; // Asignamos los pines, Receptor a RB13 y Transmisor a RB7 (Uso de tabla, para sacar 3 y 1)
    RPB7R = 1;
    RPB15R = 5; // OC1 conectado a RB15
    SYSKEY = 0x1CA11CA1;

    // Enable del STA
    U1STAbits.URXISEL = 0; // Interrupcion cuando llegue 1 char
    U1STAbits.UTXISEL = 2; // Interrupcion que salte cuando se vacie la FIFO
    U1STAbits.URXEN = 1;   // Habilitamos receptor
    U1STAbits.UTXEN = 1;   // Habilitamos transmisor

    U1MODE = 0x8000; // Arrancamos la UART, solo esta el BIT ON activado (mirar tabla)

    OC1CON = 0;
    OC1R = 2500; // Tiempo en alto de 1 ms inicial
    OC1RS = 2500;
    OC1CON = 0x8006; // OC ON, modo PWM sin faltas

    T2CON = 0;
    TMR2 = 0;
    PR2 = 49999;    // Periodo de 20 ms
    T2CON = 0x8010; // 2 ON, Div = 2

    T3CON = 0x0000; // Se apaga el timer
    TMR3 = 0;
    PR3 = 2499;        // Se configura el timer para que termine la cuenta en 500 us/0,5ms
    IPC3bits.T3IP = 2; // Se configura la prioridad de la interrupci�on
    IPC3bits.T3IS = 0; // Subprioridad 0
    IFS0bits.T3IF = 0; // Se borra el flag de interrupci�on por si estaba pendiente
    IEC0bits.T3IE = 1; // y por �ultimo se habilita su interrupci�on
    T3CON = 0x8000;    // Se arranca el timer una vez configurada la interrupci�on
}

// =========== FUNCIONES PARA EL MANEJO DE LA UART  ========

__attribute__((vector(32), interrupt(IPL3SOFT), nomips16)) void InterrupcionUART1(void)
{
    if (IFS1bits.U1RXIF == 1)
    {
        if ((rx.icabeza + 1 == rx.icola) || (rx.icabeza + 1 == TAM_COLA && rx.icola == 0))
        {
            // Break FIFO LLENO
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
            // Recepcion de nuevos datos
            U1TXREG = tx.cola[tx.icola];
            tx.icola++;
            if (tx.icola == TAM_COLA)
            {
                tx.icola = 0;
            }
        }
        else
        {                        // Se ha vaciado la cola
            IEC1bits.U1TXIE = 0; // Para evitar bucle infinito
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

//============ FUNCIONES PARA EL MANEJO DEL SERVO =========
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

__attribute__((vector(12), interrupt(IPL2SOFT), nomips16)) void InterrupcionT3(void)
{
    static uint32_t sound = 0, tick = 0, ticks = 0, seg = 0;
    uint16_t tiempo_millis = 0;
    int sube_baja = 1;
    IFS0bits.T3IF = 0;
    if (puerta_abierta == 1)
    { // Solo cuenta si abierta
        ticks++;
        if (ticks >= 2000)
        { // 1 seg, ticks
            seg++;
            ticks = 0;
        }
    }
    else
    { // Mantengo todo a 0 en caso contrario
        ticks = 0;
        seg = 0;
    }

    if (polis == 1)
    {
        tiempo_millis += sube_baja;
        // En el caso de que lleguemos al limite de los latidos del pulsador
        if (tiempo_millis == 20 || tiempo_millis == 0)
        {
            // Sea cual sea el valor, si es 1, pues bajar�.
            // Si es -1, pues subir�.
            sube_baja *= -1;
        }
        tick++;
        sound++;
        if (tick >= 2000)
        {
            tick = 0;
            LATCINV = 0x200;
        }
        else
        {
            LATCINV = 0x80;
        }
        if (sound >= (2 - tiempo_millis))
        {
            LATAINV = (1 << PIN_ZUMBADOR);
            sound = 0;
        }
    }

    if (seg >= 30)
    { // 30s
        cerrarPuerta();
        seg = 0;
    }
}

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

//==============================================
// FUNCIONES CORRESPONDIENTES AL MENU
//==============================================
static int pin, valor, estado;
static char puerto;
char response[10];

static int error_counter, select_option = 0, select_user = 0, select_pin = 0, user_selected;
static char pines_acceso[][10] = {"1234A", "2151B", "6969C", "*1CA1"};
static char nombres_pines[][10] = {"Yago", "Luis", "Chema", "Admin"};
static char pin_admin[][10] = {"*1CA11CA1"};

// Apuntes pendientes aqui
int charToInt(char c)
{
    if (c - 48 < 10)
        return c - 48;
    else if (c > 64 && c < 71)
        return c - 55;
    else
        return -1;
}

void verif(char s[])
{
    // Codigos de PINES Codigos de ejemplo: 1234A, 2151B
    int len = sizeof(pines_acceso) / sizeof(pines_acceso[0]);
    int i;
    char s_sub5[4];

    for (i = 0; i < 5; i++)
    {
        s_sub5[i] = s[i];
    }

    if (select_option == 1)
    {
        selectOption(s_sub5);
    }
    else if (select_user == 1)
    {
        selectUser(s_sub5);
    }
    else if (select_pin == 1)
    {
        changePin(s_sub5);
    }
    else
    {
        asm("di");
        puerta_abierta = 0;
        polis = 0;
        asm("ei");

        for (i = 0; i < len; i++)
        {
            LATCCLR = 0x380;
            if (!strcmp(pines_acceso[i], s_sub5) && !(PORTB >> PIN_PRESENCIA) & 1) // Se devuelve un 0 si los strings son iguales
            {
                setErrorCounter(0);
                LATACLR = (1 << PIN_ZUMBADOR);
                LATCSET = 0xF;
                // Do your stuff
                if (strcmp(pines_acceso[0], s_sub5) == 0)
                {
                    LATCSET = 0x100;
                    putsUART("\nBienvenido a casa, YAGO");
                    abrirPuerta();
                    asm("di");
                    puerta_abierta = 1;
                    asm("ei");
                }
                else if (strcmp(pines_acceso[1], s_sub5) == 0)
                {
                    LATCSET = 0x100;
                    putsUART("\nBienvenido a casa, LUIS");
                    abrirPuerta();
                    asm("di");
                    puerta_abierta = 1;
                    asm("ei");
                }
                else if (strcmp(pines_acceso[2], s_sub5) == 0)
                {
                    LATCSET = 0x100;
                    putsUART("\nChema, te queremos. NICE");
                    abrirPuerta();
                    asm("di");
                    puerta_abierta = 1;
                    asm("ei");
                }
                else if (strcmp(pines_acceso[3], s_sub5) == 0)
                {
                    char search[] = "*1CA11CA1";
                    char *ptr = strstr(s, search);

                    if (ptr != NULL)
                    {
                        menuIntro();
                        break;
                    }
                    else
                    {
                        putsUART("\nCodigo Incorrecto");
                        plusErrorCounter(1);
                        LATCCLR = 0xF;
                        cerrarPuerta();
                    }
                    break;
                }
                break;
            }
            else if (i == (len - 1) && !(PORTB >> PIN_PRESENCIA) & 1)
            {
                if (getErrorCounter() == 1)
                {
                    putsUART("\nEs la segunda vez que se inserta un codigo incorrecto.\n");
                    plusErrorCounter(1);
                    cerrarPuerta();
                    break;
                }
                else if (getErrorCounter() == 2)
                {
                    putsUART("\nYa es la tercera vez que metes el codigo mal. Llamando a la policia");
                    cerrarPuerta();
                    asm("di");
                    polis = 1;
                    asm("ei");
                    // Insertar codigo de Buzzer y cosa externa
                    break;
                }

                LATCCLR = 0x380;
                LATACLR = (1 << PIN_ZUMBADOR);
                putsUART("\nCodigo Incorrecto");
                plusErrorCounter(1);
                cerrarPuerta();
            }
            else if (i == (len - 1))
            {
                LATCCLR = 0x380;
                LATACLR = (1 << PIN_ZUMBADOR);
                putsUART("\nAcercate a la puerta");
                cerrarPuerta();
            }
        }
    }
}

//========  FUNCIONES MENU  ========================
void menuIntro(void)
{
    LATCSET = 0x180;
    putsUART("\n== Menu del Sistema ==\n");

    putsUART("1.Modificar PIN\n");
    putsUART("2.Mostrar usuarios\n");
    putsUART("3.Salir\n");
    // Para poder pasar a la selección de las opciones del menu
    select_option = 1; //<- Descomentar para activar para que al escribir caracteres pasemos al otro
}

void selectOption(char s[])
{
    // Debemos de verificar lo que se le pasa al micro como funcion y despues
    char s_c[0];
    int option;
    s_c[0] = s[0];
    // Select option para que no vaya a verif
    option = charToInt(s_c[0]);

    if (option == 1)
    {
        modifyPin();
    }
    else if (option == 2)
    {
        mostarUsuarios();
    }
    else if (option == 3)
    {
        putsUART("\nSaliendo del Menu...\n");
        select_option = 0;
    }
    else
    {
        putsUART("\nOpcion Incorrecta. Saliendo del Menu");
        select_option = 0;
    }
};

void mostarUsuarios(void)
{
    putsUART("\nUsuarios del sistema:\n");
    int i;
    for (i = 0; i < sizeof(nombres_pines) / sizeof(nombres_pines[0]); i++)
    {
        putsUART(nombres_pines[i]);
        putsUART("\n");
    }
}

void modifyPin(void)
{
    putsUART("Usuarios del sistema:\n");
    int i;
    char text[2];
    for (i = 0; i < sizeof(nombres_pines) / sizeof(nombres_pines[0]); i++)
    {
        sprintf(text, "%d", i + 1);
        putsUART(text);
        putsUART(". ");
        putsUART(nombres_pines[i]);
        putsUART("\n");
    }
    putsUART("\n¿De que usuario quiere cambiar el PIN?\n");
    select_user = 1;
    select_option = 0;
};

void selectUser(char s[])
{
    // Reseteamos para que no se vuelva a meter en la funci�n.
    select_user = 0;
    user_selected = charToInt(s[0]);
    putsUART("INSERTAR NUEVO PIN\nDeben de tener longitud 5");
    select_pin = 1;
    // Recibimos el nombre de 4 caracteres
}

void changePin(char s[])
{
    putsUART(s);
    strcpy(pines_acceso[user_selected - 1], s);
    putsUART("PIN CAMBIADO");
    select_pin = 0;
    select_option = 0;
    select_user = 0;
}
