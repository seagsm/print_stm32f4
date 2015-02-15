

#include "board_encoder_emulation.h"


static uint16_t u16_target_period = 0U;

static uint16_t u16_current_period;

/* Board encoder emulation init. */
BOARD_ERROR board_encoder_emulation_init(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    be_result = board_encoder_emulation_timer_init();

    return(be_result);
}

/* Start encoder emulation timer and interrupt. */
void board_encoder_emulation_start(void)
{
    /* here should be calculation of period. */


    /* Set start period. */
    /* board_encoder_emulation_set_period(ZERO_SPEED_PERIOD); start period should be set before function call. */

    /* Enable counter. */
    TIM_Cmd(TIM5, ENABLE);
    /* Enable update interrupt. */
    NVIC_EnableIRQ(TIM5_IRQn);
}

/* Stop encoder emulation timer and interrupt. */
void board_encoder_emulation_stop(void)
{
    TIM_Cmd(TIM5, DISABLE);
    NVIC_DisableIRQ(TIM5_IRQn);
}

/* Set target value of encoder period. */
void board_encoder_emulation_set_target_period(uint16_t u16_period)
{
    u16_target_period = u16_period;
}

/* Set current timer period. */
static void board_encoder_emulation_set_period(uint16_t u16_period)
{
    TIM5->ARR = u16_period;
}

/* Timer 5 update interrupt hundler. */
void TIM5_IRQHandler(void)
{/* 2uS. */
    if(TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)
    {
         TIM_ClearITPendingBit(TIM5, TIM_IT_Update);            /* Counter overflow, reset interrupt */
         board_encoder_emulation_set_period(u16_current_period);
         board_encoder_emulation_proccess();
    }
}


void board_encoder_emulation_proccess(void)
{
    /* Here is a place for generation of encoder signals. */
    if(i32_board_capture_duty > 0)
    {
        /* ++ */
        board_encoder_emulation_output(1);
    }
    else if(i32_board_capture_duty < 0)
    {
        /* -- */
        board_encoder_emulation_output(-1);
    }
    else
    {

    }

}

#if 0
void board_encoder_emulation_set_frequency(int32_t i32_freq)
{
    if(i32_freq > 8999)
    {
        u16_current_period = 315000000 / i32_freq;
    }
    else if(i32_freq < -8999)
    {
        u16_current_period = -315000000 / i32_freq;
    }
    else
    {
        u16_current_period = 35000;
    }

    if(u16_current_period > 35000)
    {
       u16_current_period = 35000; /* 35000 * (1/90MHz) = 400uS -> 2.5kHz-> /4 => 643Hz of encoder, min freq. */
    }
    else if(u16_current_period < 1000)
    {
      u16_current_period = 1000; /* 45kHz -> /4 = 11250Hz, max freq of encoder. */
    }
}
#endif

#if 1 /* This alghoritm pass half of test, till to head up.*/
void board_encoder_emulation_set_frequency(int32_t i32_freq)
{
    //i32_freq = i32_freq * 2;
    if(i32_freq > 0)
    {
      u16_current_period = (390 - (i32_freq / 256)) * 90;
    }
    else if(i32_freq < 0)
    {
        u16_current_period = (390 + (i32_freq / 256)) * 90;
    }
    else
    {
        u16_current_period = 390;
    }

    if(u16_current_period > 36000)
    {
       u16_current_period = 36000; /* 36000 * (1/90MHz) = 400uS -> 2.5kHz-> /4 => 625Hz of encoder, min freq. */
    }
    else if(u16_current_period < 1000)
    {
      u16_current_period = 1000; /* 45kHz -> /4 = 11250Hz, max freq of encoder. */
    }
}
#endif

/* Initialisation of timer for encoder emulation. */
static BOARD_ERROR board_encoder_emulation_timer_init(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    NVIC_InitTypeDef   NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    /* Enable the TIM5 global Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel                      = (unsigned char)TIM5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority    = TIMER5_PERIOD_INTERUPT_PRIORITY_GROUP;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority           = TIMER5_PERIOD_INTERUPT_SUB_PRIORITY_GROUP;
    NVIC_InitStructure.NVIC_IRQChannelCmd                   = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Init Timer1 like timer. */
    RCC_APB1PeriphClockCmd (RCC_APB1Periph_TIM5, ENABLE);

    /* Timer 5 work from 90MHz source clock. */
    /* Time Base configuration */
    TIM_TimeBaseStructure.TIM_Period        = ZERO_SPEED_PERIOD;
    TIM_TimeBaseStructure.TIM_Prescaler     = 0U;          /* Ftimer=fsys/(Prescaler+1),for Prescaler=71 ,Ftimer=1MHz */
    TIM_TimeBaseStructure.TIM_ClockDivision = 0U;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);

    TIM_ClearITPendingBit(TIM5, TIM_IT_Update);             /* Clear pending interrupt. */
    TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
    return(be_result);
}



/* Input parametr is 1 or -1 . */
static void board_encoder_emulation_output(int8_t i8_printer_step)
{
    static int8_t i8_encoder_possition_counter = 0;

  //GPIO_ToggleBits( GPIOA, GPIO_Pin_10);


    /* TODO: Here shoub be called function board_motor_step(direction). */
    board_motor_step(i8_printer_step);

    /* Round encoder. */
    i8_encoder_possition_counter = i8_encoder_possition_counter + i8_printer_step;
    if(i8_encoder_possition_counter > 3)
    {
        i8_encoder_possition_counter = 0;
    }
    else if(i8_encoder_possition_counter < 0)
    {
        i8_encoder_possition_counter = 3;
    }
    else
    {

    }

    switch (i8_encoder_possition_counter)
    {
        case 0:
            GPIO_SetBits(   GPIOB, GPIO_B_OUT_ENCODER_A);
            GPIO_SetBits(   GPIOB, GPIO_B_OUT_ENCODER_B);
            break;
        case 1:
            GPIO_SetBits(   GPIOB, GPIO_B_OUT_ENCODER_A);
            GPIO_ResetBits( GPIOB, GPIO_B_OUT_ENCODER_B);
            break;
        case 2:
            GPIO_ResetBits( GPIOB, GPIO_B_OUT_ENCODER_A);
            GPIO_ResetBits( GPIOB, GPIO_B_OUT_ENCODER_B);
            break;
        case 3:
            GPIO_ResetBits( GPIOB, GPIO_B_OUT_ENCODER_A);
            GPIO_SetBits(   GPIOB, GPIO_B_OUT_ENCODER_B);
            break;
        default:
            break;
    }
}









