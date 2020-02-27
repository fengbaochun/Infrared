#ifndef __CORE_DELAY_H
#define __CORE_DELAY_H

#include "main.h"
#include "stm32f4xx.h"
/* ��ȡ�ں�ʱ��Ƶ�� */
#define CPU_HZ 					168
#define GET_CPU_ClkFreq()       168000000


/* Ϊ����ʹ�ã�����ʱ�����ڲ�����CPU_TS_TmrInit������ʼ��ʱ����Ĵ�����
   ����ÿ�ε��ú��������ʼ��һ�顣
   �ѱ���ֵ����Ϊ0��Ȼ����main����������ʱ����CPU_TS_TmrInit�ɱ���ÿ�ζ���ʼ�� */  

#define CPU_TS_INIT_IN_DELAY_FUNCTION   1  

/*******************************************************************************
 * ��������
 ******************************************************************************/
//uint32_t CPU_TS_TmrRd(void);
//void CPU_TS_TmrInit(void);

////ʹ�����º���ǰ�����ȵ���CPU_TS_TmrInit����ʹ�ܼ���������ʹ�ܺ�CPU_TS_INIT_IN_DELAY_FUNCTION
////�����ʱֵΪ8��
//void CPU_TS_Tmr_Delay_US(uint32_t us);
//#define CPU_TS_Tmr_Delay_MS(ms)     CPU_TS_Tmr_Delay_US(ms*1000)
//#define CPU_TS_Tmr_Delay_S(s)       CPU_TS_Tmr_Delay_MS(s*1000)

//#define delay_us(x)		CPU_TS_Tmr_Delay_US(x)
//#define delay_ms(x)  	CPU_TS_Tmr_Delay_MS(x)
//#define delay_s(x)   	CPU_TS_Tmr_Delay_S(x)  

void delay_ms_(int nTimer)  ;
void delay_us_(int nTimer) ;


#endif /* __CORE_DELAY_H */
