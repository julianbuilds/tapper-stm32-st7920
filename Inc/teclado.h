#ifndef TECLADO_H_
#define TECLADO_H_

#include "stm32f7xx_hal.h"

#define TECLADO_C1_PIN     GPIO_PIN_2
#define TECLADO_C1_PORT    GPIOE

#define TECLADO_C2_PIN     GPIO_PIN_4
#define TECLADO_C2_PORT    GPIOE

#define TECLADO_C3_PIN     GPIO_PIN_5
#define TECLADO_C3_PORT    GPIOE

#define TECLADO_C4_PIN     GPIO_PIN_6
#define TECLADO_C4_PORT    GPIOE

#define TECLADO_F1_PIN     GPIO_PIN_3
#define TECLADO_F1_PORT    GPIOE

#define TECLADO_F2_PIN     GPIO_PIN_8
#define TECLADO_F2_PORT    GPIOE

#define TECLADO_F3_PIN     GPIO_PIN_7
#define TECLADO_F3_PORT    GPIOE

#define TECLADO_F4_PIN     GPIO_PIN_9
#define TECLADO_F4_PORT    GPIOE

#define TECLADO_ANTIRREBOTE_MS   20

void TECLADO_Init(void);

char TECLADO_LeeTecla(void);

char TECLADO_Escanear(void);

char TECLADO_LeeTeclaSinBloqueo(void);

#endif
