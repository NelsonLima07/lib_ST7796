/*
 * j3_st7796_bloc.h
 *
 *  Created on: Aug 18, 2025
 *      Author: Nelson Lima
 */

#ifndef INC_USR_J3_ST7796_BLOC_H_
#define INC_USR_J3_ST7796_BLOC_H_

#include "usr/j3_st7796.h"

// TJ3_Bloc é um conjunto de tiles, formando um bloco. Exemplo: um bloco de 4x2 tiles, totalizando 8 tiles, formando um bloco de 32x16 pixels.
struct TJ3_Bloc{
	const uint16_t *pTile;
	uint8_t sizeX; // Quantidade de tiles na horizontal;
	uint8_t sizeY; // Quantidade de tiles na vertical;
};
typedef struct TJ3_Bloc TJ3_Bloc;


void j3_ST7796_drawBloc(TDisplayST7796 *_display, const TJ3_Bloc *_bloc, uint16_t x, uint16_t y);
void j3_ST7796_eraseBloc(TDisplayST7796 *_display, const TJ3_Bloc *_bloc, uint16_t x, uint16_t y);
void j3_ST7796_redrawBloc(TDisplayST7796 *_display, const TJ3_Bloc *_bloc, uint16_t x, uint16_t y);

#endif /* INC_USR_J3_ST7796_BLOC_H_ */
