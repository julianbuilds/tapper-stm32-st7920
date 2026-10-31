# Juego tipo Tapper en GLCD 128x64 — STM32 NUCLEO-F767ZI

Juego de cantina tipo arcade hecho en C con HAL para la **NUCLEO-F767ZI**. Se juega con un
**teclado matricial 4x4** y se muestra en una **pantalla gráfica ST7920 de 128x64** (modo paralelo de 4 bits).
Proyecto del Laboratorio de Microcontroladores — Ingeniería Electrónica, UPTC (Tunja, Colombia).

> 📷 

## ¿Cómo se juega?

Un tendero sirve cervezas en una barra de 3 carriles. Los clientes salen por la izquierda y caminan
hacia él; hay que deslizarles una cerveza antes de que lleguen al final de la barra.

| Tecla | Acción |
|-------|--------|
| `A` | Subir de carril |
| `B` | Bajar de carril |
| `#` | Servir (lanzar una cerveza) |
| `*` | Pausa / continuar |
| `D` | Iniciar partida / volver al menú |

- **3 niveles**: en cada uno los clientes salen y avanzan más rápido (8, 10 y 12 clientes por nivel).
- **3 vidas**: pierdes una cuando un cliente llega hasta el tendero.
- **Puntaje**: +10 por cliente servido, +50 por terminar un nivel y −5 por cada cerveza desperdiciada.
- Buzzer y LEDs de la tarjeta dan retroalimentación (punto, peligro, golpe).

## Hardware

- NUCLEO-F767ZI (STM32F767ZI)
- Pantalla GLCD 128x64 con controlador ST7920 (pin **PSB** a nivel alto para modo paralelo)
- Teclado matricial 4x4
- Buzzer (activo, configurable en `Inc/game.h`)

### Conexiones

| Función | Pin |
|---------|-----|
| LCD RS / RW / E | PC9 / PC10 / PC11 |
| LCD RST | PD3 |
| LCD DB7 / DB6 / DB5 / DB4 | PD4 / PD5 / PD6 / PD7 |
| Teclado columnas C1..C4 | PE2, PE4, PE5, PE6 |
| Teclado filas F1..F4 | PE3, PE8, PE7, PE9 |
| Buzzer | PA3 |
| LED azul (LD2, punto) / rojo (LD3, peligro) / verde (LD1, latido) | PB7 / PB14 / PB0 |

## Estructura del proyecto

```
Src/    main.c, game.c (lógica y dibujo), ST7920_parallel.c (driver de la LCD), teclado.c
Inc/    headers y las imágenes convertidas a arreglos (escenario, menú, sprites, game over, victoria)
Startup/ y *.ld   archivos de arranque y linker del STM32F767ZI
Drivers/ HAL y CMSIS de STMicroelectronics (solo lo necesario para compilar)
```

## Cómo compilar y cargar

1. Instala **STM32CubeIDE**.
2. `File → Import → General → Existing Projects into Workspace` y selecciona la carpeta del repositorio.
3. Compila (`Ctrl+B`) y carga a la placa (`Run`/`Debug`) con el ST-LINK integrado.

## Detalles técnicos

- Reloj a **216 MHz** (HSI + PLL, Over-Drive).
- Driver propio del ST7920 con *frame buffer* de 1024 bytes en RAM y envío **solo de las zonas que cambiaron** para que el juego fluya.
- Lógica con paso fijo de 40 ms basado en `HAL_GetTick()`, sin bloquear el programa; el buzzer y los LEDs se controlan sin retardos.
- Imágenes de 1 bit por píxel (128x64) convertidas a arreglos de C; los sprites de los personajes son de 12x16 px y la cerveza de 6x10 px.

## Créditos y licencias

- Autor: **Julian** — [github.com/julianbuilds](https://github.com/julianbuilds)
- Carpeta `Drivers/`: HAL de STMicroelectronics y CMSIS de Arm, con sus licencias incluidas en cada subcarpeta.
- Licencia del código propio: _elige una (por ejemplo MIT) y agrega un archivo `LICENSE`_.
