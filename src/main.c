
/* Code style rules:                */
/* 1. Code should be an ideal.      */
/* 2. An ideal code does not exist. */
/*                 Vadym Volokitin. */

/* Inc printer stepper motor driver controller for Epson. */

#include "main.h"


 int main( void)
{
    BOARD_SYSTEM_STATE  bss_state;
    BOARD_ERROR         be_result = BOARD_ERR_OK;

    v_board_stateSetRequiredState(BOARD_SYSTEM_INIT);

    while(1U)
    {
        if(be_result == BOARD_ERR_ERROR)
        {
            v_board_stateSetRequiredState(BOARD_SYSTEM_FAULT);
        }
        bss_state = bss_board_stateGetRequiredState();
        switch (bss_state)
        {

            case BOARD_SYSTEM_INIT:

                v_board_stateUpdateCurrentState(BOARD_SYSTEM_INIT);

                be_result = be_board_init_main_init();      /* init main hardware moduls.*/

                break;

            case BOARD_SYSTEM_READY_TO_RUN:

                v_board_stateUpdateCurrentState(BOARD_SYSTEM_READY_TO_RUN);

                v_board_stateSetRequiredState(BOARD_SYSTEM_RUN);

                break;

            case BOARD_SYSTEM_RUN:
                /* One time switch a board to RUN state. */
                if(bss_board_stateGetCurrentState() != BOARD_SYSTEM_RUN )
                {
                    /* One time start function should be added here. */

                    /* Start SPI1 + DMA module. */
                    /* be_result = board_spi_4_dma_start(); */

                    v_board_stateUpdateCurrentState(BOARD_SYSTEM_RUN);

                    /* DEBUG. */
                    /* Input parameter are pcs_capture_state CW or CCW. */
                    board_capture_pwm_TIM_start();
                    board_motor_step(1);
                    //board_encoder_emulation_start();
                    //
                    /* DEBUG. */
                }
                /* Infinite start function should be added here. */
                //GPIO_SetBits( GPIOG, GPIO_Pin_13);
                //GPIO_ResetBits( GPIOG, GPIO_Pin_14);
                gv_board_sys_tick_delay(100U);
                //GPIO_SetBits( GPIOG, GPIO_Pin_14);
                //GPIO_ResetBits( GPIOG, GPIO_Pin_13);
                gv_board_sys_tick_delay(100U);

                /* DEBUG. */
                
                /* DEBUG. */

                break;

            case BOARD_SYSTEM_FAULT:

                v_board_stateUpdateCurrentState(BOARD_SYSTEM_FAULT);
                /* Flash of LED in case of FAULT. */
                GPIO_SetBits( GPIOA, GPIO_Pin_10);
                gv_board_sys_tick_delay(400U);
                GPIO_ResetBits( GPIOA, GPIO_Pin_10);
                gv_board_sys_tick_delay(400U);
                break;

            default:
                break;
        }
    }
}












#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
uint32_t assert_failed(uint8_t* file, uint32_t line)
{
    /*  User can add his own implementation to report the file name and line number,
        ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1U)
    {
        return(1U);
    }
}
#endif

