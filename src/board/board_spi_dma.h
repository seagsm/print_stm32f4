#ifndef BOARD_SPI_DMA_H
#define BOARD_SPI_DMA_H 1


#include "stm32f4xx.h"
#include "board_capture.h"

BOARD_ERROR board_spi_4_dma_slave_configuration(void);
BOARD_ERROR board_spi_4_dma_start(void);

void DMA2_Stream0_IRQHandler(void);


#endif