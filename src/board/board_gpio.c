

#include "board_gpio.h"



/* This function should set direction and mode for all pin working like GPIO. */
BOARD_ERROR be_board_gpio_init(void)
{
    BOARD_ERROR be_result = BOARD_ERR_OK;
    
    be_result |= be_board_pin_init( GPIOG, GPIO_Pin_13, GPIO_Speed_100MHz, GPIO_Mode_OUT);   /* PG13, GREEN LED. */
    be_result |= be_board_pin_init( GPIOG, GPIO_Pin_14, GPIO_Speed_100MHz, GPIO_Mode_OUT);   /* PG14, RED LED. */

    be_result |= be_board_pin_init( GPIOA, GPIO_Pin_10, GPIO_Speed_100MHz, GPIO_Mode_OUT);   /* PA10, Test pin. */
    
    GPIO_SetBits( GPIOG, GPIO_Pin_13); 
    GPIO_SetBits( GPIOG, GPIO_Pin_14); 
    
    GPIO_SetBits( GPIOA, GPIO_Pin_10); 
    
   
    
    /* Outputs GPIO. */
    be_result |= be_board_pin_init( GPIOA, GPIO_Pin_5,  GPIO_Speed_100MHz, GPIO_Mode_OUT);  /* PA5  */  /* Pulse. */
    be_result |= be_board_pin_init( GPIOG, GPIO_Pin_2,  GPIO_Speed_100MHz, GPIO_Mode_OUT);  /* PG2  */  /* Dir. */
    be_result |= be_board_pin_init( GPIOG, GPIO_Pin_3,  GPIO_Speed_100MHz, GPIO_Mode_OUT);  /* PG3 */   /* Enable. */
#if 0     
    be_result |= be_board_pin_init( GPIOB, GPIO_Pin_11, GPIO_Speed_10MHz,GPIO_Mode_Out_PP);  /* PB11 */  /* Encoder A. */
    be_result |= be_board_pin_init( GPIOB, GPIO_Pin_12, GPIO_Speed_10MHz,GPIO_Mode_Out_PP);  /* PB12 */  /* Encoder B. */
    be_result |= be_board_pin_init( GPIOC, GPIO_Pin_13, GPIO_Speed_10MHz,GPIO_Mode_Out_PP);  /* PC13 */  /* AGP emulation output. */ /* Reset value have to be "1". */

    GPIO_ResetBits( GPIOB, GPIO_Pin_0 );
    GPIO_ResetBits( GPIOB, GPIO_Pin_2 );  
    GPIO_ResetBits( GPIOB, GPIO_Pin_10);    
    GPIO_ResetBits( GPIOB, GPIO_Pin_11);    
    GPIO_ResetBits( GPIOB, GPIO_Pin_12);   
    GPIO_SetBits(   GPIOC, GPIO_Pin_13);    /* Reset value of AGP emmulator have to be "1". */

    /* Inputs GPIO. */
    be_result |= be_board_pin_init( GPIOB, GPIO_Pin_14, GPIO_Speed_10MHz,GPIO_Mode_IPU);     /* PB14 */  /* ENCODER_SIDE_END_SENSOR Input. */
    be_result |= be_board_pin_init( GPIOB, GPIO_Pin_15, GPIO_Speed_10MHz,GPIO_Mode_IPU);     /* PB15 */  /* MOTOR_SIDE_END_SENSOR Input. */
    be_result |= be_board_pin_init( GPIOA, GPIO_Pin_1,  GPIO_Speed_2MHz, GPIO_Mode_IPU);     /* PA1  */  /* Button 1. Input. */
    be_result |= be_board_pin_init( GPIOA, GPIO_Pin_2,  GPIO_Speed_2MHz, GPIO_Mode_IPU);     /* PA2  */  /* Button 2. Input. */
    be_result |= be_board_pin_init( GPIOA, GPIO_Pin_3,  GPIO_Speed_2MHz, GPIO_Mode_IPU);     /* PA3  */  /* Button 3. Input. */
    be_result |= be_board_pin_init( GPIOA, GPIO_Pin_4,  GPIO_Speed_2MHz, GPIO_Mode_IPU);     /* PA4  */  /* Button 4. Input. */
    be_result |= be_board_pin_init( GPIOA, GPIO_Pin_8,  GPIO_Speed_10MHz,GPIO_Mode_IPU);     /* PA8  */  /* PE desk Input. */

    be_result |= be_board_pin_init( GPIOC, GPIO_Pin_14, GPIO_Speed_10MHz,GPIO_Mode_IPU);     /* PC14 */  /* AGP Input. */
    be_result |= be_board_pin_init( GPIOC, GPIO_Pin_15, GPIO_Speed_10MHz,GPIO_Mode_IPU);     /* PC15 */  /* Head Gear Input. */
#endif
    
    return(be_result);
}




/*
    This function is setting GPIO pin on required mode and direction.
    GPIO_Mode_IN    GPIO Input Mode
    GPIO_Mode_OUT   GPIO Output Mode
    GPIO_Mode_AF    GPIO Alternate function Mode
    GPIO_Mode_AN    GPIO Analog Mode
*/


BOARD_ERROR be_board_pin_init(
                                GPIO_TypeDef *gpio_board_port,
                                uint16_t u16_port_pin,
                                GPIOSpeed_TypeDef gpio_speed_pin_speed,
                                GPIOMode_TypeDef gpio_mode_pin_mode
                              )
{
    BOARD_ERROR be_result = BOARD_ERR_OK;

    /* Create structure for initialisation. */
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable PORT Peripheral clock. */
    if(gpio_board_port == GPIOA)
    {
        RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA , ENABLE);
    }
    else if(gpio_board_port == GPIOB)
    {
        RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB , ENABLE);
    }
    else if(gpio_board_port == GPIOC)
    {
        RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC , ENABLE);
    }
    else if(gpio_board_port == GPIOD)
    {
        RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOD , ENABLE);
    }
    else if(gpio_board_port == GPIOE)
    {
        RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOE , ENABLE);
    }
    else if(gpio_board_port == GPIOF)
    {
        RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOF , ENABLE);
    }
    else if(gpio_board_port == GPIOG)
    {
        RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOG , ENABLE);
    }
    else if(gpio_board_port == GPIOH)
    {
        RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOH , ENABLE);
    }
    else if(gpio_board_port == GPIOI)
    {
        RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOI , ENABLE);
    }
    else if(gpio_board_port == GPIOJ)
    {
        RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOJ , ENABLE);
    }
    else if(gpio_board_port == GPIOK)
    {
        RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOK , ENABLE);
    }    
    else
    {
        be_result = BOARD_ERR_ERROR;
    }

    /* Configurate the GPIO pin structure. */
    GPIO_InitStructure.GPIO_Pin   = u16_port_pin;
    GPIO_InitStructure.GPIO_Speed = gpio_speed_pin_speed;
    GPIO_InitStructure.GPIO_Mode  = gpio_mode_pin_mode;

    /* Call GPIO init function. */
    GPIO_Init( gpio_board_port, &GPIO_InitStructure);
    return(be_result);
}
