#include "teclado.h"

static const uint16_t FILAS_PINS[4]    = {TECLADO_F1_PIN, TECLADO_F2_PIN, TECLADO_F3_PIN, TECLADO_F4_PIN};
static const uint16_t COLUMNAS_PINS[4] = {TECLADO_C1_PIN, TECLADO_C2_PIN, TECLADO_C3_PIN, TECLADO_C4_PIN};

static char     tec_vista = '\0';
static char     tec_estable = '\0';
static uint32_t tec_t_cambio = 0;


static char ObtenerTecla(uint8_t fila, uint8_t columna)
{
    char tecla = '\0';

    if (fila == 0)
    {
        if (columna == 0)      tecla = '1';
        else if (columna == 1) tecla = '2';
        else if (columna == 2) tecla = '3';
        else if (columna == 3) tecla = 'A';
    }
    else if (fila == 1)
    {
        if (columna == 0)      tecla = '4';
        else if (columna == 1) tecla = '5';
        else if (columna == 2) tecla = '6';
        else if (columna == 3) tecla = 'B';
    }
    else if (fila == 2)
    {
        if (columna == 0)      tecla = '7';
        else if (columna == 1) tecla = '8';
        else if (columna == 2) tecla = '9';
        else if (columna == 3) tecla = 'C';
    }
    else if (fila == 3)
    {
        if (columna == 0)      tecla = '*';
        else if (columna == 1) tecla = '0';
        else if (columna == 2) tecla = '#';
        else if (columna == 3) tecla = 'D';
    }

    return tecla;
}

static void EsperaCorta(void)
{
    volatile uint32_t i;
    for (i = 0; i < 500; i++)
    {
    }
}

void TECLADO_Init(void)
{
    __HAL_RCC_GPIOE_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin   = TECLADO_F1_PIN | TECLADO_F2_PIN | TECLADO_F3_PIN | TECLADO_F4_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIOE, TECLADO_F1_PIN | TECLADO_F2_PIN | TECLADO_F3_PIN | TECLADO_F4_PIN, GPIO_PIN_RESET);

    GPIO_InitStruct.Pin  = TECLADO_C1_PIN | TECLADO_C2_PIN | TECLADO_C3_PIN | TECLADO_C4_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
}

char TECLADO_LeeTecla(void)
{
    for (uint8_t f = 0; f < 4; f++)
    {
        HAL_GPIO_WritePin(GPIOE, FILAS_PINS[f], GPIO_PIN_SET);
        HAL_Delay(1);

        for (uint8_t c = 0; c < 4; c++)
        {
            if (HAL_GPIO_ReadPin(GPIOE, COLUMNAS_PINS[c]) == GPIO_PIN_SET)
            {
                HAL_Delay(20);

                if (HAL_GPIO_ReadPin(GPIOE, COLUMNAS_PINS[c]) == GPIO_PIN_SET)
                {
                    char tecla_detectada = ObtenerTecla(f, c);


                    while (HAL_GPIO_ReadPin(GPIOE, COLUMNAS_PINS[c]) == GPIO_PIN_SET);

                    HAL_GPIO_WritePin(GPIOE, FILAS_PINS[f], GPIO_PIN_RESET);
                    return tecla_detectada;
                }
            }
        }

        HAL_GPIO_WritePin(GPIOE, FILAS_PINS[f], GPIO_PIN_RESET);
    }

    return '\0';
}

char TECLADO_Escanear(void)
{
    char tecla = '\0';

    for (uint8_t f = 0; f < 4; f++)
    {
        HAL_GPIO_WritePin(GPIOE, FILAS_PINS[f], GPIO_PIN_SET);
        EsperaCorta();

        for (uint8_t c = 0; c < 4; c++)
        {
            if (HAL_GPIO_ReadPin(GPIOE, COLUMNAS_PINS[c]) == GPIO_PIN_SET)
            {
                if (tecla == '\0')
                {
                    tecla = ObtenerTecla(f, c);
                }
            }
        }

        HAL_GPIO_WritePin(GPIOE, FILAS_PINS[f], GPIO_PIN_RESET);
    }

    return tecla;
}

char TECLADO_LeeTeclaSinBloqueo(void)
{
    char     vista = TECLADO_Escanear();
    uint32_t ahora = HAL_GetTick();
    char     nueva = '\0';

    if (vista != tec_vista)
    {
        tec_vista = vista;
        tec_t_cambio = ahora;
    }
    else if (vista != tec_estable)
    {
        if ((ahora - tec_t_cambio) >= TECLADO_ANTIRREBOTE_MS)
        {
            tec_estable = vista;
            if (vista != '\0')
            {
                nueva = vista;
            }
        }
    }

    return nueva;
}
