/*
* j3_buffer.h
*
*  Created on: Mar 29, 2026
*      Author: Nelson Lima
*
* Criar uma gerenciador de buffer para display.
* Selecionar o tamanho do buffer.
* Criar janelas de escrita para o buffer.
* Buffer linear. 
*
*/

#ifndef INC_USR_J3_BUFFER_H_
#define INC_USR_J3_BUFFER_H_


#include <stdbool.h>
#include <stdint.h>

#define BUFFER_SIZE 1024 * 100 // 100KB de buffer
#define MAX_WINDOW 10 

uint8_t j3memBuffer[BUFFER_SIZE]; // Buffer de memória para armazenar os dados  

typedef struct TJ3WindowBuffer{
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
    uint16_t *buffer; // Ponteiro para o buffer de dados
    bool isPaint; // Flag para indicar se a janela precisa ser desenhada
} TJ3WindowBuffer;

typedef struct TJ3BufferManager{
    TJ3WindowBuffer windows[MAX_WINDOW]; 
    uint8_t windowCount; // Contador de janelas ativas
    uint16_t nextPointer; // Ponteiro para a próxima posição disponível no buffer
} TJ3BufferManager;


void j3_buffer_init(TJ3BufferManager *manager);
bool j3_buffer_createWindow(TJ3BufferManager *manager, uint16_t x, uint16_t y, uint16_t width, uint16_t height);


//typedef struct 



#endif /* INC_USR_J3_BUFFER_H_ */