#ifndef BOARD_CAPTURE_H
#define BOARD_CAPTURE_H 1


#include "stm32f4xx.h"
#include "board_system_type.h"
#include "board_NVIC.h"
#include "board_encoder_emulation.h"
#include "board_sys_tick.h"

extern uint16_t u16_board_capture_duty_TIM2_value;
extern uint16_t u16_board_capture_duty_TIM3_value;
extern int32_t i32_board_capture_duty;

BOARD_ERROR be_board_capture_pwm_init(void);

void board_capture_pwm_TIM_start(void);

static void TIM3_Configuration(void);

static void board_capture_gpio_TIM2_configuration(void);
static void board_capture_gpio_TIM3_configuration(void);
static void board_capture_nvic_configuration(void);
static void board_capture_tim_configuration(void);
static void board_capture_tim2_configuration(void);
static void board_capture_tim3_configuration(void);


static void board_capture_TIM2_NVIC_Configuration(void);
static void board_capture_TIM3_NVIC_Configuration(void);

void TIM2_IRQHandler(void);
void TIM3_IRQHandler(void);
void test(void);

PWM_CAPTURE_STATE board_capture_get_pwm_command(void);
void board_capture_set_pwm_command(PWM_CAPTURE_STATE command);

uint16_t board_capture_pwm_TIM2_duty(void);
uint16_t board_capture_pwm_TIM3_duty(void);
static void board_capture_duty_TIM2_filter( uint16_t u16_duty);
static void board_capture_duty_TIM3_filter( uint16_t u16_duty);



#endif