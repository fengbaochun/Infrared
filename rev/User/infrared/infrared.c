#include "./infrared/infrared.h"

// 缓存红外接收数据的全局变量
//--------------------------------------------------------
uint32_t Each_bit_duration[64] = {0};	// 记录每一位数据的时长

uint8_t	Current_bit_CNT = 0;			// 表示当前是第几位

uint16_t Receive_user_code_16bit = 0 ;	// 记录16位用户码

uint8_t  Receive_data_code_8bit = 0 ;	// 记录8位数据码

uint8_t  Receive_data_code_opposite = 0;	// 记录8位数据码的反码
//--------------------------------------------------------

TIM_HandleTypeDef TIM_TimeBaseStructure;

// 以下为红外发射的相关函数
//########################################################################################################
//内核精确延时

#define		delay8_77us()	delay_us(8)
#define		delay17_53us()	delay_us(17)

// 以下为红外发射的相关函数
//########################################################################################################

#define		delay8_77us()	delay_us(8)
#define		delay17_53us()	delay_us(17)


void Infrared_IR_Init_JX(void) 
{
//	GPIO_InitTypeDef GPIO_InitStruct; 
//	EXTI_InitTypeDef EXTI_InitStruct;
//	NVIC_InitTypeDef NVIC_InitStruct;
//	
//	/* 配置PC0引脚 */
//	//---------------------------------------------------------------------------
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);		// 使能PC端口时钟											
//	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;	                // 上拉输入模式
//	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;						// PC0
//	GPIO_Init(GPIOC, &GPIO_InitStruct);	
//	//---------------------------------------------------------------------------
//	
//	/* 配置PC0引脚中断模式 */
//	//----------------------------------------------------------------------------------
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);			// 使能映射时钟
//    
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource0); // 将中断线0映射到PC0线上
//    
//	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;			// 选择为中断
//	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling; 		// 下降沿中断
//	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
//	EXTI_InitStruct.EXTI_Line = EXTI_Line0;						// 选择外部中断线0
//	EXTI_Init(&EXTI_InitStruct);								
//	
//	 /* 配置NVIC */
//	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;		// 抢占优先级
//	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;				// 响应优先级
//	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;	
//	NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn; 				// 中断通道：外部中断线0
//	NVIC_Init(&NVIC_InitStruct);


	GPIO_InitTypeDef GPIO_InitStructure; 

    /*开启按键GPIO口的时钟*/
    REV_GPIO_CLK_ENABLE();

    /* 选择按键1的引脚 */ 
    GPIO_InitStructure.Pin = REV_GPIO_PIN;
    /* 设置引脚为输入模式 */ 
    GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;//GPIO_MODE_IT_RISING_FALLING;//GPIO_MODE_IT_RISING;	    		  GPIO_MODE_IT_RISING_FALLING
    /* 设置引脚不上拉也不下拉 */
    GPIO_InitStructure.Pull = GPIO_PULLDOWN;//GPIO_NOPULL;
    /* 使用上面的结构体初始化按键 */
    HAL_GPIO_Init(REV_GPIO_PORT, &GPIO_InitStructure); 
    /* 配置 EXTI 中断源 到key1 引脚、配置中断优先级*/
    HAL_NVIC_SetPriority(REV_EXTI_IRQ, 0, 0);
    /* 使能中断 */
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
  * @brief  定时器中断函数
	*	@note 		无
  * @retval 无
  */
