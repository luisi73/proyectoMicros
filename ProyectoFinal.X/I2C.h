/* 
 * File:   I2C.h
 * Author: daniel
 *
 * Created on January 27, 2018, 6:04 PM
 */

#ifndef I2C_H
#define	I2C_H

#ifdef	__cplusplus
extern "C" {
#endif

void I2C1Inicializa(int velocidad);
void I2C1GenerStart(void);
int I2C1EscribeByte(uint8_t dato);
uint8_t I2C1LeeByte(int ack);
void I2C1GeneraReStart(void);
void I2C1GeneraStop(void);

#ifdef	__cplusplus
}
#endif

#endif	/* I2C_H */

