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

	GPIO_InitTypeDef GPIO_InitStructure; 

    /*��������GPIO�ڵ�ʱ��*/
    REV_GPIO_CLK_ENABLE();

    /* ѡ�񰴼�1������ */ 
    GPIO_InitStructure.Pin = REV_GPIO_PIN;
    /* ��������Ϊ����ģʽ */ 
    GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;//GPIO_MODE_IT_RISING_FALLING;//GPIO_MODE_IT_RISING;	    		  GPIO_MODE_IT_RISING_FALLING
    /* �������Ų�����Ҳ������ */
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    /* ʹ������Ľṹ���ʼ������ */
    HAL_GPIO_Init(REV_GPIO_PORT, &GPIO_InitStructure); 
    /* ���� EXTI �ж�Դ ��key1 ���š������ж����ȼ�*/
    HAL_NVIC_SetPriority(REV_EXTI_IRQ, 1, 1);
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
	{ Current_bit_CNT=0; return F_decode_error; }	// ���ڹ涨�������ڣ����ؽ������
	
	
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

		Each_bit_duration[Current_bit_CNT] = GENERAL_TIM->CNT;	// �����½��ص�TIM2��������Each_bit_duration[x]��

		Current_bit_CNT ++ ;	// ����ǰ������յ�λ��+1

		// 1��������һ���½��صļ�ʱ
		// 2���ȴ���һ���½���20ms
		//---------------------------------------------------------------------
		GENERAL_TIM->CNT=0;			// ������չܽŽ��յ�һ���½��غ󣬽�TIM2��������0
	}  
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
    if(htim==(&TIM_TimeBaseStructure))
    {
		// �����յ�һ���½��غ���һ���½�������20ms�ڱ����յ�������˴κ��������Ϊ�ǳ����
		//-----------------------------------------------------------------------------
		Current_bit_CNT = 0;	// ����ǰ������յ�λ����0
		
//		GPIO_T(REV_GPIO_PORT,REV_GPIO_PIN);
		
    }
}














