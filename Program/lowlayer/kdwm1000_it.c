/*====================================================================================================*/
/*====================================================================================================*/
#include "drivers\stm32f4_system.h"

#include "kdwm1000_bsp.h"

#include "uCommon.h"



#include "stm32f4_usart.h"

#include "uSVShell.h"

extern char cmd_history[SHELL_CMD_COUNT_MAX][SHELL_CMD_LEN_MAX];
extern struct QUEUE *gUartQueue;
/*====================================================================================================*/
/*====================================================================================================*/
void NMI_Handler( void ) { while(1); }
void HardFault_Handler( void ) { while(1); }
void MemManage_Handler( void ) { while(1); }
void BusFault_Handler( void ) { while(1); }
void UsageFault_Handler( void ) { while(1); }
void SVC_Handler( void ) {}
void DebugMon_Handler( void ) {}
void PendSV_Handler( void ) {}
void SysTick_Handler( void ) { HAL_IncTick(); }
/*====================================================================================================*/
/*====================================================================================================*/
//void WWDG_IRQHandler( void )
//void PVD_IRQHandler( void )
//void TAMP_STAMP_IRQHandler( void )
//void RTC_WKUP_IRQHandler( void )
//void FLASH_IRQHandler( void )
//void RCC_IRQHandler( void )
//void EXTI0_IRQHandler( void )
//void EXTI1_IRQHandler( void )
//void EXTI2_IRQHandler( void )
//void EXTI3_IRQHandler( void )
//void EXTI4_IRQHandler( void )
//void DMA1_Stream0_IRQHandler( void )
//void DMA1_Stream1_IRQHandler( void )
//void DMA1_Stream2_IRQHandler( void )
//void DMA1_Stream3_IRQHandler( void )
//void DMA1_Stream4_IRQHandler( void )
//void DMA1_Stream5_IRQHandler( void )
//void DMA1_Stream6_IRQHandler( void )
//void ADC_IRQHandler( void )
//void EXTI9_5_IRQHandler( void )
//void TIM1_BRK_TIM9_IRQHandler( void )
//void TIM1_UP_TIM10_IRQHandler( void )
//void TIM1_TRG_COM_TIM11_IRQHandler( void )
//void TIM1_CC_IRQHandler( void )
//void TIM2_IRQHandler( void )
//void TIM3_IRQHandler( void )
//void TIM4_IRQHandler( void )
//void I2C1_EV_IRQHandler( void )
//void I2C1_ER_IRQHandler( void )
//void I2C2_EV_IRQHandler( void )
//void I2C2_ER_IRQHandler( void )
//void SPI1_IRQHandler( void )
//void SPI2_IRQHandler( void )

bool _is_uart_handle = false;



void USART1_IRQHandler( void )
{
	
	unsigned char uart_dt_8bit = 0;
	size_t data_null = 0;
	unsigned short uart_dr = 0;
	static unsigned int cmd_history_index = 0;
	unsigned int i=0;
	
  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
	
		
	uart_dr = (uint16_t)(USART1->DR & (uint16_t)0x01FF);
	uart_dt_8bit = (unsigned char)uart_dr;

  	UART_SendByte(USART1, &uart_dt_8bit);




  	if((uart_dr == '\n') || (uart_dr == '\r')){
		_is_uart_handle = true;
		cmd_history_index = 0;
	}
	else if(uart_dr == 0x03){
		//ctrl+c
		QueuePush(gUartQueue, uart_dr);
		_is_uart_handle = true;
	}
	else if(uart_dr == 0x7f){
		//backspace
		QueuePopBack(gUartQueue, &data_null);
	}
	else if(uart_dr == 0x41){
		// 向上
		if(cmd_history[cmd_history_index][0]){
			
			printf("\r");
			for(i=0; i<QueueGetItemCount(gUartQueue); i++){
				printf("\x20");
			}
			printf("\r#");
			
			SEGGER_RTT_printf(0, "i=%d, size = %d\n\r", cmd_history_index,QueueGetItemCount(gUartQueue));
			printf("%s", &cmd_history[cmd_history_index][0]);

			QueueClear(gUartQueue);
			for(i=0; i<strlen(&cmd_history[cmd_history_index][0]); i++)
			{
				QueuePush(gUartQueue, cmd_history[cmd_history_index][i]);
			}
			QueuePush(gUartQueue, 0);
			
			cmd_history_index++;
			
		}else{
			printf("\x07");
		}
	}
	else if(uart_dr == 0x42){
		//向下
		if(cmd_history_index){

			if(cmd_history[cmd_history_index-1][0]){
			
				printf("\r");
				for(i=0; i<QueueGetItemCount(gUartQueue); i++){
					printf("\x20");
				}
				printf("\r#");
				
				SEGGER_RTT_printf(0, "i=%d, size = %d\n\r", cmd_history_index,QueueGetItemCount(gUartQueue));
				printf("%s", &cmd_history[cmd_history_index-1][0]);
		
				QueueClear(gUartQueue);
				for(i=0; i<strlen(&cmd_history[cmd_history_index-1][0]); i++)
				{
					QueuePush(gUartQueue, cmd_history[cmd_history_index-1][i]);
				}
				QueuePush(gUartQueue, 0);
				
				cmd_history_index--;
			
			}else{
				printf("\x07");
			}
		}else{
				printf("\x07");
		}
		

	}
	else{
		QueuePush(gUartQueue, uart_dr);
	}




	//cmd_history
  }

  USART_ClearFlag(USART1, USART_IT_RXNE);
}
//void USART2_IRQHandler( void )
//void EXTI15_10_IRQHandler( void )
//void RTC_Alarm_IRQHandler( void )
//void OTG_FS_WKUP_IRQHandler( void )
//void DMA1_Stream7_IRQHandler( void )
//void SDIO_IRQHandler( void )
//void TIM5_IRQHandler( void )
//void SPI3_IRQHandler( void )
//void DMA2_Stream0_IRQHandler( void )
//void DMA2_Stream1_IRQHandler( void )
//void DMA2_Stream2_IRQHandler( void )
//void DMA2_Stream3_IRQHandler( void )
//void DMA2_Stream4_IRQHandler( void )
//void OTG_FS_IRQHandler( void )
//void DMA2_Stream5_IRQHandler( void )
//void DMA2_Stream6_IRQHandler( void )
//void DMA2_Stream7_IRQHandler( void )
//void USART6_IRQHandler( void )
//void I2C3_EV_IRQHandler( void )
//void I2C3_ER_IRQHandler( void )
//void FPU_IRQHandler( void )
//void SPI4_IRQHandler( void )
//void SPI5_IRQHandler( void )
/*====================================================================================================*/
/*====================================================================================================*/
