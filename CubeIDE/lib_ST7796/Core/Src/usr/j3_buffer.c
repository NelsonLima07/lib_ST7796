#include "j3_buffer.h"


void j3_buffer_init(TJ3BufferManager *manager)
{
    manager->windowCount = 0;
    manager->nextPointer = 0;
    // Inicializar as janelas como inativas
    for (uint8_t i = 0; i < MAX_WINDOW; i++) {
        manager->windows[i].isPaint = false;
    }
}

bool j3_buffer_createWindow(TJ3BufferManager *manager, uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    if (manager->windowCount >= MAX_WINDOW) {
        return false; // Não é possível criar mais janelas
    }

    TJ3WindowBuffer *window = &manager->windows[manager->windowCount];
    window->x = x;
    window->y = y;
    window->width = width;
    window->height = height;
    window->buffer = &j3memBuffer[manager->nextPointer]; // Atribuir o buffer para a janela
    window->isPaint = true; // Marcar a janela como ativa

    // Atualizar o ponteiro para a próxima posição disponível no buffer
    manager->nextPointer += (width * height * sizeof(uint8_t)); // Supondo 8 bits por pixel
    manager->windowCount++;

    return true; // Janela criada com sucesso
}