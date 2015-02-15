/*
    This file consist function to fulfill requerement for capture PWM from TIM2CH1
    and TIM3CH1
*/

#include "board_capture.h"

        uint16_t u16_board_capture_duty_TIM2_value;
        uint16_t u16_board_capture_duty_TIM3_value;

#define DUTY_ARRAY 50

static  uint16_t u16_i = 0;
static  int16_t i16_duty_array[DUTY_ARRAY];
        int32_t i32_board_capture_duty;

static PWM_CAPTURE_STATE board_capture_command = PWM_CAPTURE_STOP;

static void board_capture_duty_TIM2_filter( uint16_t u16_duty)
{
    i32_board_capture_duty = i32_board_capture_duty - i16_duty_array[u16_i];
    i16_duty_array[u16_i]  = (int16_t)u16_duty;
    i32_board_capture_duty = i32_board_capture_duty + i16_duty_array[u16_i];
    u16_i++;
    if(u16_i >= DUTY_ARRAY )
    {
        u16_i = 0;
    }
}

static void board_capture_duty_TIM3_filter( uint16_t u16_duty)
{
    int16_t i16_duty;
    i16_duty = (int32_t)u16_duty;

    i16_duty = 0 - i16_duty;
    i32_board_capture_duty = i32_board_capture_duty - i16_duty_array[u16_i];/* Emulation of negative value */
    i16_duty_array[u16_i]  = i16_duty;
    i32_board_capture_duty = i32_board_capture_duty + i16_duty_array[u16_i];/* Emulation of negative value */
    u16_i++;

    if(u16_i >= DUTY_ARRAY )
    {
        u16_i = 0;
    }
}



/* Get TIM2 captured PWM duty. */
uint16_t board_capture_pwm_TIM2_duty(void)
{
    return(u16_board_capture_duty_TIM2_value);
}

/* Get TIM3 captured PWM duty. */
uint16_t board_capture_pwm_TIM3_duty(void)
{
    return(u16_board_capture_duty_TIM3_value);
}

/* Get current state of PWM capture.*/
PWM_CAPTURE_STATE board_capture_get_pwm_command(void)
{
    return(board_capture_command);
}

/* Input parameter are pcs_capture_state CW or CCW. */
void board_capture_pwm_TIM_start(void)
{

    /* Enable counter. */
    TIM_Cmd(TIM2, ENABLE);
    TIM_Cmd(TIM3, ENABLE);
}

/* Initialisation of capture module. */
BOARD_ERROR be_board_capture_pwm_init(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    board_capture_gpio_TIM2_configuration();/* Initialisation of GPIO's for capture PWM. */
    board_capture_gpio_TIM3_configuration();/* Initialisation of GPIO's for capture PWM. */

    board_capture_TIM2_NVIC_Configuration();
    board_capture_TIM3_NVIC_Configuration();

    board_capture_tim_configuration();  /* Configuretion of capture timers. */


    return(be_result);
}


/* Input capture GPIO initialisation. */
static void board_capture_gpio_TIM2_configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB, ENABLE);
    /* Connect clock to port A for TIM2 CH1, pin PA5). */
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd    = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_5;  /*PA5 -> TIM2_CH1 */
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Connect TIM2 pins to AF1 */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_TIM2);
}


/* Input capture GPIO initialisation. */
static void board_capture_gpio_TIM3_configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB, ENABLE);
    /* Connect clock to port B for TIM3 CH1, pin PB4). */
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd    = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_4;  /*PB4 -> TIM3_CH1 */
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    /* Connect TIM3 pins to AF2 */
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_TIM3);
}

static void board_capture_tim_configuration(void)
{
    board_capture_tim2_configuration();
    board_capture_tim3_configuration();
}

static void board_capture_TIM2_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel                      = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority    = TIMER2_PERIOD_INTERUPT_PRIORITY_GROUP;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority           = TIMER2_PERIOD_INTERUPT_SUB_PRIORITY_GROUP;
    NVIC_InitStructure.NVIC_IRQChannelCmd                   = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

static void board_capture_TIM3_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel                      = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority    = TIMER3_PERIOD_INTERUPT_PRIORITY_GROUP;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority           = TIMER3_PERIOD_INTERUPT_SUB_PRIORITY_GROUP;
    NVIC_InitStructure.NVIC_IRQChannelCmd                   = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/* Set Timer 2 Interrupt (Input Pulse Capture) */
