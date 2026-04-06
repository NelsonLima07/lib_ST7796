/*
 * main_st7796.c
 *
 *  Created on: Aug 6, 2025
 *      Author: Nelson Lima
 */
#include <stdbool.h>
#include <stdint.h>

#include "usr/main_st7796.h"
#include "usr/j3_st7796.h"
#include "main.h"
#include "usr/j3_tiles.h"
#include "usr/j3_st7796_tile.h"
#include "usr/j3_st7796_bloc.h"
#include "usr/j3_buffer.h"



#define QTD_BLOCOS_LINHA 14 // 14 Colunas  Formam uma linha de blocos;
#define QTD_BLOCOS_COLUNA 8 // 8 Linhas  Formam uma linha de blocos;
#define QTD_BLOCOS QTD_BLOCOS_LINHA * QTD_BLOCOS_COLUNA //112 blocos formam a tela
#define ESPACAMENTO_BLOCOS 5 // 5 pixels;
#define QTD_TILES_BLOCO_H 4 // 4 TILES Formam um bloco na Horizontal;
#define QTD_TILES_BLOCO_V 2 // 2 TILES Formam um bloco na Vertical;
#define TAM_BLOCO_H  QTD_TILES_BLOCO_H * TILE_WIDTH // Tamanho do bloco é (qtd de tiles) x (altura tile);


extern SPI_HandleTypeDef hspi1;

TDisplayST7796 meuDisplay;
TDisplayST7796 *display = &meuDisplay;

TJ3BufferManager bufferManager;
TJ3BufferManager *pBufferManager = &bufferManager;


struct TBall{
  const uint16_t *tile;
  uint16_t posX;
  uint16_t posY;
  int8_t velX;
  int8_t velY;
  bool vivo;
};
typedef struct TBall TBall;

struct TGameBloco{
  const TJ3_Bloc *pBloc;
  uint16_t posX;
  uint16_t posY;
  bool vivo;
};
typedef struct TGameBloco TGameBloco;

TGameBloco blocoLinha1[QTD_BLOCOS_LINHA];
TGameBloco blocoLinha2[QTD_BLOCOS_LINHA];
TGameBloco blocoLinha3[QTD_BLOCOS_LINHA];
TGameBloco blocoLinha4[QTD_BLOCOS_LINHA];
TGameBloco blocoLinha5[QTD_BLOCOS_LINHA];
TGameBloco blocoLinha6[QTD_BLOCOS_LINHA];
TGameBloco blocoLinha7[QTD_BLOCOS_LINHA];
TGameBloco blocoLinha8[QTD_BLOCOS_LINHA];
TGameBloco blocoLinha9[QTD_BLOCOS_LINHA];


static void bola_movimento(TBall *_bola)
{
  // Movimento horizontal
  int16_t calcX = _bola->posX + _bola->velX;

  if ((calcX + TILE_WIDTH) > (ST7796_WIDTH - 1)) {
      _bola->velX = _bola->velX * (-1); //
      _bola->posX = _bola->posX - ((calcX + TILE_WIDTH) - (ST7796_WIDTH - 1));
  } else if (calcX < 0) {
      _bola->velX = _bola->velX * (-1); //
      _bola->posX = -(calcX); // rebote suave
  } else {
      _bola->posX = calcX;
  }

  // Movimento vertical
  int16_t calcY = _bola->posY + _bola->velY;

  if ((calcY + TILE_HEIGHT) > (ST7796_HEIGHT - 1)) {
      _bola->velY = _bola->velY * (-1);
      _bola->posY = _bola->posY - ((calcY + TILE_HEIGHT) - (ST7796_HEIGHT - 1));
  } else if (calcY < 0) {
      _bola->velY = _bola->velY * (-1);
      _bola->posY = -(calcY); // rebote suave
  } else {
      _bola->posY = calcY;
  }
}


/*
 static uint32_t lerAD_PA0(void){
  HAL_ADC_Start(&hadc1);
  HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
  uint32_t value = HAL_ADC_GetValue(&hadc1);
  return value;
}
*/

static int8_t calcPosicaoPlay1(uint32_t _valAD){

  return 0;

}

void main_st7796()
{
  uint32_t tempoGame_lastTick, nowTick;

  //display = j3_ST7796_new(&hspi1, true);
  j3_ST7796_init(display, &hspi1);
  j3_ST7796_setBufferManager(display, pBufferManager);
  j3_ST7796_setBackground(display, 0xFF00);
  j3_ST7796_setTransparencia(display, 0xFFFF);
  j3_ST7796_fillBackground(display);

  uint16_t corFundo = 0x0000; // Preto
  while(1){


    HAL_Delay(3000);
    j3_ST7796_setBackground(display, corFundo);
    j3_ST7796_fillBackground(display);

    corFundo = corFundo += 0x000F;
  }

  TBall bola;
  bola.tile = bola_azul_8x8;
  bola.posX = 50;
  bola.posY = 50;
  bola.vivo = true;
  bola.velX = 3;
  bola.velY = 2;

  TJ3_Bloc blocoAzul;
  blocoAzul.pTile = bloco_azul_8x8;
  blocoAzul.sizeX = QTD_TILES_BLOCO_H;
  blocoAzul.sizeY = QTD_TILES_BLOCO_V;

  TJ3_Bloc blocoVermelho;
  blocoAzul.pTile = bloco_vermelho_8x8;
  blocoAzul.sizeX = 5;
  blocoAzul.sizeY = 2;

  blocoLinha1[0].pBloc = &blocoAzul;


  for(uint8_t i = 0; i < 5; i++){
    blocoLinha1[i].pBloc = &blocoVermelho;
    blocoLinha1[i].vivo = true;
    blocoLinha1[i].posX = (ESPACAMENTO_BLOCOS + TAM_BLOCO_H) * i;
    blocoLinha1[i].posY = 20;
     
	  j3_ST7796_drawBloc(display, blocoLinha1[i].pBloc, blocoLinha1[i].posX, blocoLinha1[i].posY);
  }

  tempoGame_lastTick = 0;
  while(1){

    nowTick = HAL_GetTick();

    if( (nowTick - tempoGame_lastTick) > 50) {

	    j3_ST7796_eraseTile(display, bola.tile, bola.posX, bola.posY);
	    bola_movimento(&bola);
	    j3_ST7796_drawTile(display, bola.tile, bola.posX, bola.posY);

	    tempoGame_lastTick += 50;
    }

  }

}


void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
  if (hspi->Instance == SPI1) {
      display->dmaBusy = false;
      //HAL_GPIO_TogglePin(PLACA_LED_GPIO_Port,PLACA_LED_Pin);
  }
}
