/*
 * main_st7796.c
 *
 *  Created on: Aug 6, 2025
 *      Author: Nelson Lima
 */
#include <stdbool.h>

#include "usr/main_st7796.h"
#include "usr/j3_st7796.h"
#include "main.h"
#include "usr/j3_tiles.h"
#include "usr/j3_st7796_tile.h"
#include "usr/j3_st7796_bloc.h"


#define QTD_BLOCOS 112 // 14 Colunas x 8 Linhas;
#define ESPACAMENTO_BLOCOS 5 // 5 pixels;
#define QTD_TILES_BLOCO_H 4 // 4 TILES Formam um bloco na Horizontal;
#define QTD_TILES_BLOCO_V 2 // 2 TILES Formam um bloco na Vertical;
#define TAM_BLOCO_H  QTD_TILES_BLOCO_H * TILE_WIDTH // Tamanho do bloco é (qtd de tiles) x (altura tile);


extern SPI_HandleTypeDef hspi1;
TDisplayST7796 *display;


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

TGameBloco blocos[QTD_BLOCOS];


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

  display = j3_ST7796_new(&hspi1, true);
  j3_ST7796_init(display);

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
  blocoAzul.pTile = bloco_azul_8x8;
  blocoAzul.sizeX = 5;
  blocoAzul.sizeY = 2;

  TJ3_Bloc blocoVerde;
  blocoAzul.pTile = bloco_azul_8x8;
  blocoAzul.sizeX = 4;
  blocoAzul.sizeY = 2;

  for(uint8_t i = 0; i < 5; i++){
      blocos[i].pBloc = &blocoAzul;
      blocos[i].posX = ESPACAMENTO_BLOCOS + (i * TAM_BLOCO_H) + (i * ESPACAMENTO_BLOCOS);
      blocos[i].posY = 200;
      blocos[i].vivo = true;
  }

  j3_ST7796_fillBackground(display);
  j3_ST7796_setTransparencia(display, 0x0000);

  /*
   *
  HAL_Delay(2000);
  j3_ST7796_fillScreenDMA(display, 0x0FF0);
  HAL_Delay(2000);
  j3_ST7796_fillBackground(display);
   */

  //j3_ST7796_drawTile(display, bola_azul_8x8, bola.posX, bola.posY);
  for(uint8_t i = 0; i < 5; i++){
      if(blocos[i].vivo){
	  j3_ST7796_drawBloc(display, blocos[i].pBloc, blocos[i].posX, blocos[i].posY);
      }
  }

  tempoGame_lastTick = 0;
  while(1){

      nowTick = HAL_GetTick();

      if( (nowTick - tempoGame_lastTick) > 50) {

	  if((bola.posY >= 190) && (bola.posY <= 220) ){
	      for(uint8_t i = 0; i < 5; i++){
		  if(blocos[i].vivo){
		      j3_ST7796_redrawBloc(display, blocos[i].pBloc, blocos[i].posX, blocos[i].posY);
		  }
	      }
	  }


	  j3_ST7796_eraseTile(display, bola.tile, bola.posX, bola.posY);
	  bola_movimento(&bola);
	  j3_ST7796_drawTile(display, bola.tile, bola.posX, bola.posY);

	  tempoGame_lastTick += 50;
      }

  }

}


void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
  if (hspi->Instance == SPI1) {
      HAL_GPIO_WritePin(ST7796_CS_GPIO_Port, ST7796_CS_Pin, GPIO_PIN_SET);   // CS = 1 para deselecionar
      display->dmaBusy = false;
      //HAL_GPIO_TogglePin(PLACA_LED_GPIO_Port,PLACA_LED_Pin);
  }
}
