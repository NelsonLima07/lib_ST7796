/*
 * j3_st7796_tile.h
 *
 *  Created on: Aug 12, 2025
 *      Author: Nelson Lima
 */

#ifndef INC_USR_J3_ST7796_TILE_H_
#define INC_USR_J3_ST7796_TILE_H_


#include "usr/j3_st7796.h"

#define TILE_WIDTH  8
#define TILE_HEIGHT 8


void j3_ST7796_redrawTile(TDisplayST7796 *_display, const uint16_t *_tile, uint16_t x, uint16_t y);
void j3_ST7796_drawTile(TDisplayST7796 *_display, const uint16_t *_tile, uint16_t x, uint16_t y);
void j3_ST7796_eraseTile(TDisplayST7796 *_display, const uint16_t *_tile, uint16_t x, uint16_t y);


#endif /* INC_USR_J3_ST7796_TILE_H_ */
