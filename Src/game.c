//Juego
#include "game.h"
#include "ST7920_parallel.h"
#include "teclado.h"
#include <stdio.h> //solo para snprintf

//Imagenes
#include "escenario.h"
#include "menu.h"
#include "gameover.h"
#include "victoria.h"
#include "tendero.h"
#include "muneco.h"
#include "latacerveza.h"

extern uint8_t image[1024];

#define ANCHO_PANTALLA      128
#define ALTO_PANTALLA       64
#define BYTES_PANTALLA      1024
#define BYTES_POR_FILA      16

#define PASO_MS             40      //LÓGICA CADA 40MS
#define MAX_PASOS           3       //PASOS

#define NUM_CARRILES        3
#define NUM_NIVELES         3
#define VIDAS_INICIALES     3
#define MAX_CLIENTES        8
#define MAX_CERVEZAS        6
#define CARRIL_INICIAL      1
#define HUD_X               20  //POSICIÓN PARA MARCADORES
#define HUD_Y_PUNTOS        46
#define HUD_Y_NIVEL         55
#define VEL_CERVEZA         3  //VELOCIDAD DE LA CERVEZA EN PIXLES
#define ESPERA_CERVEZA      6
#define DIST_PELIGRO        24 //PELIGRO CON EL CLIENTE
#define PUNTOS_SERVIR       10 //PUNTOS POR CLIENTE
#define PUNTOS_NIVEL        50
#define PUNTOS_DESPERDICIO  5  //PARA CUANDO NO HAY CLIENTE
#define PASOS_INTRO         50
#define PASOS_GOLPE         30 //PARA VIDA PERDIDA
#define PASOS_BLOQUEO       30
#define PAUSA_PITIDO_MS     70
#define DESTELLO_MS         120//TIEMPO LED
//ESTADOS DEL JUEGO
#define ST_MENU             0
#define ST_NIVEL            1
#define ST_JUEGO            2
#define ST_PAUSA            3
#define ST_GOLPE            4
#define ST_GAMEOVER         5
#define ST_VICTORIA         6

//POSICION PARA EL PERSONAJE Y CERVEZA EN FORMA VERTICAL
static const uint8_t TOP_Y_PERSONAJE[NUM_CARRILES] = { 0, 12, 25 };
static const uint8_t TOP_Y_CERVEZA[NUM_CARRILES]   = { 5, 18, 31 };

//POSICION PARA EL PERSONAJE Y CLIENTES EN FORMA HORIZONTAL
static const int16_t X_INICIO[NUM_CARRILES]  = { 24, 16, 8 };
static const int16_t X_TENDERO[NUM_CARRILES] = { 92, 100, 108 };
//DIFICULTAD
static const uint8_t  VEL_CLIENTE[NUM_NIVELES]          = { 35, 60, 85 }; //VELOCIDAD DE LOS CLIENTES
static const uint16_t PASOS_ENTRE_CLIENTES[NUM_NIVELES] = { 70, 50, 36 };  //PASOS ENTRE CLIENTES
static const uint8_t  META_CLIENTES[NUM_NIVELES]        = { 8, 10, 12 };  //CANTIDAD DE CLIENTES

static uint8_t  estado;
static uint8_t  nivel;
static uint8_t  vidas;
static uint16_t puntos;
static uint8_t  carril_tendero;
static uint8_t  espera_cerveza;
static uint8_t  servidos;
static uint8_t  generados;
static uint16_t t_cliente_nuevo; //PASOS DESDE EL ULTIMO CLIENTE
static uint8_t  acum_avance;
static uint16_t t_estado;
static uint32_t t_ultimo_paso;
static char     tecla_pendiente;
static uint32_t semilla = 1;
//CLIENTES ACTIVOS
static uint8_t  cli_activo[MAX_CLIENTES];
static uint8_t  cli_carril[MAX_CLIENTES];
static int16_t  cli_x[MAX_CLIENTES];
//CERVEZAS
static uint8_t  cer_activo[MAX_CERVEZAS];
static uint8_t  cer_carril[MAX_CERVEZAS];
static int16_t  cer_x[MAX_CERVEZAS];

//BUZZER Y LED
static uint8_t  bz_restantes;
static uint8_t  bz_sonando;
static uint16_t bz_duracion;
static uint32_t bz_t_cambio;
static uint8_t  led_punto_encendido;
static uint32_t led_punto_t;

static const char FUENTE_LETRAS[] = "0123456789ADEILNOPSTUV:";

