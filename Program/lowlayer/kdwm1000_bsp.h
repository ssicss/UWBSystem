/* #include "kdwm1000_bsp.h" */

#ifndef __KDWM1000_BSP_H
#define __KDWM1000_BSP_H

#include "drivers\stm32f4_system.h"
/*====================================================================================================*/
/*====================================================================================================*/
#define LED_R_PIN                 GPIO_Pin_14
#define LED_R_GPIO_PORT           GPIOC
#define LED_R_Set()               __GPIO_SET(LED_R_GPIO_PORT, LED_R_PIN)
#define LED_R_Reset()             __GPIO_RST(LED_R_GPIO_PORT, LED_R_PIN)
#define LED_R_Toggle()            __GPIO_TOG(LED_R_GPIO_PORT, LED_R_PIN)

#define LED_G_PIN                 GPIO_Pin_13
#define LED_G_GPIO_PORT           GPIOC
#define LED_G_Set()               __GPIO_SET(LED_G_GPIO_PORT, LED_G_PIN)
#define LED_G_Reset()             __GPIO_RST(LED_G_GPIO_PORT, LED_G_PIN)
#define LED_G_Toggle()            __GPIO_TOG(LED_G_GPIO_PORT, LED_G_PIN)

#define LED_B_PIN                 GPIO_Pin_15
#define LED_B_GPIO_PORT           GPIOC
#define LED_B_Set()               __GPIO_SET(LED_B_GPIO_PORT, LED_B_PIN)
#define LED_B_Reset()             __GPIO_RST(LED_B_GPIO_PORT, LED_B_PIN)
#define LED_B_Toggle()            __GPIO_TOG(LED_B_GPIO_PORT, LED_B_PIN)

#define KEY_PIN                   GPIO_Pin_8
#define KEY_GPIO_PORT             GPIOA
#define KEY_Read()                (__GPIO_READ(KEY_GPIO_PORT, KEY_PIN) == KEY_PIN)

extern pFunc UART1_irqEven;
/*====================================================================================================*/
/*====================================================================================================*/
void KDWM_GPIO_Config( void );
void KDWM_UART_Config( pFunc pUARTx );
void KDWM_SSD1331_Config( void );
void KDWM_DW1000_Config( void );
/*====================================================================================================*/
/*====================================================================================================*/
#endif
