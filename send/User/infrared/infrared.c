#include "./infrared/infrared.h"

// �������������ݵ�ȫ�ֱ���
//--------------------------------------------------------
uint32_t Each_bit_duration[64] = {0};	// ��¼ÿһλ���ݵ�ʱ��

uint8_t	Current_bit_CNT = 0;			// ��ʾ��ǰ�ǵڼ�λ

uint16_t Receive_user_code_16bit = 0 ;	// ��¼16λ�û���

uint8_t  Receive_data_code_8bit = 0 ;	// ��¼8λ������

uint8_t  Receive_data_code_opposite = 0;	// ��¼8λ������ķ���
//--------------------------------------------------------



// ����Ϊ���ⷢ�����غ���
//########################################################################################################
//�ں˾�ȷ��ʱ

#define		delay8_77us()	delay_us(8)
#define		delay17_53us()	delay_us(17)
//---------------------------------------

 /**
  * @brief  ���� PA0 Ϊ���жϿڣ��������ж����ȼ�
  * @param  ��
  * @retval ��
  */
void EXTI_Key_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; 


    SEND_GPIO_CLK_ENABLE();
    KEY2_INT_GPIO_CLK_ENABLE();

	/*����*/
    GPIO_InitStructure.Pin = SEND_GPIO_PIN;

    GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;	    		

    GPIO_InitStructure.Pull = GPIO_NOPULL;

    HAL_GPIO_Init(SEND_GPIO_PORT, &GPIO_InitStructure); 

}
void SEND_IRQHandler(void)
{
  //ȷ���Ƿ������EXTI Line�ж�
	if(__HAL_GPIO_EXTI_GET_IT(SEND_GPIO_PIN) != RESET) 
	{

    //����жϱ�־λ
		__HAL_GPIO_EXTI_CLEAR_IT(SEND_GPIO_PIN);     
	}  
}

void KEY2_IRQHandler(void)
{
  //ȷ���Ƿ������EXTI Line�ж�
	if(__HAL_GPIO_EXTI_GET_IT(KEY2_INT_GPIO_PIN) != RESET) 
	{

    //����жϱ�־λ
		__HAL_GPIO_EXTI_CLEAR_IT(KEY2_INT_GPIO_PIN);     
	}  
}




// ��ʼ�����ⷢ��ܽţ�Infrared_IE = PC1
//----------------------------------------------------------------------------------------------
void Infrared_IE_Init_JX(void)
{
//	GPIO_InitTypeDef GPIO_InitStruct;                       // ����һ��GPIO_InitTypeDef���͵ı���
//	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);   // ����GPIOCʱ��
//	
//	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;                  // GPIO_Pin_1
//	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;           // ͨ���������
//	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;          // 50MHz�ٶ�
//	GPIO_Init(GPIOC, &GPIO_InitStruct);
//	
//	GPIO_ResetBits(GPIOC, GPIO_Pin_1);                      //PC1����
	
	GPIO_InitTypeDef GPIO_InitStructure; 

	SEND_GPIO_CLK_ENABLE();

	/*����*/
	GPIO_InitStructure.Pin = SEND_GPIO_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;	    		
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;

	HAL_GPIO_Init(SEND_GPIO_PORT, &GPIO_InitStructure); 
	
	INF_SEND(0);
	
}
//----------------------------------------------------------------------------------------------


//*********************************** �ز����� *********************************************
//
//	26.3us��һ�����ڣ�= 8.77us��IEΪ�ߣ��������⣩+ 17.53us��IEΪ�ͣ����������⣩
//	ע��һ���ز������У���������8.77us  + ����������17.53us��������ھ����ز��������ڡ�
//	ע���ز����������У����������ռ�ձ�һ��Ϊ1/3��

//	26.3us��һ�����ڣ�= 26.3us��IEΪ�ͣ����������⣩
//	ע�����������ڣ������������⣬�������Ϊ�ز����������ڡ�

//******************************************************************************************


// NECЭ������"0"= �ز�����0.56ms + �ز�������0.56ms
//-------------------------------------------------
void NEC_IE_Send_zero(void)     
{
	uint8_t i;
	
	// �ز�����0.56ms �� 26.3us * 21
	//-------------------------------
	for(i=0;i<22;i++)
	{
        //26.3us���ز��������ڣ�
		//------------------------------------
		INF_SEND(1);	// IȨ�ߣ���������
		delay8_77us();  // ��ʱ8.77us                        
											
		INF_SEND(0);	// IE���ͣ�����������
		delay17_53us(); // ��ʱ17.53us
		//------------------------------------
	}

	
	// �ز�������0.56ms �� 26.3us * 21
	//-------------------------------
	for(i=0;i<21;i++)
	{
        //26.3us���ز����������ڣ�
		//------------------------------------
		INF_SEND(0);	// IE���ͣ�����������
		delay8_77us();  // ��ʱ8.77us
                                            
		INF_SEND(0);	// IE���ͣ�����������
		delay17_53us(); // ��ʱ17.53us
		//------------------------------------
	}
}
//-------------------------------------------------


