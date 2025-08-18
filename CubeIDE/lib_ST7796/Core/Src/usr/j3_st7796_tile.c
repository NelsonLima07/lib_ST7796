/*
 * j3_st7796_tile.c
 *
 *  Created on: Aug 12, 2025
 *      Author: Nelson Lima
 */


#include "usr/j3_st7796_tile.h"
#include "usr/j3_st7796.h"


void j3_ST7796_redrawTile(TDisplayST7796 *_display, const uint16_t *_tile, uint16_t x, uint16_t y){
  j3_ST7796_eraseTile(_display, _tile, x, y);
  j3_ST7796_drawBitmap(_display, x, y, TILE_WIDTH, TILE_HEIGHT, _tile);

}

void j3_ST7796_drawTile(TDisplayST7796 *_display, const uint16_t *_tile, uint16_t x, uint16_t y)
{
	j3_ST7796_drawBitmap(_display, x, y, TILE_WIDTH, TILE_HEIGHT, _tile);
}

void j3_ST7796_eraseTile(TDisplayST7796 *_display, const uint16_t *_tile, uint16_t x, uint16_t y)
{
	j3_ST7796_fillWindow(_display, x, y, x + TILE_WIDTH - 1, y + TILE_HEIGHT -1, _display->backgroundCor);
}