static const uint8_t FUENTE_COLUMNAS[] = {
    0x3E, 0x51, 0x49, 0x45, 0x3E,   /* 0 */
    0x00, 0x42, 0x7F, 0x40, 0x00,   /* 1 */
    0x42, 0x61, 0x51, 0x49, 0x46,   /* 2 */
    0x21, 0x41, 0x45, 0x4B, 0x31,   /* 3 */
    0x18, 0x14, 0x12, 0x7F, 0x10,   /* 4 */
    0x27, 0x45, 0x45, 0x45, 0x39,   /* 5 */
    0x3C, 0x4A, 0x49, 0x49, 0x30,   /* 6 */
    0x01, 0x71, 0x09, 0x05, 0x03,   /* 7 */
    0x36, 0x49, 0x49, 0x49, 0x36,   /* 8 */
    0x06, 0x49, 0x49, 0x29, 0x1E,   /* 9 */
    0x7E, 0x11, 0x11, 0x11, 0x7E,   /* A */
    0x7F, 0x41, 0x41, 0x22, 0x1C,   /* D */
    0x7F, 0x49, 0x49, 0x49, 0x41,   /* E */
    0x00, 0x41, 0x7F, 0x41, 0x00,   /* I */
    0x7F, 0x40, 0x40, 0x40, 0x40,   /* L */
    0x7F, 0x04, 0x08, 0x10, 0x7F,   /* N */
    0x3E, 0x41, 0x41, 0x41, 0x3E,   /* O */
    0x7F, 0x09, 0x09, 0x09, 0x06,   /* P */
    0x46, 0x49, 0x49, 0x49, 0x31,   /* S */
    0x01, 0x01, 0x7F, 0x01, 0x01,   /* T */
    0x3F, 0x40, 0x40, 0x40, 0x3F,   /* U */
    0x1F, 0x20, 0x40, 0x20, 0x1F,   /* V */
    0x00, 0x36, 0x36, 0x00, 0x00    /* : */
};

//BUZZER Y LEDS
static void buzzer_salida(uint8_t encendido)
{
    if (encendido != 0) {
        HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, BUZZER_NIVEL_ON);
    } else {
        HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, BUZZER_NIVEL_OFF);
    }
}

//PARA EL BUZZER
static void buzzer_tono(uint16_t duracion_ms)
{
    uint32_t t0 = HAL_GetTick();
    uint32_t t_anterior = t0;

    while ((HAL_GetTick() - t0) < duracion_ms) {
        if (HAL_GetTick() != t_anterior) {
            t_anterior = HAL_GetTick();
            HAL_GPIO_TogglePin(BUZZER_PORT, BUZZER_PIN);
        }
    }
    buzzer_salida(0);
}

//HACER PITAR EL BUZZER
static void buzzer_pitar(uint8_t cantidad, uint16_t duracion_ms)
{
    uint8_t i;

    if (BUZZER_PASIVO == 1) {
        for (i = 0; i < cantidad; i++) {
            buzzer_tono(duracion_ms);
            if (i + 1 < cantidad) {
                HAL_Delay(PAUSA_PITIDO_MS);
            }
        }
    } else {
        bz_restantes = cantidad;
        bz_duracion = duracion_ms;
        bz_sonando = 1;
        bz_t_cambio = HAL_GetTick();
        buzzer_salida(1);
    }
}

//APAAGAR EL BUZZER Y ACTUALIZARLO
static void buzzer_actualizar(void)
{
    uint32_t ahora = HAL_GetTick();

    if (bz_sonando == 1) {
        if ((ahora - bz_t_cambio) >= bz_duracion) {
            buzzer_salida(0);
            bz_sonando = 0;
            bz_t_cambio = ahora;
            if (bz_restantes > 0) {
                bz_restantes--;
            }
        }
    } else if (bz_restantes > 0) {
        if ((ahora - bz_t_cambio) >= PAUSA_PITIDO_MS) {
            buzzer_salida(1);
            bz_sonando = 1;
            bz_t_cambio = ahora;
        }
    }
}

