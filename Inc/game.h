#ifndef GAME_H
#define GAME_H

#include "stm32f7xx_hal.h"
#include <stdint.h>

#define TECLA_SUBIR     'A'     /* tendero un carril arriba            */
#define TECLA_BAJAR     'B'     /* tendero un carril abajo             */
#define TECLA_SERVIR    '#'     /* lanzar una cerveza                  */
#define TECLA_PAUSA     '*'     /* pausar / continuar                  */
#define TECLA_INICIO    'D'     /* iniciar partida / volver al menu    */


#define BUZZER_PORT         GPIOA
#define BUZZER_PIN          GPIO_PIN_3
#define BUZZER_NIVEL_ON     GPIO_PIN_SET
#define BUZZER_NIVEL_OFF    GPIO_PIN_RESET
#define BUZZER_PASIVO       0


#define LED_PUNTO_PORT      GPIOB
#define LED_PUNTO_PIN       GPIO_PIN_7          /* LD2 azul: punto / nivel   */
#define LED_PELIGRO_PORT    GPIOB
#define LED_PELIGRO_PIN     GPIO_PIN_14         /* LD3 rojo: peligro / golpe */

void Game_Init(void);


void Game_Update(void);

#endif
