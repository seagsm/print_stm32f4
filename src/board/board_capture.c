/*
    This file consist function to fulfill requerement for capture PWM from TIM2CH1
    and TIM4CH1
*/

#include "board_capture.h"

       uint16_t u16_board_capture_duty_value;
static uint16_t u16_board_capture_period_value;

static PWM_CAPTURE_STATE board_capture_command = PWM_CAPTURE_STOP;


/* Get TIM3 captured PWM duty. */
uint16_t board_capture_pwm_TIM3_duty(void)
{
    return(u16_board_capture_duty_value);
}

/* Get current state of PWM capture.*/
PWM_CAPTURE_STATE board_capture_get_pwm_command(void)
{
    return(board_capture_command);
}

/* Input parameter are pcs_capture_state CW or CCW. */
void board_capture_pwm_TIM_start(PWM_CAPTURE_STATE pcs_capture_state)
{
    /* Set flag of module to START state. */
    board_capture_command = pcs_capture_state;
    /* Enable counter. */
    TIM_Cmd(TIM3, ENABLE);
}

/* Function set state STOP. */
void board_capture_pwm_TIM_stop(void)
{
    /* Set flad of module to STOP state. */
    board_capture_command = PWM_CAPTURE_STOP;
    /* Disable timer. */
    TIM_Cmd(TIM3, DISABLE);
}

/* Initialisation of capture module. */
BOARD_ERROR be_board_capture_pwm_init(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    board_capture_gpio_configuration(); /* Initialisation of GPIO's for capture PWM. */
    board_capture_tim_configuration();  /* Configuretion of capture timers. */
    board_capture_pwm_TIM_stop();
    return(be_result);
}

/*
    timing is :
    T = reg_l * Tau = reg_l * (1/(Fslc / (1 + Prescaler )))=
    = |If Fslc = 180000000 and Prescaler = 4 | = reg_l * ( 1/(180000000/5) )=
    = reg_l * 1 / 36000000 = reg_l * 27.7(7)nS;
*/

/* Input capture GPIO initialisation. */
static void board_capture_gpio_configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB, ENABLE);
    /* Connect clock to port B for TIM3 CH1, pin PB4). */
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd    = GPIO_PuPd_DOWN;//GPIO_PuPd_NOPULL;//GPIO_PuPd_UP ;
    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_4;  /*PB4 -> TIM3_CH1 */
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    /* Connect TIM3 pins to AF2 */
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_TIM3);
}

static void board_capture_tim_configuration(void)
{
    board_capture_tim3_configuration();
}

/* Set Timer 3 Interrupt (Input Pulse Capture) */
static void board_capture_tim3_configuration(void)
{
    /* Time base configuration */
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_ICInitTypeDef  TIM_ICInitStructure;

    /* TIM3 clock enable*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM3, DISABLE);

    TIM_TimeBaseStructure.TIM_Prescaler     = 0U;
    TIM_TimeBaseStructure.TIM_Period        = 0xFFFFU;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    /* Channel 1 Config*/
    TIM_ICInitStructure.TIM_Channel         = TIM_Channel_1;
    TIM_ICInitStructure.TIM_ICPolarity      = TIM_ICPolarity_Rising;
    TIM_ICInitStructure.TIM_ICSelection     = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler     = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter        = 0x00U;
    TIM_ICInit(TIM3, &TIM_ICInitStructure);

    /* Channel 2 Config*/
    TIM_ICInitStructure.TIM_Channel         = TIM_Channel_2;
    TIM_ICInitStructure.TIM_ICPolarity      = TIM_ICPolarity_Falling;
    TIM_ICInitStructure.TIM_ICSelection     = TIM_ICSelection_IndirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler     = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter        = 0x00U;
    TIM_ICInit(TIM3, &TIM_ICInitStructure);

    /*More*/
    TIM_SelectInputTrigger(TIM3,TIM_TS_TI1FP1);
    TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Reset);

    /* Initialize DMA to transfer CC1 to board_capture_period_value. */
    board_capture_dma1_ch5_stream4_init();
    /* Initialize DMA to transfer CC2 to board_capture_duty_value. */
    board_capture_dma1_ch5_stream5_init();
}

/* This function should initialiseTIM3 CH1 DMA CH5 STREAM4. */
static void board_capture_dma1_ch5_stream4_init(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    /* DMA1 module clk ON. */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
    /* DMA reset. */
    DMA_DeInit(DMA1_Stream4);
    /* Fill DMA init structure before initialisation. */
    DMA_InitStructure.DMA_Channel               = DMA_Channel_5;
    DMA_InitStructure.DMA_PeripheralBaseAddr    = (uint32_t)(&TIM3->CCR1);
    DMA_InitStructure.DMA_Memory0BaseAddr       = (uint32_t)(&u16_board_capture_period_value);
    DMA_InitStructure.DMA_DIR                   = DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_BufferSize            = 1;
    DMA_InitStructure.DMA_PeripheralInc         = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc             = DMA_MemoryInc_Disable;
    DMA_InitStructure.DMA_PeripheralDataSize    = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize        = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode                  = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority              = DMA_Priority_Low;
    DMA_InitStructure.DMA_FIFOMode              = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold         = DMA_FIFOThreshold_HalfFull;
    DMA_InitStructure.DMA_MemoryBurst           = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst       = DMA_PeripheralBurst_Single;
    /* Initialisation of DMA1 stream4. */
    DMA_Init(DMA1_Stream4, &DMA_InitStructure);
    /* TIM3 CH1 ->  DMA1 CH5 STR4 enable. */
    TIM_DMACmd(TIM3, TIM_DMA_CC1, ENABLE);
    /* Start DMA transfer. */
    DMA_Cmd(DMA1_Stream4, ENABLE);
}

/* This function should initialiseTIM3 CH2 DMA CH5 STREAM5. */
static void board_capture_dma1_ch5_stream5_init(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    /* DMA1 module clk ON. */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
    /* DMA reset. */
    DMA_DeInit(DMA1_Stream5);
    /* Fill DMA init structure before initialisation. */
    DMA_InitStructure.DMA_Channel               = DMA_Channel_5;
    DMA_InitStructure.DMA_PeripheralBaseAddr    = (uint32_t)(&TIM3->CCR2);
    DMA_InitStructure.DMA_Memory0BaseAddr       = (uint32_t)(&u16_board_capture_duty_value);
    DMA_InitStructure.DMA_DIR                   = DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_BufferSize            = 1;
    DMA_InitStructure.DMA_PeripheralInc         = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc             = DMA_MemoryInc_Disable;
    DMA_InitStructure.DMA_PeripheralDataSize    = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize        = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode                  = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority              = DMA_Priority_High;
    DMA_InitStructure.DMA_FIFOMode              = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold         = DMA_FIFOThreshold_HalfFull;
    DMA_InitStructure.DMA_MemoryBurst           = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst       = DMA_PeripheralBurst_Single;
    /* Initialisation of DMA1 stream5. */
    DMA_Init(DMA1_Stream5, &DMA_InitStructure);
    /* TIM3 CH2 ->  DMA1 CH5 STR5 enable. */
    TIM_DMACmd(TIM3, TIM_DMA_CC2, ENABLE);
    /* Start DMA transfer. */
    DMA_Cmd(DMA1_Stream5, ENABLE);
}

