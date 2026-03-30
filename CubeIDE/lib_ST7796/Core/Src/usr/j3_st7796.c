/*
 * j3_st7796.c
 *
 *  Created on: Aug 6, 2025
 *      Author: Nelson Lima
 */
#include <stdlib.h>

#include "usr/j3_st7796.h"


// Função para enviar um comando para o display
static void ST7796_SendCommand(TDisplayST7796 *_display, uint8_t command) {
  uint16_t data = command;
  HAL_GPIO_WritePin(ST7796_DC_GPIO_Port, ST7796_DC_Pin, GPIO_PIN_RESET); // DC = 0 para comando
  HAL_GPIO_WritePin(ST7796_CS_GPIO_Port, ST7796_CS_Pin, GPIO_PIN_RESET); // CS = 0 para selecionar
  HAL_SPI_Transmit(_display->spi, (uint8_t*)&data, 1, HAL_MAX_DELAY);
  HAL_GPIO_WritePin(ST7796_CS_GPIO_Port, ST7796_CS_Pin, GPIO_PIN_SET);   // CS = 1 para deselecionar
}

// Função para enviar dados 16bits para o display
static void ST7796_SendData16(TDisplayST7796 *_display, uint16_t *data, uint16_t size) {
  HAL_GPIO_WritePin(ST7796_DC_GPIO_Port, ST7796_DC_Pin, GPIO_PIN_SET); // DC = 1 para dados
  HAL_GPIO_WritePin(ST7796_CS_GPIO_Port, ST7796_CS_Pin, GPIO_PIN_RESET); // CS = 0 para selecionar
  HAL_SPI_Transmit(_display->spi, (uint8_t *)data, size, HAL_MAX_DELAY);
  HAL_GPIO_WritePin(ST7796_CS_GPIO_Port, ST7796_CS_Pin, GPIO_PIN_SET);   // CS = 1 para deselecionar
}

// Função para enviar dados 16bits para o display
static void ST7796_SendDataDMA(TDisplayST7796 *_display, uint32_t size) {
  HAL_GPIO_WritePin(ST7796_DC_GPIO_Port, ST7796_DC_Pin, GPIO_PIN_SET); // DC = 1 para dados
  HAL_GPIO_WritePin(ST7796_CS_GPIO_Port, ST7796_CS_Pin, GPIO_PIN_RESET); // CS = 0 para selecionar
  _display->dmaBusy = true;
  HAL_SPI_Transmit_DMA(_display->spi, (uint8_t *)_display->buffer, size);
}


static void ST7796_setWindow(TDisplayST7796 *_display, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
  if(x2 >= ST7796_WIDTH) x2 = ST7796_WIDTH-1;
  if(y2 >= ST7796_HEIGHT) y2 = ST7796_HEIGHT-1;
  if(x1 > x2)x2 = x1;
  if(y1 > y2)y2 = y1;

  ST7796_SendCommand(_display, 0x2A); // CASET - Column Address Set
  uint16_t data[2];
  data[0] = x1;
  data[1] = x2;
  ST7796_SendData16(_display, data, 2);

  ST7796_SendCommand(_display, 0x2B); // RASET - Row Address Set
  data[0] = y1;
  data[1] = y2;
  ST7796_SendData16(_display, data, 4);
}


TDisplayST7796 *j3_ST7796_new(SPI_HandleTypeDef *_hspi, bool _createBuffer)
{
  TDisplayST7796 *auxDisplay;

  auxDisplay = malloc(sizeof(TDisplayST7796));
  auxDisplay->spi = _hspi;
  if(_createBuffer){
      auxDisplay->buffer = (uint16_t*)malloc(TAM_BUFFER);
  }
  auxDisplay->bufferBloco = 1;
  auxDisplay->dmaBusy = false;
  auxDisplay->temTranparencia = false;
  auxDisplay->transparenciaCor = 0x0000;
  auxDisplay->backgroundCor = 0xFFFF;

  return auxDisplay;
}

