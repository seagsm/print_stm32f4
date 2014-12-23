#ifndef BOARD_MOTOR_H
#define BOARD_MOTOR_H 1


#include "stm32f4xx.h"
#include "board_system_type.h"
#include "board_NVIC.h"
#include "board_gpio_table.h"

#if 0

#define BOARD_MOTOR_STEP_PERIOD     1384U; /* 15.3(7)uS or 65000Hz. */
#define BOARD_MOTOR_STEP_DUTY       400U;  /* 400*(1/90MHz) = 4.5uS => duty(inverted) = 15.38uS - 4.5uS = 10.97uS */

#else
/* So, for emulation PRN PWM, period = 65uS/(1/90MHz) = 5850 */
/* and duty = (65us - 8uS)/(1/90MHz) = 5130 */
#define BOARD_MOTOR_STEP_PERIOD     5850U; 
#define BOARD_MOTOR_STEP_DUTY       5130;//3870U(22.0)//4140U(19);//4320U(17.0);//4842U(11.2);//5112(8.2);//5130U(8);//4050(20);

#endif
       BOARD_ERROR board_motor_init(void);
static        void board_motor_timer_init(void);
static        void board_motor_enable(uint8_t uint8_enable);
static        void board_motor_direction(int32_t int32_direction);

              void board_motor_step(int8_t i8_step);
              
              void TIM2_IRQHandler(void);
              void TIM3_IRQHandler(void);
              
static        void board_motor_timer_pulse_counter_init(void);              
              void TIM4_IRQHandler(void);


#endif