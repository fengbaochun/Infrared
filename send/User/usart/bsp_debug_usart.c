#include "./usart/bsp_debug_usart.h"
#include "string.h"
#include "stdlib.h"

UART_HandleTypeDef UartHandle;
//extern uint8_t ucTemp;  

 /**
  * @brief  DEBUG_USART GPIO 配置,工作模式配置。115200 8-N-1
  * @param  无
  * @retval 无
  */  
void DEBUG_USART_Config(void)
{ 
  
  UartHandle.Instance          = DEBUG_USART;
  
  UartHandle.Init.BaudRate     = DEBUG_USART_BAUDRATE;
  UartHandle.Init.WordLength   = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits     = UART_STOPBITS_1;
  UartHandle.Init.Parity       = UART_PARITY_NONE;
  UartHandle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode         = UART_MODE_TX_RX;
  
  HAL_UART_Init(&UartHandle);
    
 /*使能串口接收断 */
  __HAL_UART_ENABLE_IT(&UartHandle,UART_IT_RXNE);  
}


/**
  * @brief UART MSP 初始化 
  * @param huart: UART handle
  * @retval 无
  */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{  
  GPIO_InitTypeDef  GPIO_InitStruct;
  
  DEBUG_USART_CLK_ENABLE();
	
	DEBUG_USART_RX_GPIO_CLK_ENABLE();
  DEBUG_USART_TX_GPIO_CLK_ENABLE();
  
/**USART1 GPIO Configuration    
  PA9     ------> USART1_TX
  PA10    ------> USART1_RX 
  */
  /* 配置Tx引脚为复用功能  */
  GPIO_InitStruct.Pin = DEBUG_USART_TX_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = DEBUG_USART_TX_AF;
  HAL_GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStruct);
  
  /* 配置Rx引脚为复用功能 */
  GPIO_InitStruct.Pin = DEBUG_USART_RX_PIN;
  GPIO_InitStruct.Alternate = DEBUG_USART_RX_AF;
  HAL_GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStruct); 
 
  HAL_NVIC_SetPriority(DEBUG_USART_IRQ ,0,1);	//抢占优先级0，子优先级1
  HAL_NVIC_EnableIRQ(DEBUG_USART_IRQ );		    //使能USART1中断通道  
}

/*

//例子　

    #include <stdio.h>
　　#include <stdlib.h>
　　void main()
　　{
　　    char* p = "0x1b";
　　    char* str;
　　    long i = strtol(p, &str, 16);
　　    printf("%d\r\n", i);
　　}


//输出值为 27

*/

long str_to_int(char* p)
{
	char* str;
	long ret = strtol(p, &str, 16);
	return ret;
}


char usart_cmd[CMD_MAX_LEN];
char user_code[USER_CODE_LEN];
char data_code[DATA_CODE_LEN];
__IO int u_code,d_code;
/*
<00,11>
*/

int send_status=0;

/*
解析命令
*/
void deal_cmd()
{
	
	char user_code[USER_CODE_LEN];
	char data_code[DATA_CODE_LEN];
		
	memset(user_code,0,sizeof(char)*USER_CODE_LEN);
	memset(data_code,0,sizeof(char)*DATA_CODE_LEN);
	
	memcpy(user_code,usart_cmd,2*sizeof(char));
	memcpy(data_code,usart_cmd+3,2*sizeof(char));
		
	u_code=str_to_int(user_code);
	d_code=str_to_int(data_code);
	
	printf("u_code->%d\r\n",u_code);
	printf("d_code->%d\r\n",d_code);
	
	
//	printf("user_code->%s\r\n",user_code);
//	printf("data_code->%s\r\n",data_code);
		
	memset(usart_cmd,0,CMD_MAX_LEN*sizeof(char));
	send_status=1;

}

void data_deal(char dat)
{
    static uint8_t data_flag,dat_index=0;
	/*协议头尾*/
	uint8_t head_ch='<';
	uint8_t tail_ch='>';
    if(data_flag==0&&dat==head_ch)
    {
        data_flag=1;
    }
    else if(data_flag==1)
    {
        usart_cmd[dat_index++]=dat;
        if(dat==tail_ch)
        {
            deal_cmd();
            data_flag=0;
            dat_index=0;
        }
    }
}



void  DEBUG_USART_IRQHandler(void)
{
  uint8_t ch=0; 
  
	if(__HAL_UART_GET_FLAG( &UartHandle, UART_FLAG_RXNE ) != RESET)
	{		
		ch=( uint16_t)READ_REG(UartHandle.Instance->DR);
		data_deal(ch);
		
		//WRITE_REG(UartHandle.Instance->DR,ch); 
	}
}


/*****************  发送字符串 **********************/
void Usart_SendString(uint8_t *str)
{
	unsigned int k=0;
  do 
  {
      HAL_UART_Transmit(&UartHandle,(uint8_t *)(str + k) ,1,1000);
      k++;
  } while(*(str + k)!='\0');
  
}
///重定向c库函数printf到串口DEBUG_USART，重定向后可使用printf函数
int fputc(int ch, FILE *f)
{
	/* 发送一个字节数据到串口DEBUG_USART */
	HAL_UART_Transmit(&UartHandle, (uint8_t *)&ch, 1, 1000);	
	
	return (ch);
}

///重定向c库函数scanf到串口DEBUG_USART，重写向后可使用scanf、getchar等函数
int fgetc(FILE *f)
{
		
	int ch;
	HAL_UART_Receive(&UartHandle, (uint8_t *)&ch, 1, 1000);	
	return (ch);
}
/*********************************************END OF FILE**********************/