static void led_peligro(uint8_t encendido)
{
    if (encendido != 0) {
        HAL_GPIO_WritePin(LED_PELIGRO_PORT, LED_PELIGRO_PIN, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(LED_PELIGRO_PORT, LED_PELIGRO_PIN, GPIO_PIN_RESET);
    }
}

//PARA PRENDER EL LED
static void led_punto_destello(void)
{
    HAL_GPIO_WritePin(LED_PUNTO_PORT, LED_PUNTO_PIN, GPIO_PIN_SET);
    led_punto_encendido = 1;
    led_punto_t = HAL_GetTick();
}

static void leds_actualizar(void)
{
    if (led_punto_encendido == 1) {
        if ((HAL_GetTick() - led_punto_t) >= DESTELLO_MS) {
            HAL_GPIO_WritePin(LED_PUNTO_PORT, LED_PUNTO_PIN, GPIO_PIN_RESET);
            led_punto_encendido = 0;
        }
    }
}

//PARA GENERAR LOS NÚMEROS
static uint8_t numero_al_azar(uint8_t limite)
{
    semilla = semilla * 1103515245u + 12345u + HAL_GetTick();
    return (uint8_t)((semilla >> 16) % limite);
}

//PARA DIBUJAR SOBRE CADA FRAME CONSIDERANDO LAS COORDENADAS EN X Y EN Y
static void poner_pixel(int16_t x, int16_t y, uint8_t encendido)
{
    uint16_t indice;
    uint8_t  mascara;

    if (x < 0 || x >= ANCHO_PANTALLA || y < 0 || y >= ALTO_PANTALLA) {
        return; //SI ES FUERA DE LA PANTALLA
    }

    indice  = (uint16_t)(y * BYTES_POR_FILA + x / 8);
    mascara = (uint8_t)(0x80 >> (x % 8));

    if (encendido != 0) {
        image[indice] |= mascara; //ENCENDER
    } else {
        image[indice] &= (uint8_t)~mascara; //APAGAR
    }
}

//COPIAR IMAGEN COMPLETA
static void copiar_fondo(const uint8_t *imagen)
{
    uint16_t i;
    for (i = 0; i < BYTES_PANTALLA; i++) {
        image[i] = imagen[i];
    }
}

static void invertir_pantalla(void)
{
    uint16_t i;
    for (i = 0; i < BYTES_PANTALLA; i++) {
        image[i] ^= 0xFF;
    }
}

static void dibujar_sprite(int16_t x, int16_t y, uint8_t ancho, uint8_t alto, const uint8_t *sprite)
{
    uint8_t bytes_por_fila = (uint8_t)((ancho + 7) / 8);
    uint8_t fila, col, dato;

    for (fila = 0; fila < alto; fila++) {
        for (col = 0; col < ancho; col++) {
            dato = sprite[fila * bytes_por_fila + col / 8];
            if ((dato & (0x80 >> (col % 8))) != 0) {
                poner_pixel(x + col, y + fila, 1);
            }
        }
    }
}

static void dibujar_caracter(int16_t x, int16_t y, char c)
{
    uint8_t i = 0;
    uint8_t col, fila, columna;

    while (FUENTE_LETRAS[i] != '\0' && FUENTE_LETRAS[i] != c) {
        i++;
    }
    if (FUENTE_LETRAS[i] == '\0') {
        return;
    }

    for (col = 0; col < 5; col++) {
        columna = FUENTE_COLUMNAS[i * 5 + col];
        for (fila = 0; fila < 7; fila++) {
            if ((columna & (1 << fila)) != 0) {
                poner_pixel(x + col, y + fila, 1);
            }
        }
    }
}

static uint8_t largo_texto(const char *texto)
{
    uint8_t n = 0;
    while (texto[n] != '\0') {
        n++;
    }
    return n;
}

static void dibujar_texto(int16_t x, int16_t y, const char *texto)
{
    uint8_t i = 0;
    while (texto[i] != '\0') {
        dibujar_caracter(x, y, texto[i]);
        x += 6;
        i++;
    }
}

//RECTANGULO
static void dibujar_caja(int16_t x, int16_t y, int16_t ancho, int16_t alto)
{
    int16_t i, j;

    for (j = 0; j < alto; j++) {
        for (i = 0; i < ancho; i++) {
            poner_pixel(x + i, y + j, 0);
        }
    }
    for (i = 0; i < ancho; i++) {
        poner_pixel(x + i, y, 1);
        poner_pixel(x + i, y + alto - 1, 1);
    }
    for (j = 0; j < alto; j++) {
        poner_pixel(x, y + j, 1);
        poner_pixel(x + ancho - 1, y + j, 1);
    }
}

//TEXTO DCENTRO DE UN CUADRADO
static void cuadro_texto_centrado(int16_t y, const char *texto)
{
    int16_t ancho = (int16_t)(largo_texto(texto) * 6 + 7);
    int16_t x = (int16_t)((ANCHO_PANTALLA - ancho) / 2);

    dibujar_caja(x, y, ancho, 13);
    dibujar_texto(x + 4, y + 3, texto);
}

//LÓGICA DEL JUEGO
static void limpiar_carriles(void)
{
    uint8_t i;
    for (i = 0; i < MAX_CLIENTES; i++) {
        cli_activo[i] = 0;
    }
    for (i = 0; i < MAX_CERVEZAS; i++) {
        cer_activo[i] = 0;
    }
    espera_cerveza = 0;
    acum_avance = 0;
}

static void iniciar_nivel(void)
{
    limpiar_carriles();
    servidos = 0;
    generados = 0;
    t_cliente_nuevo = PASOS_ENTRE_CLIENTES[nivel - 1] / 2;  //SALE PRIMER CLIENTE
    carril_tendero = CARRIL_INICIAL;
    t_estado = PASOS_INTRO;
    estado = ST_NIVEL;

    led_peligro(0);
    led_punto_destello();
    buzzer_pitar(nivel, 80);
}

static void nueva_partida(void)
{
    semilla = semilla + HAL_GetTick();
    nivel = 1;
    vidas = VIDAS_INICIALES;
    puntos = 0;
    iniciar_nivel();
}

static void lanzar_cerveza(void)
{
    uint8_t i;

    if (espera_cerveza > 0) {
        return;
    }
    for (i = 0; i < MAX_CERVEZAS; i++) {
        if (cer_activo[i] == 0) {
            cer_activo[i] = 1;
            cer_carril[i] = carril_tendero;
            cer_x[i] = X_TENDERO[carril_tendero] - CERVEZA_ANCHO;
            espera_cerveza = ESPERA_CERVEZA;
            return;
        }
    }
}

static void crear_cliente(void)
{
    uint8_t i;
    uint8_t carril = numero_al_azar(NUM_CARRILES);

    for (i = 0; i < MAX_CLIENTES; i++) {
        if (cli_activo[i] == 1 && cli_carril[i] == carril &&
            cli_x[i] < X_INICIO[carril] + MUNECO_ANCHO + 4) {
            return;
        }
    }

    for (i = 0; i < MAX_CLIENTES; i++) {
        if (cli_activo[i] == 0) {
            cli_activo[i] = 1;
            cli_carril[i] = carril;
            cli_x[i] = X_INICIO[carril];
            generados++;
            t_cliente_nuevo = 0;
            return;
        }
    }
}

//SI LA CERVEZA TOCA UN CLIENTE
static void revisar_colisiones(void)
{
    uint8_t b, c;

    for (b = 0; b < MAX_CERVEZAS; b++) {
        if (cer_activo[b] == 1) {
            for (c = 0; c < MAX_CLIENTES; c++) {
                if (cer_activo[b] == 1 && cli_activo[c] == 1 && cli_carril[c] == cer_carril[b]) {
                    if (cer_x[b] < cli_x[c] + MUNECO_ANCHO && cer_x[b] + CERVEZA_ANCHO > cli_x[c]) {
                        cer_activo[b] = 0;
                        cli_activo[c] = 0;
                        puntos += PUNTOS_SERVIR;
                        servidos++;
                        buzzer_pitar(1, 30);
                        led_punto_destello();
                    }
                }
            }
        }
    }
}

static void perder_vida(void)
{
    if (vidas > 0) {
        vidas--;
    }
    t_estado = PASOS_GOLPE;
    estado = ST_GOLPE;
    led_peligro(1);
    buzzer_pitar(1, 500);
}

static void actualizar_juego(char tecla)
{
    uint8_t i;
    uint8_t peligro = 0;

    if (tecla == TECLA_PAUSA) { //PARA LA PAUSA
        estado = ST_PAUSA;
        buzzer_pitar(1, 30);
        return;
    }
    if (tecla == TECLA_SUBIR) {
        if (carril_tendero > 0) {
            carril_tendero--;
        }
    } else if (tecla == TECLA_BAJAR) {
        if (carril_tendero < NUM_CARRILES - 1) {
            carril_tendero++;
        }
    } else if (tecla == TECLA_SERVIR) {
        lanzar_cerveza();
    }

    if (espera_cerveza > 0) {
        espera_cerveza--;
    }

    for (i = 0; i < MAX_CERVEZAS; i++) { //PARA DESLIZAR LAS CERVEZAS A LA IZQUIERDA
        if (cer_activo[i] == 1) {
            cer_x[i] -= VEL_CERVEZA;
            if (cer_x[i] + CERVEZA_ANCHO <= X_INICIO[cer_carril[i]]) {
                cer_activo[i] = 0; //PERDER PUNTOS PORQUE NO ENCUENTRA CLIENTE
                if (puntos >= PUNTOS_DESPERDICIO) {
                    puntos -= PUNTOS_DESPERDICIO;
                } else {
                    puntos = 0;
                }
                buzzer_pitar(1, 120);
            }
        }
    }

    revisar_colisiones();

    acum_avance += VEL_CLIENTE[nivel - 1];
    if (acum_avance >= 100) {
        acum_avance -= 100;
        for (i = 0; i < MAX_CLIENTES; i++) {
            if (cli_activo[i] == 1) {
                cli_x[i]++;
                if (cli_x[i] + MUNECO_ANCHO >= X_TENDERO[cli_carril[i]]) {
                    perder_vida();  //CUANDO CLIENTE TOCA AL CANTINERO
                    return;
                }
            }
        }
    }
    for (i = 0; i < MAX_CLIENTES; i++) {
        if (cli_activo[i] == 1) {
            if (X_TENDERO[cli_carril[i]] - (cli_x[i] + MUNECO_ANCHO) <= DIST_PELIGRO) {
                peligro = 1;  //CLIENTE CERCA DEL CANTINERO
            }
        }
    }
    led_peligro(peligro);
    //NUEVOS CLIENTES
    if (generados < META_CLIENTES[nivel - 1]) {
        t_cliente_nuevo++;
        if (t_cliente_nuevo >= PASOS_ENTRE_CLIENTES[nivel - 1]) {
            crear_cliente();
        }
    }
    //SE ACABA EL NIVEL
    if (servidos >= META_CLIENTES[nivel - 1]) {
        puntos += PUNTOS_NIVEL;
        led_peligro(0);
        if (nivel < NUM_NIVELES) {
            nivel++;
            iniciar_nivel();
        } else {
            t_estado = PASOS_BLOQUEO;
            estado = ST_VICTORIA;
            buzzer_pitar(5, 80);
        }
    }
}

//UN PASO CADA 40MS
static void logica(char tecla)
{
    if (estado == ST_MENU) {
        if (tecla == TECLA_INICIO) {
            nueva_partida();
        }
    } else if (estado == ST_NIVEL) {
        if (t_estado > 0) {
            t_estado--;
        } else {
            estado = ST_JUEGO;
        }
    } else if (estado == ST_JUEGO) {
        actualizar_juego(tecla);
    } else if (estado == ST_PAUSA) {
        if (tecla == TECLA_PAUSA) {
            estado = ST_JUEGO;
            buzzer_pitar(1, 30);
        }
    } else if (estado == ST_GOLPE) {
        if (t_estado > 0) {
            t_estado--;
        } else {
            led_peligro(0);
            if (vidas == 0) {
                t_estado = PASOS_BLOQUEO;
                estado = ST_GAMEOVER;
                buzzer_pitar(3, 250);
            } else {
                limpiar_carriles();
                generados = servidos;
                t_cliente_nuevo = PASOS_ENTRE_CLIENTES[nivel - 1] / 2;
                carril_tendero = CARRIL_INICIAL;
                estado = ST_JUEGO;
            }
        }
    } else {
    	//SI ES VICTORIA O GAMEOVER
        if (t_estado > 0) {
            t_estado--;
        } else if (tecla == TECLA_INICIO) {
            estado = ST_MENU;
        }
    }
}

//PARA DIBUJAR LA PANTALLA
static void dibujar_escena(void)
{
    char texto[20];
    uint8_t i;

    copiar_fondo(frame_escenario_0);

    for (i = 0; i < MAX_CLIENTES; i++) {
        if (cli_activo[i] == 1) {
            dibujar_sprite(cli_x[i], TOP_Y_PERSONAJE[cli_carril[i]], MUNECO_ANCHO, MUNECO_ALTO, frame_muneco_0);
        }
    }
    for (i = 0; i < MAX_CERVEZAS; i++) {
        if (cer_activo[i] == 1) {
            dibujar_sprite(cer_x[i], TOP_Y_CERVEZA[cer_carril[i]], CERVEZA_ANCHO, CERVEZA_ALTO, frame_cerveza_0);
        }
    }
    dibujar_sprite(X_TENDERO[carril_tendero], TOP_Y_PERSONAJE[carril_tendero], TENDERO_ANCHO, TENDERO_ALTO, frame_tendero_0);
    //MARCADORES EN LA PANTALLA
    snprintf(texto, sizeof(texto), "PUNTOS:%u", (unsigned)puntos);
    dibujar_texto(HUD_X, HUD_Y_PUNTOS, texto);
    snprintf(texto, sizeof(texto), "NIVEL:%u VIDAS:%u", (unsigned)nivel, (unsigned)vidas);
    dibujar_texto(HUD_X, HUD_Y_NIVEL, texto);
}

static void dibujar(void)
{
    char texto[16];
    uint32_t ahora = HAL_GetTick();

    if (estado == ST_MENU) {
        copiar_fondo(frame_menu_0);
        if (((ahora / 500) % 2) == 0) {
            cuadro_texto_centrado(49, "PULSA D");
        }
    } else if (estado == ST_NIVEL) {
        dibujar_escena();
        snprintf(texto, sizeof(texto), "NIVEL %u", (unsigned)nivel);
        cuadro_texto_centrado(25, texto);
    } else if (estado == ST_JUEGO) {
        dibujar_escena();
    } else if (estado == ST_PAUSA) {
        dibujar_escena();
        cuadro_texto_centrado(25, "PAUSA");
    } else if (estado == ST_GOLPE) {
        dibujar_escena();
        snprintf(texto, sizeof(texto), "VIDAS %u", (unsigned)vidas);
        cuadro_texto_centrado(25, texto);
        if (((t_estado / 5) % 2) == 1) {
            invertir_pantalla();
        }
    } else if (estado == ST_GAMEOVER) {
        copiar_fondo(gif_gameover_frames[(ahora / 200) % FRAME_GAMEOVER_TOTAL_FRAMES]);
        snprintf(texto, sizeof(texto), "PUNTOS: %u", (unsigned)puntos);
        dibujar_texto((int16_t)((ANCHO_PANTALLA - largo_texto(texto) * 6) / 2), 56, texto);
    } else {
        //VICTORIA
        copiar_fondo(frame_victoria_0);
        snprintf(texto, sizeof(texto), "PUNTOS: %u", (unsigned)puntos);
        cuadro_texto_centrado(46, texto);
    }
}


void Game_Init(void)
{
    GPIO_InitTypeDef pines = {0};
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, BUZZER_NIVEL_OFF);
    pines.Pin = BUZZER_PIN;
    pines.Mode = GPIO_MODE_OUTPUT_PP;
    pines.Pull = GPIO_NOPULL;
    pines.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(BUZZER_PORT, &pines);

    HAL_GPIO_WritePin(LED_PUNTO_PORT, LED_PUNTO_PIN, GPIO_PIN_RESET);
    pines.Pin = LED_PUNTO_PIN;
    HAL_GPIO_Init(LED_PUNTO_PORT, &pines);

    HAL_GPIO_WritePin(LED_PELIGRO_PORT, LED_PELIGRO_PIN, GPIO_PIN_RESET);
    pines.Pin = LED_PELIGRO_PIN;
    HAL_GPIO_Init(LED_PELIGRO_PORT, &pines);


    bz_restantes = 0;
    bz_sonando = 0;
    led_punto_encendido = 0;
    tecla_pendiente = '\0';
    nivel = 1;
    vidas = VIDAS_INICIALES;
    puntos = 0;
    limpiar_carriles();
    estado = ST_MENU;
    t_ultimo_paso = HAL_GetTick();
}

void Game_Update(void)
{
    uint32_t ahora;
    uint8_t  pasos = 0;
    char     tecla;

    buzzer_actualizar();
    leds_actualizar();

    tecla = TECLADO_LeeTeclaSinBloqueo();
    if (tecla != '\0') {
        tecla_pendiente = tecla;
    }

    ahora = HAL_GetTick();
    if ((ahora - t_ultimo_paso) < PASO_MS) {
        return;
    }
    //PARA CUANDO HAYA DEMORA

    while ((ahora - t_ultimo_paso) >= PASO_MS && pasos < MAX_PASOS) {
        t_ultimo_paso += PASO_MS;
        logica(tecla_pendiente);
        tecla_pendiente = '\0';
        pasos++;
    }
    if ((ahora - t_ultimo_paso) >= PASO_MS) {
        t_ultimo_paso = ahora;
    }

    dibujar();
    ST7920_Update();
}
