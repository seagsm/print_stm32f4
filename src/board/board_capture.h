#ifndef BOARD_CAPTURE_H
#define BOARD_CAPTURE_H 1


#include "stm32f4xx.h"
#include "board_system_type.h"
#include "board_NVIC.h"
#include "board_encoder_emulation.h"
#include "board_sys_tick.h"


extern uint16_t u16_board_capture_duty_value;
/* extern uint16_t u16_board_capture_period_value; */

BOARD_ERROR be_board_capture_pwm_init(void);

void board_capture_pwm_TIM_start(PWM_CAPTURE_STATE pcs_capture_state);
void board_capture_pwm_TIM_stop(void);

static void TIM3_Configuration(void);

static void board_capture_gpio_configuration(void);
static void board_capture_nvic_configuration(void);
static void board_capture_tim_configuration(void);
static void board_capture_tim3_configuration(void);

static void board_capture_dma1_ch5_stream4_init(void);
static void board_capture_dma1_ch5_stream5_init(void);

void TIM3_IRQHandler(void);
void test(void);

PWM_CAPTURE_STATE board_capture_get_pwm_command(void);
void board_capture_set_pwm_command(PWM_CAPTURE_STATE command);

uint16_t board_capture_pwm_TIM3_duty(void);

#endif