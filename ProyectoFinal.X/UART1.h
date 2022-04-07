/* 
 * File:   UART1.h
 * Author: daniel
 *
 * Created on January 27, 2018, 6:04 PM
 */

#ifndef UART1_H
#define	UART1_H

#ifdef	__cplusplus
extern "C" {
#endif

void InicializarPines(int baudios);
void abrirPuerta(void);
void cerrarPuerta(void);
char getcUART(void);
void putsUART(char *s);
int charToInt(char c);
void verif(char s[]);

void setErrorCounter(int counter);
void plusErrorCounter(int add_num);

int getErrorCounter(void);
void changePasswordSystem(void);
char* convertIntegerToChar(int N);


void modifyPin(void);
void newUser(void);
void deleteUser(void);

#ifdef	__cplusplus
}
#endif

#endif	/* UART1_H */

