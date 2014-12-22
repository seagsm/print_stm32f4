#ifndef BOARD_ENCODER_EMULATION_H
#define BOARD_ENCODER_EMULATION_H 1


#include "stm32f4xx.h"
#include "board_system_type.h"
#include "board_NVIC.h"
#include "board_capture.h"
#include "board_gpio_table.h"
#include "board_motor.h"

#define ZERO_SPEED_PERIOD   35000U
#define PID_PROPORTIONAL    10   /* from 4000 to 50 around 4500mS */

       BOARD_ERROR board_encoder_emulation_init(void);
static BOARD_ERROR board_encoder_emulation_timer_init(void);
static        void board_encoder_emulation_set_period(uint16_t u16_period);

              void board_encoder_emulation_set_frequency(int32_t i32_freq);

static        void board_encoder_emulation_proccess(void);

void board_encoder_emulation_start(void);
              void board_encoder_emulation_stop(void);

              void board_encoder_emulation_set_target_period(uint16_t u16_period);
static        void board_encoder_emulation_output(int8_t i8_printer_step);


void TIM5_IRQHandler(void);






#endif