static void board_capture_tim2_configuration(void)
{
  /* Time base configuration */
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_ICInitTypeDef  TIM_ICInitStructure;

    /* TIM2 clock enable*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM2, DISABLE);

    TIM_TimeBaseStructure.TIM_Prescaler     = 0U;
    TIM_TimeBaseStructure.TIM_Period        = 0x4FFFU;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    /* Channel 1 Config*/
    TIM_ICInitStructure.TIM_Channel         = TIM_Channel_1;
    TIM_ICInitStructure.TIM_ICPolarity      = TIM_ICPolarity_Rising;
    TIM_ICInitStructure.TIM_ICSelection     = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler     = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter        = 0x0FU;

    /* CH 1 is period, CH 2 duty. */
    TIM_PWMIConfig(TIM2, &TIM_ICInitStructure);

    /* Trigger event on. */
    TIM_SelectMasterSlaveMode(TIM2, TIM_MasterSlaveMode_Enable);

    TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE);

    /* Select trigger input PA5*/
    TIM_SelectInputTrigger(TIM2, TIM_TS_TI1FP1);

    /* Reset counter on trigger event. */
    TIM_SelectSlaveMode(TIM2, TIM_SlaveMode_Reset);

    /* Enable trigger event. */
    TIM_SelectMasterSlaveMode(TIM2, TIM_MasterSlaveMode_Enable);

    TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE);
    TIM_ITConfig(TIM2, TIM_IT_CC2, ENABLE);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    TIM_Cmd(TIM2, ENABLE);
    NVIC_EnableIRQ(TIM2_IRQn);

    DBGMCU_APB1PeriphConfig(DBGMCU_TIM2_STOP, ENABLE); /* stop counting during debug breakpoint. */
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
    TIM_TimeBaseStructure.TIM_Period        = 0x4FFFU; /* around 3 periods we wait for next pulse. */
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    /* Channel 1 Config*/
    TIM_ICInitStructure.TIM_Channel         = TIM_Channel_1;
    TIM_ICInitStructure.TIM_ICPolarity      = TIM_ICPolarity_Rising;
    TIM_ICInitStructure.TIM_ICSelection     = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler     = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter        = 0x0FU;

    /* CH 1 is period, CH 2 duty. */
    TIM_PWMIConfig(TIM3, &TIM_ICInitStructure);

    /* Turn on events from trigger.  */
    TIM_SelectMasterSlaveMode(TIM3, TIM_MasterSlaveMode_Enable);

    TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);

    /* Choose input for trigger. */
    TIM_SelectInputTrigger(TIM3, TIM_TS_TI1FP1);

    /* Reset counter for trigger event on. */
    TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Reset);

    /* Trigger event enable. */
    TIM_SelectMasterSlaveMode(TIM3, TIM_MasterSlaveMode_Enable);

    TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE);
    TIM_ITConfig(TIM3, TIM_IT_CC2, ENABLE);
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

    TIM_Cmd(TIM3, ENABLE);
    NVIC_EnableIRQ(TIM3_IRQn);


    DBGMCU_APB1PeriphConfig(DBGMCU_TIM3_STOP, ENABLE); /* stop counting during debud breakpoint. */
}

void TIM2_IRQHandler(void)
{GPIO_ToggleBits( GPIOA, GPIO_Pin_10);
    if(TIM_GetITStatus(TIM2, TIM_IT_CC1) == SET)                    /* If compare capture has occured. */
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);

        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);                 /* Clear Update counter, because we had interrupt from input pin. */
    }

    if(TIM_GetITStatus(TIM2, TIM_IT_CC2) == SET)                    /* If compare capture has occured */
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
        u16_board_capture_duty_TIM2_value   = TIM_GetCapture2(TIM2);/* Get timer counts for Duty. */
        board_capture_duty_TIM2_filter( u16_board_capture_duty_TIM2_value);
        /* TODO: here should be setup of encoder period */
        /* Set duty value for encoder period calculation. */
        board_encoder_emulation_set_frequency(i32_board_capture_duty);

        if(board_capture_command == PWM_CAPTURE_STOP)
        {
            board_capture_command = PWM_CAPTURE_CW;
            GPIO_SetBits( GPIOG, GPIO_Pin_13);                      /* Green led on. */
            /* Start should be only start, do not setup encoder period. */
            board_encoder_emulation_start();
        }
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }

    if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        if( board_capture_command == PWM_CAPTURE_CW )
        {
            board_capture_command = PWM_CAPTURE_STOP;               /* Set state to stop, because we had not input interrut during timer counting.*/
            board_encoder_emulation_stop();
            GPIO_ResetBits( GPIOG, GPIO_Pin_13);                    /* Green led off. */
        }
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);                 /* Counter overflow, reset interrupt */
    }
}

void TIM3_IRQHandler(void)
{GPIO_ToggleBits( GPIOA, GPIO_Pin_10);
    if(TIM_GetITStatus(TIM3, TIM_IT_CC1) == SET)                    /* If compare capture has occured. */
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);

        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);                 /* Clear Update counter, because we had interrupt from input pin. */
    }

    if(TIM_GetITStatus(TIM3, TIM_IT_CC2) == SET)                    /* If compare capture has occured */
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
        u16_board_capture_duty_TIM3_value   = TIM_GetCapture2(TIM3);/* Get timer counts for Duty. */
        board_capture_duty_TIM3_filter(u16_board_capture_duty_TIM3_value);
        /* Set duty value for encoder period calculation. */
        board_encoder_emulation_set_frequency(i32_board_capture_duty);

        if((board_capture_command == PWM_CAPTURE_STOP))
        {
            board_capture_command = PWM_CAPTURE_CCW;
            GPIO_SetBits( GPIOG, GPIO_Pin_14);                      /* Red led on. */
            board_encoder_emulation_start();
        }
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    }

    if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
        if( board_capture_command == PWM_CAPTURE_CCW )
        {
            board_capture_command = PWM_CAPTURE_STOP;               /* Set state to stop, because we had not input interrut during timer counting.*/
            board_encoder_emulation_stop();
            GPIO_ResetBits( GPIOG, GPIO_Pin_14);                    /* Red led off. */
        }
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);                 /* Counter overflow, reset interrupt */

    }
}














