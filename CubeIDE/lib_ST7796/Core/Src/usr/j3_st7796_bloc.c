/*
 * j3_st7796_bloc.c
 *
 *  Created on: Aug 18, 2025
 *      Author: Nelson Lima
 */


#include "usr/j3_tiles.h"
#include "usr/j3_st7796_tile.h"
#include "usr/j3_st7796_bloc.h"
#include "usr/j3_st7796.h"



void j3_ST7796_drawBloc(TDisplayST7796 *_display, const TJ3_Bloc *_bloc, uint16_t x, uint16_t y)
{
  uint16_t calcY, calcX;

  for(uint8_t iy = 0; iy < _bloc->sizeY; iy++)
    {
      calcY = y + (iy * TILE_HEIGHT - 1);
      for(uint8_t ix = 0; ix < _bloc->sizeX; ix++){
	  calcX = x + (ix * TILE_WIDTH - 1);
	  j3_ST7796_drawTile(_display, _bloc->pTile, calcX, calcY);
      }
    }
}
