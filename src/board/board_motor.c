


#include "board_motor.h"

static int32_t int32_possition;

BOARD_ERROR board_motor_init(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    board_motor_timer_init();
    board_motor_timer_pulse_counter_init();
    board_motor_direction(CW);    
    board_motor_enable(ENABLE);
    return(be_result);
}

/* Add one step to step counter. */
void board_motor_step(int8_t i8_step)
{
    int32_possition = int32_possition + i8_step;    /* Add one step to pool. */ 
    board_motor_direction(int32_possition);         /* Set direction. */
    TIM_Cmd(TIM1, ENABLE);                          /* Start step by PWM generation. */
}

/* Set direction in motor driver. */    
static void board_motor_direction(int32_t int32_direction)
{
    /* Direction should be tested. */
    if(int32_direction >= 0)
    {  
        GPIO_SetBits(GPIOG, GPIO_G_OUT_MOTOR_DIR);    
    }
    else
    {
        GPIO_ResetBits(GPIOG, GPIO_G_OUT_MOTOR_DIR);
    }
}

/* Enable - disable motor driver. */
static void board_motor_enable(uint8_t uint8_enable)
{
    if(uint8_enable == ENABLE)
    {
        GPIO_SetBits(GPIOG, GPIO_G_OUT_MOTOR_ENABLE);
    }
    else
    {
        GPIO_ResetBits(GPIOG, GPIO_G_OUT_MOTOR_ENABLE);
    }  
}

/* Initialisation of timer 1 for generation of pulses PWM for motor driver. */
static void board_motor_timer_init(void)
{   
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    
    uint16_t u16_pwm_period = BOARD_MOTOR_STEP_PERIOD;
    uint16_t u16_pwm_duty   = BOARD_MOTOR_STEP_DUTY;
    
    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOE, ENABLE);
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_TIM1,  ENABLE);    
    
    /* Connect clock to port E for TIM1 CH1, pin PE9. */
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType   = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd    = GPIO_PuPd_UP ;    
    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_9;  /*PE9 -> TIM1_CH1 */
    GPIO_Init(GPIOE, &GPIO_InitStructure);
 
    /* Connect TIM1 pins to AF1 */  
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_TIM1);

    /*Timer 1 work from 180MHz source clock.
       Time1 base configuration. InClk = 180MHz, for Prescaler = 0,   */
    /* Time1 base configuration. InClk = 90MHz,  for Prescaler = 1,   */
    TIM_TimeBaseStructure.TIM_Period                = u16_pwm_period;
    TIM_TimeBaseStructure.TIM_Prescaler             = 1;/* Ftimer=fsys(==180MHz)/(Prescaler+1),for Prescaler=1 ,Ftimer=90MHz */
    TIM_TimeBaseStructure.TIM_ClockDivision         = 0;
    TIM_TimeBaseStructure.TIM_CounterMode           = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter     = 0U;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
    
    /* PWM1 Mode configuration: Channel1 */
    TIM_OCInitStructure.TIM_OCMode          = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState     = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse           = u16_pwm_duty;
    TIM_OCInitStructure.TIM_OCPolarity      = TIM_OCPolarity_Low;
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);

    /* Turn on output triger of TIM2 to OnUpdate event. */
    TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_Update);
    TIM_SetCounter(TIM1, 0U);
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    
    DBGMCU_APB2PeriphConfig(DBGMCU_TIM1_STOP, ENABLE);
    
    TIM_Cmd(TIM1, ENABLE);
}

/* Initialisation of TIM4 for caunting of pulses from PWM, generated by TIM2. */
/* This timer should count 9 OnUpdate event from TIM2(PWM) and disable it.
 * It is neccesary for generation 9 pulses for each encoder step.
 */
static void board_motor_timer_pulse_counter_init(void)
{
    NVIC_InitTypeDef   NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    /* Enable the TIM4 global Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel                      = (unsigned char)TIM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority    = TIMER4_PERIOD_INTERUPT_PRIORITY_GROUP;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority           = TIMER4_PERIOD_INTERUPT_SUB_PRIORITY_GROUP;
    NVIC_InitStructure.NVIC_IRQChannelCmd                   = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* TIM4 clock enable*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM4, DISABLE);

    /* Time Base configuration */
    TIM_TimeBaseStructure.TIM_Period        = 8U;
    TIM_TimeBaseStructure.TIM_Prescaler     = 0U;       /* Ftimer=fsys/(Prescaler+1),for Prescaler=71 ,Ftimer=1MHz */
    TIM_TimeBaseStructure.TIM_ClockDivision = 0U;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    /* http://robocraft.ru/blog/ARM/739.html */
    /* Connect TIM4 triger input to TIM2 triger output. */
    TIM_SelectInputTrigger(TIM4, TIM_TS_ITR0);
    TIM_SelectMasterSlaveMode(TIM4, TIM_MasterSlaveMode_Enable);
    TIM_SelectSlaveMode(TIM4, TIM_SlaveMode_External1);
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);         /* Clear pending interrupt. */
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
    TIM_SetCounter(TIM4, 0U);                           /* Clear counter. */
    TIM_Cmd(TIM4, ENABLE);                              /* Enable timer. */
}

void TIM4_IRQHandler(void)
{ /* < 700nS. */
    if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
    {   
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);  /* Counter overflow, reset interrupt */ 

        if(int32_possition > 0)
        {                          
            int32_possition--;
        }
        else if(int32_possition < 0)
        {                          
            int32_possition++;
        }
        else
        {}
           
        if(int32_possition == 0) /* Zero point. Stop moving. */
        {  
            /* turn it on. */
            //TIM_Cmd(TIM1, DISABLE);
            //TIM_SetCounter(TIM1, 0U);
            
        }  
    }
}
