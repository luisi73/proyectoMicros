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
char getcUART(void);
void putsUART(char *s);
int charToInt(char c);
void verif(char s[]);

void abrirPuerta(void);
void cerrarPuerta(void);

void setErrorCounter(int counter);
void plusErrorCounter(int add_num);

int getErrorCounter(void);
void changePasswordSystem(void);
char* convertIntegerToChar(int N);

void menuSelect(void);
void selectOption(char s[]);
void modifyPin(void);
void selectUser(char s[]);
void changePin(char s[]);

#ifdef	__cplusplus
}
#endif

#endif	/* UART1_H */

