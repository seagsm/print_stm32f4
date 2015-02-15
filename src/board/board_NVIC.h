#ifndef BOARD_NVIC_H
#define BOARD_NVIC_H 1


#include "stm32f4xx.h"

/*  Define current priority and subpriority. */
/*  #define NVIC_PRIORITY_GPOUP NVIC_PriorityGroup_0 */
/*  #define NVIC_PRIORITY_GPOUP NVIC_PriorityGroup_1 */
/*  #define NVIC_PRIORITY_GPOUP NVIC_PriorityGroup_2 */
/*  #define NVIC_PRIORITY_GPOUP NVIC_PriorityGroup_3 */  /* preemption 0-7 , sub 0-1 */
  #define NVIC_PRIORITY_GPOUP NVIC_PriorityGroup_4       /* just priority 0-15*/
 /* It means that priority group can be 0-15, but subpriority only 0. */

/*TIMER2 ****************************************/
#define TIMER2_PERIOD_INTERUPT_PRIORITY_GROUP       1U /* PWM capture. */
#define TIMER2_PERIOD_INTERUPT_SUB_PRIORITY_GROUP   0U
/*TIMER3 ****************************************/
#define TIMER3_PERIOD_INTERUPT_PRIORITY_GROUP       1U /* PWM capture. */
#define TIMER3_PERIOD_INTERUPT_SUB_PRIORITY_GROUP   0U

/*TIMER4 ****************************************/
#define TIMER4_PERIOD_INTERUPT_PRIORITY_GROUP       4U /* PWM out counter. */
#define TIMER4_PERIOD_INTERUPT_SUB_PRIORITY_GROUP   0U

/*TIMER5 ****************************************/
#define TIMER5_PERIOD_INTERUPT_PRIORITY_GROUP       3U /* Encoder output. */
#define TIMER5_PERIOD_INTERUPT_SUB_PRIORITY_GROUP   0U


void NVIC_init(void);

#endif