void  GENERAL_TIM_INT_IRQHandler (void)
{
	HAL_TIM_IRQHandler(&TIM_TimeBaseStructure);	 	
}
/**
  * @brief  回调函数
	*	@note 		无
  * @retval 无
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	uint16_t TIM2_IT_Update_Cnt = 0 ;		// TIM2的溢出次数，用于LED1的闪烁计时
    if(htim==(&TIM_TimeBaseStructure))
    {
		// 当接收到一个下降沿后，下一个下降沿须在20ms内被接收到，否则此次红外接收认为是出错的
		//-----------------------------------------------------------------------------
		Current_bit_CNT = 0;	// 将当前红外接收的位数清0
		
		TIM2_IT_Update_Cnt ++ ;
		
		if( TIM2_IT_Update_Cnt >= 25 )		// 蓝灯闪烁速率：1s
		{
			TIM2_IT_Update_Cnt = 0 ;
			
		}
		
		
    }
}





// 解码NEC编码格式的信息
// 注：协议码的时长根据需要，可自行限定判断区间
// 注：TIM2的计数周期为1us，溢出值为20000
//------------------------------------------------------------------------------------------
uint8_t NEC_IR_decode_message(void)
{
	uint8_t NEC_decode_cnt;
	
	// 协议引导码的标准长度为13.5ms
	// 判断引导码的长度是否在(12ms,15ms)区间内
	//-----------------------------------------------------------
	if( Each_bit_duration[1]<12000 || Each_bit_duration[1]>15000 )
	{ Current_bit_CNT=0;return F_decode_error; }	// 不在规定的区间内，返回解码出错
	
	
	// 解码16位用户码
	//---------------------------------------------------------
	for(NEC_decode_cnt=2; NEC_decode_cnt<18; NEC_decode_cnt++)
	{
		// 协议数据"1"的标准长度为2.24ms
		// 协议数据位的长度在(2ms,2.5ms)区间内 => 协议数据"1"
		//-------------------------------------------------------------------------------------
		if( Each_bit_duration[NEC_decode_cnt]>2000 && Each_bit_duration[NEC_decode_cnt]<2500 )
		{ 
			Receive_user_code_16bit >>= 1;
			
			Receive_user_code_16bit |= 0x8000;
		}
		
		// 协议数据"0"的标准长度为1.12ms
		// 协议数据位的长度在(1ms,1.25ms)区间内 => 协议数据"0"
		//------------------------------------------------------------------------------------------
		else if( Each_bit_duration[NEC_decode_cnt]>1000 && Each_bit_duration[NEC_decode_cnt]<1250 )
		{
			Receive_user_code_16bit >>= 1;
			
			Receive_user_code_16bit &= ~0x8000;
		}
		
		// 协议数据位不在限定范围内，出错
		//-----------------------------
		else { return F_decode_error; }
	}
	
	// 解码8位数据码
	//---------------------------------------------------------
	for(NEC_decode_cnt=18; NEC_decode_cnt<26; NEC_decode_cnt++)
	{
		// 协议数据"1"的标准长度为2.24ms
		// 协议数据位的长度在(2ms,2.5ms)区间内 => 协议数据"1"
		//-------------------------------------------------------------------------------------
		if( Each_bit_duration[NEC_decode_cnt]>2000 && Each_bit_duration[NEC_decode_cnt]<2500 )
		{ 
			Receive_data_code_8bit >>= 1;
			
			Receive_data_code_8bit |= 0x80;
		}
		
		// 协议数据"0"的标准长度为1.12ms
		// 协议数据位的长度在(1ms,1.25ms)区间内 => 协议数据"0"
		//------------------------------------------------------------------------------------------
		else if( Each_bit_duration[NEC_decode_cnt]>1000 && Each_bit_duration[NEC_decode_cnt]<1250 )
		{
			Receive_data_code_8bit >>= 1;
			
			Receive_data_code_8bit &= ~0x80;
		}
		
		// 协议数据位不在限定范围内，出错
		//-----------------------------
		else { return F_decode_error; }
	}
	
	// 解码8位数据码的反码
	//---------------------------------------------------------
	for(NEC_decode_cnt=26; NEC_decode_cnt<34; NEC_decode_cnt++)
	{
		// 协议数据"1"的标准长度为2.24ms
		// 协议数据位的长度在(2ms,2.5ms)区间内 => 协议数据"1"
		//-------------------------------------------------------------------------------------
		if( Each_bit_duration[NEC_decode_cnt]>2000 && Each_bit_duration[NEC_decode_cnt]<2500 )
		{ 
			Receive_data_code_opposite >>= 1;
			
			Receive_data_code_opposite |= 0x80;
		}
		
		// 协议数据"0"的标准长度为1.12ms
		// 协议数据位的长度在(1ms,1.25ms)区间内 => 协议数据"0"
		//------------------------------------------------------------------------------------------
		else if( Each_bit_duration[NEC_decode_cnt]>1000 && Each_bit_duration[NEC_decode_cnt]<1250 )
		{
			Receive_data_code_opposite >>= 1;
			
			Receive_data_code_opposite &= ~0x80;
		}
		
		// 协议数据位不在限定范围内，出错
		//-----------------------------
		else { return F_decode_error; }
	}
	
	return F_decode_success;	// 解码成功	
}

void REV_IRQHandler(void)
{
  //确保是否产生了EXTI Line中断
	if(__HAL_GPIO_EXTI_GET_IT(REV_GPIO_PIN) != RESET) 
	{
		//清除中断标志位
		__HAL_GPIO_EXTI_CLEAR_IT(REV_GPIO_PIN);     

		Each_bit_duration[Current_bit_CNT] = TIM2->CNT;	// 将此下降沿的TIM2计数存入Each_bit_duration[x]中

		Current_bit_CNT ++ ;	// 将当前红外接收的位数+1

		// 1、方便下一个下降沿的计时
		// 2、等待下一个下降沿20ms
		//---------------------------------------------------------------------
		TIM2->CNT=0;			// 红外接收管脚接收到一个下降沿后，将TIM2计数器清0
	}  
}















