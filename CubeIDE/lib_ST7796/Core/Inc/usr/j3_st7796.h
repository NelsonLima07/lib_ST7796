/*
 * j3_st7796.h
 *
 *  Created on: Aug 6, 2025
 *      Author: Nelson Lima
 */

#ifndef INC_J3_ST7796_H_
#define INC_J3_ST7796_H_

#include <stdint.h>
#include <stdbool.h>

#include "j3_buffer.h"
#include "stm32f4xx_hal.h"

#include "usr/j3_buffer.h"

// Definições de pinos para o display (ajuste conforme sua conexão)
#define ST7796_RST_Pin GPIO_PIN_3
#define ST7796_RST_GPIO_Port GPIOA
#define ST7796_DC_Pin GPIO_PIN_2
#define ST7796_DC_GPIO_Port GPIOA
#define ST7796_CS_Pin GPIO_PIN_1
#define ST7796_CS_GPIO_Port GPIOA

// Definições de resolução do display (comum para ST7796)
#define ST7796_WIDTH 480
#define ST7796_HEIGHT 320
#define ST7796_ORIENTACAO 1


#define TAM_TELA 480 * 320    // 153600 Pixels
#define TAM_BUFFER 65535 * 2 // 131070 => 127Kbytes

struct TDisplayST7796{
	SPI_HandleTypeDef *spi;
	volatile bool dmaBusy;
	bool temTranparencia;
	uint16_t transparenciaCor;
	uint16_t backgroundCor;
	TJ3BufferManager *bufferManager;
	bool bufferManagerAtivo;
};
typedef struct TDisplayST7796 TDisplayST7796;


// Funções da biblioteca
void j3_ST7796_init(TDisplayST7796 *_display, SPI_HandleTypeDef *_hspi);
void j3_ST7796_setRotation(TDisplayST7796 *_display, uint8_t _rotation);
void j3_ST7796_setTransparencia(TDisplayST7796 *_display, uint16_t _cor);
void j3_ST7796_setNoTransparencia(TDisplayST7796 *_display);
void j3_ST7796_setBackground(TDisplayST7796 *_display, uint16_t _cor);
void j3_ST7796_setBufferManager(TDisplayST7796 *_display, TJ3BufferManager *_bufferManager);

void j3_ST7796_drawBuffer(TDisplayST7796 *_display);

void j3_ST7796_drawPixel(TDisplayST7796 *_display, uint16_t x, uint16_t y, uint16_t color);
void j3_ST7796_drawLine(TDisplayST7796 *_display, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color); //
void j3_ST7796_drawBitmap(TDisplayST7796 *_display, uint16_t _x, uint16_t _y, uint16_t _largura, uint16_t _altura, const uint16_t *_bitmap);


void j3_ST7796_fillBackground(TDisplayST7796 *_display);
void j3_ST7796_fillScreen(TDisplayST7796 *_display, uint16_t color);
void j3_ST7796_fillScreenDMA(TDisplayST7796 *_display, uint16_t color);
void j3_ST7796_fillWindow(TDisplayST7796 *_display, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t _cor);

// Enviar uma buffer diretamente para o display, usando DMA.
void ST7796_SendDataDMA(TDisplayST7796 *_display, uint16_t *_buffer, uint32_t _size);
void ST7796_SendBuffer(TDisplayST7796 *_display);

#endif /* INC_J3_ST7796_H_ */