void j3_ST7796_init(TDisplayST7796 *_display)
{
  // Reset do display
  HAL_GPIO_WritePin(ST7796_RST_GPIO_Port, ST7796_RST_Pin, GPIO_PIN_RESET);
  HAL_Delay(100);
  HAL_GPIO_WritePin(ST7796_RST_GPIO_Port, ST7796_RST_Pin, GPIO_PIN_SET);
  HAL_Delay(100);

  // Sequência de inicialização (baseada no datasheet do ST7796)
  ST7796_SendCommand(_display, 0x01); // SWRESET - Software reset
  HAL_Delay(150);

  ST7796_SendCommand(_display, 0x11); // SLPOUT - Leave sleep mode
  HAL_Delay(120);

  ST7796_SendCommand(_display, 0x3A); // COLMOD - Interface Pixel Format
  uint16_t color_mode = 0x5500;   // 16 bits/pixel (RGB565)
  ST7796_SendData16(_display, &color_mode, 1);

  /*
  ST7796_SendCommand(0x36); // MADCTL - Memory Data Access Control
  uint8_t madctl = 0x00;       // Ajuste conforme a orientação desejada
  ST7796_SendData(&madctl, 1);
   */
  j3_ST7796_setRotation(_display, ST7796_ORIENTACAO);




  //ST7796_SendCommand(_display, c); // DISPINV - Display Inversion ON (opcional)

  ST7796_SendCommand(_display, 0x13); // NORON - Normal display mode ON

  ST7796_SendCommand(_display, 0x29); // DISPON - Display ON
  HAL_Delay(10);
}

void j3_ST7796_setRotation(TDisplayST7796 *_display, uint8_t _rotation)
{
  uint16_t madctl = 0;

  switch (_rotation) {
    case 0: // 0 degrees (Portrait)
      madctl = 0x0000;
      break;
    case 1: // 90 degrees (Landscape)
      //madctl = 0x2000;
      madctl = 0x2800;
      break;
    case 2: // 180 degrees (Portrait inverted)
      madctl = 0xC000;
      break;
    case 3: // 270 degrees (Landscape inverted)
      madctl = 0xA000;
      break;
    default:

      break;
  }

  ST7796_SendCommand(_display, 0x36); // MADCTL
  ST7796_SendData16(_display, &madctl, 1);
}

void j3_ST7796_setTransparencia(TDisplayST7796 *_display, uint16_t _cor)
{
  _display->temTranparencia = true;
  _display->transparenciaCor = _cor;
}

void j3_ST7796_setNoTransparencia(TDisplayST7796 *_display)
{
  _display->temTranparencia = false;
}

void j3_ST7796_setBackground(TDisplayST7796 *_display, uint16_t _cor)
{
  _display->backgroundCor = _cor;
}

// Função para desenhar um pixel na tela
void j3_ST7796_drawPixel(TDisplayST7796 *_display, uint16_t x, uint16_t y, uint16_t color) {
  if (x >= ST7796_WIDTH || y >= ST7796_HEIGHT) return;

  ST7796_SendCommand(_display, 0x2A); // CASET - Column Address Set
  uint16_t data[2];
  data[0] = x;
  data[1] = x;
  ST7796_SendData16(_display, data, 2);

  ST7796_SendCommand(_display, 0x2B); // RASET - Row Address Set
  data[0] = y;
  data[1] = y;
  ST7796_SendData16(_display, data, 2);

  ST7796_SendCommand(_display, 0x2C); // RAMWR - Memory Write
  data[0] = color;
  ST7796_SendData16(_display, data, 1);
}

void j3_ST7796_drawBitmap(TDisplayST7796 *_display, uint16_t _x, uint16_t _y, uint16_t _largura, uint16_t _altura, const uint16_t *_bitmap)
{
  uint16_t x2;
  uint16_t y2;

  x2 = _x + _largura - 1;
  if (x2 > ST7796_WIDTH)
    x2 = ST7796_WIDTH;

  y2 = _y + _altura -1;
  if(y2 > ST7796_HEIGHT)
    y2 = ST7796_HEIGHT;


  ST7796_setWindow(_display, _x, _y, x2, y2);
  ST7796_SendCommand(_display, 0x2C); // RAMWR - Memory Write
  uint32_t contBitmap = 0;
  uint16_t contBuffer = 0;
  bool flagEnviouDMA = false;

  while( contBitmap < (_largura * _altura)){
      flagEnviouDMA = false;
      if (_display->temTranparencia && (_bitmap[contBitmap] == _display->transparenciaCor)){
	  _display->buffer[contBuffer] = _display->backgroundCor;
      }else{
	  _display->buffer[contBuffer] = _bitmap[contBitmap];
      }

      if(contBuffer >= 65535){
	  ST7796_SendDataDMA(_display, contBuffer+1);
	  while(_display->dmaBusy);
	  flagEnviouDMA = true;
	  contBuffer = 0;
      }else{
	  contBuffer++;
      }
      contBitmap++;
  }
  if(!flagEnviouDMA){
      ST7796_SendDataDMA(_display, contBuffer + 1);
      while(_display->dmaBusy);
  }
}


