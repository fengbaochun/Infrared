#ifndef	__INFRARED_H
#define	__INFRARED_H

#include "main.h"
#include "stm32f4xx.h"


//���Ŷ���
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




// �������������ݵ�ȫ�ֱ���
//------------------------------------------------------------
extern uint32_t Each_bit_duration[64];		// ��¼ÿһλ���ݵ�ʱ��

extern uint8_t  Current_bit_CNT;				// ��ǰ������յ�λ��

extern uint16_t Receive_user_code_16bit ;	// ��¼16λ�û���

extern uint8_t  Receive_data_code_8bit ;		// ��¼8λ������

extern uint8_t  Receive_data_code_opposite ;	// ��¼8λ������ķ���
//------------------------------------------------------------

#define	F_decode_error		1			// �������
#define	F_decode_success	0			// ����ɹ�



void Infrared_IE_Init_JX(void);	// ��ʼ�����ⷢ��ܽţ�Infrared_IE = PC1

void NEC_IE_Send_zero(void);	// NECЭ������"0"

void NEC_IE_Send_one(void);		// NECЭ������"1"

void NEC_IE_One_Data(uint8_t IE_One_Data);	// ��һ֡���ݵ���Ϊ�����ز������ȥ

void NEC_IE_code_message(uint16_t user_code, uint8_t data_code);	// ����NEC�����ʽ����Ϣ

void Infrared_IR_Init_JX(void);	// ��ʼ�����ⷢ��ܽţ�Infrared_IR = PC0

uint8_t NEC_IR_decode_message(void);	// ����NEC�����ʽ����Ϣ

void TIM_Init(void);




	
#endif	/* __INFRARED_H */
