#ifndef	__INFRARED_H
#define	__INFRARED_H

#include "main.h"
#include "stm32f4xx.h"


//引脚定义
/*******************************************************/
#define SEND_GPIO_PORT                	GPIOA
#define SEND_GPIO_CLK_ENABLE()        	__GPIOA_CLK_ENABLE();
#define SEND_GPIO_PIN                 	GPIO_PIN_0
#define SEND_EXTI_IRQ                 	EXTI0_IRQn
#define SEND_IRQHandler                 EXTI0_IRQHandler

#define REV_GPIO_PORT               	GPIOC
#define REV_GPIO_CLK_ENABLE()       	__GPIOC_CLK_ENABLE();
#define REV_GPIO_PIN                	GPIO_PIN_13
#define REV_EXTI_IRQ                	EXTI15_10_IRQn
#define REV_IRQHandler                  EXTI15_10_IRQHandler


#define GENERAL_TIM							TIM2
#define GENERAL_TIM_CLK_ENABLE()  			__TIM2_CLK_ENABLE()

#define GENERAL_TIM_IRQ                  	TIM2_IRQn
#define GENERAL_TIM_INT_IRQHandler       	TIM2_IRQHandler


/*******************************************************/

#define INF_SEND(x)					HAL_GPIO_WritePin(SEND_GPIO_PORT,SEND_GPIO_PIN,x)
				
#define INF_REV(x)					HAL_GPIO_WritePin(LED1_GPIO_PORT,LED1_PIN,x)

#define GPIO_T(p,i)					{p->ODR ^=i;}	




// 缓存红外接收数据的全局变量
//------------------------------------------------------------
extern uint32_t Each_bit_duration[64];		// 记录每一位数据的时长

extern uint8_t  Current_bit_CNT;				// 当前红外接收的位数

extern uint16_t Receive_user_code_16bit ;	// 记录16位用户码

extern uint8_t  Receive_data_code_8bit ;		// 记录8位数据码

extern uint8_t  Receive_data_code_opposite ;	// 记录8位数据码的反码
//------------------------------------------------------------

#define	F_decode_error		1			// 解码出错
#define	F_decode_success	0			// 解码成功



void Infrared_IE_Init_JX(void);	// 初始化红外发射管脚：Infrared_IE = PC1

void NEC_IE_Send_zero(void);	// NEC协议数据"0"

void NEC_IE_Send_one(void);		// NEC协议数据"1"

void NEC_IE_One_Data(uint8_t IE_One_Data);	// 将一帧数据调制为红外载波发射出去

void NEC_IE_code_message(uint16_t user_code, uint8_t data_code);	// 发送NEC编码格式的信息

void Infrared_IR_Init_JX(void);	// 初始化红外发射管脚：Infrared_IR = PC0

uint8_t NEC_IR_decode_message(void);	// 解码NEC编码格式的信息

void TIM_Init(void);




	
#endif	/* __INFRARED_H */
