#include "./infrared/infrared.h"

// 缓存红外接收数据的全局变量
//--------------------------------------------------------
uint32_t Each_bit_duration[64] = {0};	// 记录每一位数据的时长

uint8_t	Current_bit_CNT = 0;			// 表示当前是第几位

uint16_t Receive_user_code_16bit = 0 ;	// 记录16位用户码

uint8_t  Receive_data_code_8bit = 0 ;	// 记录8位数据码

uint8_t  Receive_data_code_opposite = 0;	// 记录8位数据码的反码
//--------------------------------------------------------



// 以下为红外发射的相关函数
//########################################################################################################
//内核精确延时

#define		delay8_77us()	delay_us(8)
#define		delay17_53us()	delay_us(17)
//---------------------------------------

 /**
  * @brief  配置 PA0 为线中断口，并设置中断优先级
  * @param  无
  * @retval 无
  */
void EXTI_Key_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; 


    SEND_GPIO_CLK_ENABLE();
    KEY2_INT_GPIO_CLK_ENABLE();

	/*发送*/
    GPIO_InitStructure.Pin = SEND_GPIO_PIN;

    GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;	    		

    GPIO_InitStructure.Pull = GPIO_NOPULL;

    HAL_GPIO_Init(SEND_GPIO_PORT, &GPIO_InitStructure); 

}
void SEND_IRQHandler(void)
{
  //确保是否产生了EXTI Line中断
	if(__HAL_GPIO_EXTI_GET_IT(SEND_GPIO_PIN) != RESET) 
	{

    //清除中断标志位
		__HAL_GPIO_EXTI_CLEAR_IT(SEND_GPIO_PIN);     
	}  
}

void KEY2_IRQHandler(void)
{
  //确保是否产生了EXTI Line中断
	if(__HAL_GPIO_EXTI_GET_IT(KEY2_INT_GPIO_PIN) != RESET) 
	{

    //清除中断标志位
		__HAL_GPIO_EXTI_CLEAR_IT(KEY2_INT_GPIO_PIN);     
	}  
}




// 初始化红外发射管脚：Infrared_IE = PC1
//----------------------------------------------------------------------------------------------
void Infrared_IE_Init_JX(void)
{
//	GPIO_InitTypeDef GPIO_InitStruct;                       // 定义一个GPIO_InitTypeDef类型的变量
//	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);   // 允许GPIOC时钟
//	
//	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;                  // GPIO_Pin_1
//	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;           // 通用推挽输出
//	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;          // 50MHz速度
//	GPIO_Init(GPIOC, &GPIO_InitStruct);
//	
//	GPIO_ResetBits(GPIOC, GPIO_Pin_1);                      //PC1拉低
	
	GPIO_InitTypeDef GPIO_InitStructure; 

	SEND_GPIO_CLK_ENABLE();

	/*发送*/
	GPIO_InitStructure.Pin = SEND_GPIO_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;	    		
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;

	HAL_GPIO_Init(SEND_GPIO_PORT, &GPIO_InitStructure); 
	
	INF_SEND(0);
	
}
//----------------------------------------------------------------------------------------------


//*********************************** 载波调制 *********************************************
//
//	26.3us（一个周期）= 8.77us（IE为高，发射红外光）+ 17.53us（IE为低，不发射红外光）
//	注：一个载波周期中，发射红外光8.77us  + 不发射红外光17.53us，这个周期就是载波发射周期。
//	注：载波发射周期中，发射红外光的占空比一般为1/3。

//	26.3us（一个周期）= 26.3us（IE为低，不发射红外光）
//	注：整个周期内，都不发射红外光，这个周期为载波不发射周期。

//******************************************************************************************


