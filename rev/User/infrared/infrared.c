#include "./infrared/infrared.h"

// �������������ݵ�ȫ�ֱ���
//--------------------------------------------------------
uint32_t Each_bit_duration[64] = {0};	// ��¼ÿһλ���ݵ�ʱ��

uint8_t	Current_bit_CNT = 0;			// ��ʾ��ǰ�ǵڼ�λ

uint16_t Receive_user_code_16bit = 0 ;	// ��¼16λ�û���

uint8_t  Receive_data_code_8bit = 0 ;	// ��¼8λ������

uint8_t  Receive_data_code_opposite = 0;	// ��¼8λ������ķ���
//--------------------------------------------------------

TIM_HandleTypeDef TIM_TimeBaseStructure;

// ����Ϊ���ⷢ�����غ���
//########################################################################################################
//�ں˾�ȷ��ʱ

#define		delay8_77us()	delay_us(8)
#define		delay17_53us()	delay_us(17)

// ����Ϊ���ⷢ�����غ���
//########################################################################################################

#define		delay8_77us()	delay_us(8)
#define		delay17_53us()	delay_us(17)


void Infrared_IR_Init_JX(void) 
{
//	GPIO_InitTypeDef GPIO_InitStruct; 
//	EXTI_InitTypeDef EXTI_InitStruct;
//	NVIC_InitTypeDef NVIC_InitStruct;
//	
//	/* ����PC0���� */
//	//---------------------------------------------------------------------------
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);		// ʹ��PC�˿�ʱ��											
//	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;	                // ��������ģʽ
//	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;						// PC0
//	GPIO_Init(GPIOC, &GPIO_InitStruct);	
//	//---------------------------------------------------------------------------
//	
//	/* ����PC0�����ж�ģʽ */
//	//----------------------------------------------------------------------------------
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);			// ʹ��ӳ��ʱ��
//    
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource0); // ���ж���0ӳ�䵽PC0����
//    
//	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;			// ѡ��Ϊ�ж�
//	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling; 		// �½����ж�
//	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
//	EXTI_InitStruct.EXTI_Line = EXTI_Line0;						// ѡ���ⲿ�ж���0
//	EXTI_Init(&EXTI_InitStruct);								
//	
//	 /* ����NVIC */
//	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;		// ��ռ���ȼ�
//	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;				// ��Ӧ���ȼ�
//	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;	
//	NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn; 				// �ж�ͨ�����ⲿ�ж���0
//	NVIC_Init(&NVIC_InitStruct);


	GPIO_InitTypeDef GPIO_InitStructure; 

    /*��������GPIO�ڵ�ʱ��*/
    REV_GPIO_CLK_ENABLE();

    /* ѡ�񰴼�1������ */ 
    GPIO_InitStructure.Pin = REV_GPIO_PIN;
    /* ��������Ϊ����ģʽ */ 
    GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;//GPIO_MODE_IT_RISING_FALLING;//GPIO_MODE_IT_RISING;	    		  GPIO_MODE_IT_RISING_FALLING
    /* �������Ų�����Ҳ������ */
    GPIO_InitStructure.Pull = GPIO_PULLDOWN;//GPIO_NOPULL;
    /* ʹ������Ľṹ���ʼ������ */
    HAL_GPIO_Init(REV_GPIO_PORT, &GPIO_InitStructure); 
    /* ���� EXTI �ж�Դ ��key1 ���š������ж����ȼ�*/
    HAL_NVIC_SetPriority(REV_EXTI_IRQ, 0, 0);
    /* ʹ���ж� */
    HAL_NVIC_EnableIRQ(REV_EXTI_IRQ);	

}

/*

*/
void TIM_Init(void)
{

	GENERAL_TIM_CLK_ENABLE();

	TIM_TimeBaseStructure.Instance = GENERAL_TIM;
	TIM_TimeBaseStructure.Init.Period = 20000-1;	
	TIM_TimeBaseStructure.Init.Prescaler = 84-1;
	TIM_TimeBaseStructure.Init.CounterMode=TIM_COUNTERMODE_UP;
	TIM_TimeBaseStructure.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_Base_Init(&TIM_TimeBaseStructure);

	HAL_TIM_Base_Start_IT(&TIM_TimeBaseStructure);	
    HAL_NVIC_SetPriority(GENERAL_TIM_IRQ, 0, 0);
    HAL_NVIC_EnableIRQ(GENERAL_TIM_IRQ);
}

/**
  * @brief  ��ʱ���жϺ���
	*	@note 		��
  * @retval ��
  */
