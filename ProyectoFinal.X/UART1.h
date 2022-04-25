#ifndef UART1_H
#define UART1_H

#ifdef __cplusplus
extern "C"
{
#endif

    void InicializarPines(int baudios);
    char getcUART(void);
    void putsUART(char *s);
    void verif(char s[]);

#ifdef __cplusplus
}
#endif

#endif /* UART1_H */