// NEC协议数据"0"= 载波发射0.56ms + 载波不发射0.56ms
//-------------------------------------------------
void NEC_IE_Send_zero(void)     
{
	uint8_t i;
	
	// 载波发射0.56ms ≈ 26.3us * 21
	//-------------------------------
	for(i=0;i<22;i++)
	{
        //26.3us（载波发射周期）
		//------------------------------------
		INF_SEND(1);	// IE抬高，发射红外光
		delay8_77us();  // 延时8.77us                        
											
		INF_SEND(0);	// IE拉低，不发射红外光
		delay17_53us(); // 延时17.53us
		//------------------------------------
	}

	
	// 载波不发射0.56ms ≈ 26.3us * 21
	//-------------------------------
	for(i=0;i<21;i++)
	{
        //26.3us（载波不发射周期）
		//------------------------------------
		INF_SEND(0);	// IE拉低，不发射红外光
		delay8_77us();  // 延时8.77us
                                            
		INF_SEND(0);	// IE拉低，不发射红外光
		delay17_53us(); // 延时17.53us
		//------------------------------------
	}
}
//-------------------------------------------------


// NEC协议数据"1" = 载波发射0.56ms + 载波不发射1.68ms
//-------------------------------------------------
void NEC_IE_Send_one(void)
{
	uint8_t i;
	
	// 载波发射0.56ms ≈ 26.3us * 21
	//-------------------------------
	for(i=0;i<22;i++)
	{
		//26.3us（载波发射周期）
		//------------------------------------
		INF_SEND(1);    // IE抬高，发射红外光
		delay8_77us();	// 延时8.77us
		
		INF_SEND(0);  // IE拉低，不发射红外光
		delay17_53us();	// 延时17.53us
		//------------------------------------
	}
	
	// 载波不发射1.68ms ≈ 26.3us * 64
	//--------------------------------
	for(i=0;i<64;i++)
	{
		//26.3us（载波不发射周期）
		//------------------------------------
		INF_SEND(0);	// IE拉低，不发射红外光
		delay8_77us();	// 延时8.77us
		
		INF_SEND(0);	// IE拉低，不发射红外光
		delay17_53us();	// 延时17.53us
		//------------------------------------
	}
}
//-------------------------------------------------


// 将一帧数据调制为NEC协议规定的红外载波发射出去
// 一帧数据格式：低位在前，由低到高发送8位数据
// 这帧数据可以是：②16位用户码低字节(8位) / ③16位用户码高字节(8位) / ④8位数据码 / ⑤8位数据码的反码
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



// 发送NEC编码格式的信息
//----------------------------------------------------------------------------------------------------------------------
// NEC 编码格式： ①引导码 + ②16位用户码低字节(8位) + ③16位用户码高字节(8位) + ④8位数据码 + ⑤8位数据码的反码 + ⑥结束码‘0’ 
//----------------------------------------------------------------------------------------------------------------------
void NEC_IE_code_message(uint16_t user_code_16bit, uint8_t data_code_8bit)
{
	uint16_t i;
	
	uint8_t T_user_code_high = user_code_16bit>>8;
	
	uint8_t T_user_code_low = user_code_16bit;

	// ①引导码：载波发射9ms，不发射4.5ms
	//------------------------------------------------------------
	// 载波发射 9ms ≈ 26.3us * 342
	for(i=0;i<342;i++) 
	{
		INF_SEND(1);	// IE抬高，发射红外光
		delay8_77us();	// 延时8.77us
		
		INF_SEND(0);	// IE拉低，不发射红外光
		delay17_53us();	// 延时17.53us
	}
	
	// 载波不发射 4.5ms ≈ 26.3us * 171
	for(i=0;i<171;i++)
	{
		INF_SEND(0);	// IE拉低，不发射红外光
		delay8_77us();	// 延时8.77us
		
		INF_SEND(0);	// IE拉低，不发射红外光
		delay17_53us();	// 延时17.53us
	}
	//------------------------------------------------------------
	
	
	//------------------------------------------------------------
	// ②16位用户码低字节	：8位数据
	NEC_IE_One_Data(T_user_code_low);
	
	// ③16位用户码高字节：8位数据
	NEC_IE_One_Data(T_user_code_high);
	
	// ④8位数据码 ：8位数据
	NEC_IE_One_Data(data_code_8bit);
	
	// ⑤8位数据码的反码：8位数据
	NEC_IE_One_Data(~data_code_8bit);
	
	//------------------------------------------------------------
	
	
	// ⑥结束码‘0’
	//--------------
	NEC_IE_Send_zero();
	//--------------
}