void j3_ST7796_fillBackground(TDisplayST7796 *_display)
{
  j3_ST7796_fillScreenDMA(_display, _display->backgroundCor);
}

// Função para preencher toda a tela com uma única cor
void j3_ST7796_fillScreen(TDisplayST7796 *_display, uint16_t color) {
  uint16_t data[1];
  data[0] = color;

  ST7796_SendCommand(_display, 0x2A); // CASET - Column Address Set
  uint16_t col_data[2];
  col_data[0] = 0x0000;
  col_data[1] = (ST7796_WIDTH - 1);
  ST7796_SendData16(_display, col_data, 2);

  ST7796_SendCommand(_display, 0x2B); // RASET - Row Address Set
  uint16_t row_data[2];
  row_data[0] = 0x0000;
  row_data[1] = (ST7796_HEIGHT - 1);
  ST7796_SendData16(_display, row_data, 2);

  ST7796_SendCommand(_display, 0x2C); // RAMWR - Memory Write

  HAL_GPIO_WritePin(ST7796_DC_GPIO_Port, ST7796_DC_Pin, GPIO_PIN_SET); // DC = 1 para dados

  // Envia a cor para todos os pixels

  HAL_GPIO_WritePin(ST7796_CS_GPIO_Port, ST7796_CS_Pin, GPIO_PIN_RESET); // CS = 0 para selecionar
  for (uint32_t i = 0; i < (uint32_t)ST7796_WIDTH * ST7796_HEIGHT; i++) {

      HAL_SPI_Transmit(_display->spi, (uint8_t*)data, 1, HAL_MAX_DELAY);

  }
  HAL_GPIO_WritePin(ST7796_CS_GPIO_Port, ST7796_CS_Pin, GPIO_PIN_SET);   // CS = 1 para deselecionar
}


void j3_ST7796_fillScreenDMA(TDisplayST7796 *_display, uint16_t color) {

  ST7796_SendCommand(_display, 0x2A); // CASET - Column Address Set
  uint16_t col_data[2];
  col_data[0] = 0x0000;
  col_data[1] = (ST7796_WIDTH - 1);
  ST7796_SendData16(_display, col_data, 2);

  ST7796_SendCommand(_display, 0x2B); // RASET - Row Address Set
  uint16_t row_data[2];
  row_data[0] = 0x0000;
  row_data[1] = (ST7796_HEIGHT - 1);
  ST7796_SendData16(_display, row_data, 2);

  ST7796_SendCommand(_display, 0x2C); // RAMWR - Memory Write

  uint32_t contScreen = 0;
  uint16_t contBuffer = 0;
  bool flagEnviouDMA = false;

  while( contScreen < (ST7796_WIDTH * ST7796_HEIGHT)){
      flagEnviouDMA = false;
      _display->buffer[contBuffer] = color;

      if(contBuffer >= 65535){
	  ST7796_SendDataDMA(_display, contBuffer);
	  while(_display->dmaBusy);
	  flagEnviouDMA = true;
	  contBuffer = 0;
      }else{
	  contBuffer++;
      }
      contScreen++;
  }
  if(!flagEnviouDMA){
      ST7796_SendDataDMA(_display, contBuffer);
      while(_display->dmaBusy);
  }

}


void j3_ST7796_fillWindow(TDisplayST7796 *_display, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t _cor)
{
  if (x1 > x2) return;
  if (y1 > y2) return;

  ST7796_setWindow(_display, x1, y1, x2, y2);
  ST7796_SendCommand(_display, 0x2C); // RAMWR - Memory Write

  uint32_t contScreen = 0;
  uint16_t contBuffer = 0;
  bool flagEnviouDMA = false;
  while( contScreen <  ( ((x2 - x1) + 1) * ((y2 - y1) + 1)) ){
      flagEnviouDMA = false;
      _display->buffer[contBuffer] = _cor;

      if(contBuffer >= 65535){
	  ST7796_SendDataDMA(_display, contBuffer);
	  while(_display->dmaBusy);
	  flagEnviouDMA = true;
	  contBuffer = 0;
      }else{
	  contBuffer++;
      }
      contScreen++;
  }
  if(!flagEnviouDMA){
      ST7796_SendDataDMA(_display, contBuffer);
      while(_display->dmaBusy);
  }
}


void j3_ST7796_setBufferManager(TDisplayST7796 *_display, TJ3BufferManager *_bufferManager){
  _display->bufferManager = *_bufferManager;
  _display->bufferManagerAtivo = true;
}