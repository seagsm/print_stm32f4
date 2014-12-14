
#include "board_spi_dma.h"

static uint16_t SPIReceivedValue[2];

BOARD_ERROR board_spi_4_dma_slave_configuration(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    GPIO_InitTypeDef    GPIO_InitStructure; /* Variable used to setup the GPIO      */
    SPI_InitTypeDef     SPI_InitStructure;  /* Variable used to setup the SPI       */
    DMA_InitTypeDef     DMA_InitStructure;  /* Variable used to setup the DMA       */
    NVIC_InitTypeDef    NVIC_InitStructure; /* Variable used to setup the Interrupt */

    /* SPI4 GPIO config. */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

    /*--Enable the SPI4 periph */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI4, ENABLE);

    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType   = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd    = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_2 | GPIO_Pin_4 |GPIO_Pin_5 | GPIO_Pin_6;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOE, GPIO_PinSource2, GPIO_AF_SPI4); /* SPI4 SCK.  */
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource4, GPIO_AF_SPI4); /* SPI4 NSS. Could be used like GPIO.  */
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource5, GPIO_AF_SPI4); /* SPI4 MISO. */
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource6, GPIO_AF_SPI4); /* SPI4 MOSI. */

    /* Reset SPI Interface */
    SPI_I2S_DeInit(SPI4);

    /* SPI4 configuration */
    SPI_StructInit(&SPI_InitStructure);
    SPI_InitStructure.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode              = SPI_Mode_Slave;
    SPI_InitStructure.SPI_DataSize          = SPI_DataSize_16b;
    SPI_InitStructure.SPI_CPOL              = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA              = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS               = SPI_NSS_Soft;//SPI_NSS_Hard;
    SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    SPI_InitStructure.SPI_CRCPolynomial     = 7U;

    SPI_Init(SPI4, &SPI_InitStructure);

    /* Enable DMA2 channel4 Stream0 IRQ Channel */
    NVIC_InitStructure.NVIC_IRQChannel                      = (unsigned char)DMA2_Stream0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority    = DMA2_Stream0_PRIORITY_GROUP;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority           = DMA2_Stream0_SUB_PRIORITY_GROUP;
    NVIC_InitStructure.NVIC_IRQChannelCmd                   = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /*--Enable DMA2 clock--*/
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

    /* DMA2 - Stream0 SPI4 -> memory. */
    DMA_DeInit(DMA2_Stream0); /* Set DMA registers to default values */
    DMA_StructInit(&DMA_InitStructure);
    DMA_InitStructure.DMA_Channel               = DMA_Channel_4;
    DMA_InitStructure.DMA_PeripheralBaseAddr    = (uint32_t)&SPI4->DR; /* Address of peripheral the DMA must map to */
    DMA_InitStructure.DMA_Memory0BaseAddr       = (uint32_t)&SPIReceivedValue[0]; /* Variable to which received data will be stored */
    DMA_InitStructure.DMA_DIR                   = DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_BufferSize            = 2U; /* Buffer size */
    DMA_InitStructure.DMA_PeripheralInc         = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc             = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize    = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize        = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode                  = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority              = DMA_Priority_High;
    DMA_InitStructure.DMA_FIFOMode              = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold         = DMA_FIFOThreshold_HalfFull;
    DMA_InitStructure.DMA_MemoryBurst           = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst       = DMA_PeripheralBurst_Single;
    DMA_Init(DMA2_Stream0, &DMA_InitStructure);     /* Initialise the DMA */

    DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE);
    DMA_Cmd(DMA2_Stream0, ENABLE);                  /* Enable the DMA2 STREAM0 */
    return(be_result);
}

/* Start SPI4, SPI4_DMA interrupt. */
BOARD_ERROR board_spi_4_dma_start(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    SPI_I2S_DMACmd(SPI4, SPI_I2S_DMAReq_Rx, ENABLE);    /* Enable the SPI4 RX requests. */
    NVIC_EnableIRQ(DMA2_Stream0_IRQn);                  /* Enable DMA interrupt. */
    SPI_Cmd(SPI4, ENABLE);                              /* Enable SPI4 */

    return(be_result);
}

/* This codes found but not used.
          case 0xBFFDU :
*/
void DMA2_Stream0_IRQHandler(void)
{
    uint16_t SPI_value[2];
    PWM_CAPTURE_STATE pcsState;
    /* Reset DMA transfer complete interrupt.*/
    if (DMA_GetITStatus(DMA2_Stream0, DMA_IT_TCIF0))
    {
        GPIO_SetBits( GPIOB, GPIO_Pin_0);
        SPI_value[0] = SPIReceivedValue[0];
        SPI_value[1] = SPIReceivedValue[1];



        if( SPI_value[0] == 0x7063U)
        {
            GPIO_SetBits( GPIOG, GPIO_Pin_13);
            /* Get current state. */
            pcsState = board_capture_get_pwm_command();

            switch(SPI_value[1])
            {
                case 0xBDFFU : /* CW */
                case 0xBFFFU :
                    GPIO_SetBits( GPIOA, GPIO_Pin_10);
                    if((pcsState == PWM_CAPTURE_STOP) || (pcsState == PWM_CAPTURE_CCW_START))
                    {
                        /* Start PWM capture from CW channel. */
                        board_capture_pwm_TIM_start(PWM_CAPTURE_CW_START);
                        /* Start encoder emulation module. */
                        board_encoder_emulation_start();
                    }
                    GPIO_ResetBits( GPIOA, GPIO_Pin_10);
                    break;

                case 0xBDFBU :
                case 0xBFFBU : /* CCW */
                    GPIO_SetBits( GPIOA, GPIO_Pin_11);
                    if((pcsState == PWM_CAPTURE_STOP) || (pcsState == PWM_CAPTURE_CW_START))
                    {
                        /* Start PWM capture from CCW channel. */
                        board_capture_pwm_TIM_start(PWM_CAPTURE_CCW_START);
                        /* Start encoder emulation module.     */
                        board_encoder_emulation_start();
                    }
                    GPIO_ResetBits( GPIOA, GPIO_Pin_11);
                    break;

                case 0xBDFDU : /* STOP */
                case 0xBFFDU : /* STOP */
                    GPIO_SetBits( GPIOA, GPIO_Pin_12);
                    /* Stop encoder emulation. */
                    board_encoder_emulation_stop();
                    /* Stop PWM capture of CW. */
                    board_capture_pwm_TIM_stop();
                    GPIO_ResetBits( GPIOA, GPIO_Pin_12);
                    break;
                default:
                    break;
            }
        }
        else if( SPI_value[0] == 0x3063U)
        {
            GPIO_SetBits( GPIOG, GPIO_Pin_13);

            /* Stop PWM capture of CW. */
            board_capture_pwm_TIM_stop();
            /* Stop encoder emulation. */
            board_encoder_emulation_stop();

        }
        else
        {
            GPIO_ResetBits( GPIOG, GPIO_Pin_13);
            GPIO_SetBits( GPIOG, GPIO_Pin_14);
        }

        GPIO_ResetBits( GPIOB, GPIO_Pin_0);

        DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);
    }
}