void  GENERAL_TIM_INT_IRQHandler (void)
{
	HAL_TIM_IRQHandler(&TIM_TimeBaseStructure);	 	
}
/**
  * @brief  �ص�����
	*	@note 		��
  * @retval ��
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	uint16_t TIM2_IT_Update_Cnt = 0 ;		// TIM2���������������LED1����˸��ʱ
    if(htim==(&TIM_TimeBaseStructure))
    {
		// �����յ�һ���½��غ���һ���½�������20ms�ڱ����յ�������˴κ��������Ϊ�ǳ����
		//-----------------------------------------------------------------------------
		Current_bit_CNT = 0;	// ����ǰ������յ�λ����0
		
		TIM2_IT_Update_Cnt ++ ;
		
		if( TIM2_IT_Update_Cnt >= 25 )		// ������˸���ʣ�1s
		{
			TIM2_IT_Update_Cnt = 0 ;
			
		}
		
		
    }
}





// ����NEC�����ʽ����Ϣ
// ע��Э�����ʱ��������Ҫ���������޶��ж�����
// ע��TIM2�ļ�������Ϊ1us�����ֵΪ20000
//------------------------------------------------------------------------------------------
uint8_t NEC_IR_decode_message(void)
{
	uint8_t NEC_decode_cnt;
	
	// Э��������ı�׼����Ϊ13.5ms
	// �ж�������ĳ����Ƿ���(12ms,15ms)������
	//-----------------------------------------------------------
	if( Each_bit_duration[1]<12000 || Each_bit_duration[1]>15000 )
	{ Current_bit_CNT=0;return F_decode_error; }	// ���ڹ涨�������ڣ����ؽ������
	
	
	// ����16λ�û���
	//---------------------------------------------------------
	for(NEC_decode_cnt=2; NEC_decode_cnt<18; NEC_decode_cnt++)
	{
		// Э������"1"�ı�׼����Ϊ2.24ms
		// Э������λ�ĳ�����(2ms,2.5ms)������ => Э������"1"
		//-------------------------------------------------------------------------------------
		if( Each_bit_duration[NEC_decode_cnt]>2000 && Each_bit_duration[NEC_decode_cnt]<2500 )
		{ 
			Receive_user_code_16bit >>= 1;
			
			Receive_user_code_16bit |= 0x8000;
		}
		
		// Э������"0"�ı�׼����Ϊ1.12ms
		// Э������λ�ĳ�����(1ms,1.25ms)������ => Э������"0"
		//------------------------------------------------------------------------------------------
		else if( Each_bit_duration[NEC_decode_cnt]>1000 && Each_bit_duration[NEC_decode_cnt]<1250 )
		{
			Receive_user_code_16bit >>= 1;
			
			Receive_user_code_16bit &= ~0x8000;
		}
		
		// Э������λ�����޶���Χ�ڣ�����
		//-----------------------------
		else { return F_decode_error; }
	}
	
	// ����8λ������
	//---------------------------------------------------------
	for(NEC_decode_cnt=18; NEC_decode_cnt<26; NEC_decode_cnt++)
	{
		// Э������"1"�ı�׼����Ϊ2.24ms
		// Э������λ�ĳ�����(2ms,2.5ms)������ => Э������"1"
		//-------------------------------------------------------------------------------------
		if( Each_bit_duration[NEC_decode_cnt]>2000 && Each_bit_duration[NEC_decode_cnt]<2500 )
		{ 
			Receive_data_code_8bit >>= 1;
			
			Receive_data_code_8bit |= 0x80;
		}
		
		// Э������"0"�ı�׼����Ϊ1.12ms
		// Э������λ�ĳ�����(1ms,1.25ms)������ => Э������"0"
		//------------------------------------------------------------------------------------------
		else if( Each_bit_duration[NEC_decode_cnt]>1000 && Each_bit_duration[NEC_decode_cnt]<1250 )
		{
			Receive_data_code_8bit >>= 1;
			
			Receive_data_code_8bit &= ~0x80;
		}
		
		// Э������λ�����޶���Χ�ڣ�����
		//-----------------------------
		else { return F_decode_error; }
	}
	
	// ����8λ������ķ���
	//---------------------------------------------------------
	for(NEC_decode_cnt=26; NEC_decode_cnt<34; NEC_decode_cnt++)
	{
		// Э������"1"�ı�׼����Ϊ2.24ms
		// Э������λ�ĳ�����(2ms,2.5ms)������ => Э������"1"
		//-------------------------------------------------------------------------------------
		if( Each_bit_duration[NEC_decode_cnt]>2000 && Each_bit_duration[NEC_decode_cnt]<2500 )
		{ 
			Receive_data_code_opposite >>= 1;
			
			Receive_data_code_opposite |= 0x80;
		}
		
		// Э������"0"�ı�׼����Ϊ1.12ms
		// Э������λ�ĳ�����(1ms,1.25ms)������ => Э������"0"
		//------------------------------------------------------------------------------------------
		else if( Each_bit_duration[NEC_decode_cnt]>1000 && Each_bit_duration[NEC_decode_cnt]<1250 )
		{
			Receive_data_code_opposite >>= 1;
			
			Receive_data_code_opposite &= ~0x80;
		}
		
		// Э������λ�����޶���Χ�ڣ�����
		//-----------------------------
		else { return F_decode_error; }
	}
	
	return F_decode_success;	// ����ɹ�	
}

void REV_IRQHandler(void)
{
  //ȷ���Ƿ������EXTI Line�ж�
	if(__HAL_GPIO_EXTI_GET_IT(REV_GPIO_PIN) != RESET) 
	{
		//����жϱ�־λ
		__HAL_GPIO_EXTI_CLEAR_IT(REV_GPIO_PIN);     

		Each_bit_duration[Current_bit_CNT] = TIM2->CNT;	// �����½��ص�TIM2��������Each_bit_duration[x]��

		Current_bit_CNT ++ ;	// ����ǰ������յ�λ��+1

		// 1��������һ���½��صļ�ʱ
		// 2���ȴ���һ���½���20ms
		//---------------------------------------------------------------------
		TIM2->CNT=0;			// ������չܽŽ��յ�һ���½��غ󣬽�TIM2��������0
	}  
}















