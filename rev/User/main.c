/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2017-xx-xx
  * @brief   GPIO���--ʹ�ù̼������LED��
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F407 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx.h"
#include "./led/bsp_led.h"
#include "./key/bsp_exti.h"
#include "./infrared/infrared.h"
#include "./usart/bsp_debug_usart.h"


static void SystemClock_Config(void); 

uint16_t	user_code_16bit = 0x1234;	// ��ʼ��16λ�û���

uint8_t data_code_8bit = 0x00 ;		// ��ʼ��8λ������


int a=0;
/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{

	SystemClock_Config();
	delay_init(168);
	TIM_Init();
	Infrared_IR_Init_JX();
//	DEBUG_USART_Config();
	

	while(1)                            
	{
				// �ж��Ƿ���յ���Ч��NEC������Ϣ
		//------------------------------------------------------------------------
		if(Current_bit_CNT>=34)
		{
			// ����NEC��ʽ������Ϣ
			//------------------------------
			if( ! NEC_IR_decode_message() )
			{
				// �ж�������������뷴���Ƿ��෴
				//-----------------------------
				if( Receive_data_code_8bit == (uint8_t)(~Receive_data_code_opposite) )
				{
					// �����ظ�����
					//-------------
	//					if( user_code_16bit!=Receive_user_code_16bit || data_code_8bit!=Receive_data_code_8bit )
					{
						user_code_16bit = Receive_user_code_16bit;	// �����û���
					
						data_code_8bit  = Receive_data_code_8bit ;	// ����������
						
	//						//�����յ��ģ��û�����ֽ� / �û�����ֽ� / �����룬ͨ�����ڷ���
	//						//------------------------------------------------------------
	//						USART_SendData( USART1, user_code_16bit>>8 );
	//						while ( USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET );
	//						USART_SendData( USART1, user_code_16bit );
	//						while ( USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET );
	//						USART_SendData( USART1, data_code_8bit );
	//						while ( USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET );
	//						//------------------------------------------------------------
						Current_bit_CNT=0;
						delay_ms(200);
						
					}
				}
			}
		}
////		GPIO_T(REV_GPIO_PORT,REV_GPIO_PIN);

//		

	}
}
/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 168000000
  *            HCLK(Hz)                       = 168000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 25
  *            PLL_N                          = 336
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();
  
  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    while(1) {};
  }
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    while(1) {};
  }

  /* STM32F405x/407x/415x/417x Revision Z devices: prefetch is supported  */
  if (HAL_GetREVID() == 0x1001)
  {
    /* Enable the Flash prefetch */
    __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