// NECЭ������"1" = �ز�����0.56ms + �ز�������1.68ms
//-------------------------------------------------
void NEC_IE_Send_one(void)
{
	uint8_t i;
	
	// �ز�����0.56ms �� 26.3us * 21
	//-------------------------------
	for(i=0;i<22;i++)
	{
		//26.3us���ز��������ڣ�
		//------------------------------------
		INF_SEND(1);    // IȨ�ߣ���������
		delay8_77us();	// ��ʱ8.77us
		
		INF_SEND(0);  // IE���ͣ�����������
		delay17_53us();	// ��ʱ17.53us
		//------------------------------------
	}
	
	// �ز�������1.68ms �� 26.3us * 64
	//--------------------------------
	for(i=0;i<64;i++)
	{
		//26.3us���ز����������ڣ�
		//------------------------------------
		INF_SEND(0);	// IE���ͣ�����������
		delay8_77us();	// ��ʱ8.77us
		
		INF_SEND(0);	// IE���ͣ�����������
		delay17_53us();	// ��ʱ17.53us
		//------------------------------------
	}
}
//-------------------------------------------------


// ��һ֡���ݵ���ΪNECЭ��涨�ĺ����ز������ȥ
// һ֡���ݸ�ʽ����λ��ǰ���ɵ͵��߷���8λ����
// ��֡���ݿ����ǣ���16λ�û�����ֽ�(8λ) / ��16λ�û�����ֽ�(8λ) / ��8λ������ / ��8λ������ķ���
//----------------------------------------------------------------------------------------------
void NEC_IE_One_Data(uint8_t IE_One_Data)
{
	uint8_t i;
	
	for(i=0;i<8;i++)
	{
		if( IE_One_Data & 0x01 )
			NEC_IE_Send_one();
		else
			NEC_IE_Send_zero();
			
		IE_One_Data>>=1;
	}
}
//---------------------------------



// ����NEC�����ʽ����Ϣ
//----------------------------------------------------------------------------------------------------------------------
// NEC �����ʽ�� �������� + ��16λ�û�����ֽ�(8λ) + ��16λ�û�����ֽ�(8λ) + ��8λ������ + ��8λ������ķ��� + �޽����롮0�� 
//----------------------------------------------------------------------------------------------------------------------
void NEC_IE_code_message(uint16_t user_code_16bit, uint8_t data_code_8bit)
{
	uint16_t i;
	
	uint8_t T_user_code_high = user_code_16bit>>8;
	
	uint8_t T_user_code_low = user_code_16bit;

	// �������룺�ز�����9ms��������4.5ms
	//------------------------------------------------------------
	// �ز����� 9ms �� 26.3us * 342
	for(i=0;i<342;i++) 
	{
		INF_SEND(1);	// IȨ�ߣ���������
		delay8_77us();	// ��ʱ8.77us
		
		INF_SEND(0);	// IE���ͣ�����������
		delay17_53us();	// ��ʱ17.53us
	}
	
	// �ز������� 4.5ms �� 26.3us * 171
	for(i=0;i<171;i++)
	{
		INF_SEND(0);	// IE���ͣ�����������
		delay8_77us();	// ��ʱ8.77us
		
		INF_SEND(0);	// IE���ͣ�����������
		delay17_53us();	// ��ʱ17.53us
	}
	//------------------------------------------------------------
	
	
	//------------------------------------------------------------
	// ��16λ�û�����ֽ�	��8λ����
	NEC_IE_One_Data(T_user_code_low);
	
	// ��16λ�û�����ֽڣ�8λ����
	NEC_IE_One_Data(T_user_code_high);
	
	// ��8λ������ ��8λ����
	NEC_IE_One_Data(data_code_8bit);
	
	// ��8λ������ķ��룺8λ����
	NEC_IE_One_Data(~data_code_8bit);
	
	//------------------------------------------------------------
	
	
	// �޽����롮0��
	//--------------
	NEC_IE_Send_zero();
	//--------------
}
