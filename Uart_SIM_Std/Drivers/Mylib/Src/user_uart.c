#include "user_uart.h"

uint16_t check_countBuffer_uart1=0;
uint32_t Get_systick_countBuffer_uart1=0;
uint16_t State_systick_countBuffer_uart1=0;

uint16_t check_countBuffer_uart3=0;
uint32_t Get_systick_countBuffer_uart3=0;
uint16_t State_systick_countBuffer_uart3=0;

int8_t Check_CountBuffer_Complete_Uart1(UART_BUFFER *sUart)
{
	uint16_t answer=0;
	if(sUart->countBuffer > 0)
	{
		if(State_systick_countBuffer_uart1 == 0 )
		{
			Get_systick_countBuffer_uart1 = HAL_GetTick();
			State_systick_countBuffer_uart1 = 1;
			check_countBuffer_uart1 = sUart->countBuffer;
		}
		
		if(HAL_GetTick() - Get_systick_countBuffer_uart1>1 && State_systick_countBuffer_uart1 == 1)	
		{
			if(check_countBuffer_uart1 == sUart->countBuffer)
			{
				answer = 1;
			}
			else
			{
				Get_systick_countBuffer_uart1 = HAL_GetTick();
				check_countBuffer_uart1 = sUart->countBuffer;
			}
		}
	}
	else
	{
		State_systick_countBuffer_uart1 = 0;
	}
	return answer;
}

int8_t Check_CountBuffer_Complete_Uart3(UART_BUFFER *sUart)
{
	uint16_t answer=0;
	if(sUart->countBuffer > 0)
	{
		if(State_systick_countBuffer_uart3 == 0 )
		{
			Get_systick_countBuffer_uart3 = HAL_GetTick();
			State_systick_countBuffer_uart3 = 1;
			check_countBuffer_uart3 = sUart->countBuffer;
		}
		
		if(HAL_GetTick() - Get_systick_countBuffer_uart3 > 1 && State_systick_countBuffer_uart3 == 1)	
		{
			if(check_countBuffer_uart3 == sUart->countBuffer)
			{
				answer = 1;
			}
			else
			{
				Get_systick_countBuffer_uart3 = HAL_GetTick();
				check_countBuffer_uart3 = sUart->countBuffer;
			}
		}
	}
	else
	{
		State_systick_countBuffer_uart3 = 0;
	}
	return answer;
}

void Transmit_Data_Uart(UART_HandleTypeDef huart,void* data)																								
{
	HAL_UART_Transmit(&huart,(uint8_t *)data,(uint16_t)strlen(data),1000);
	HAL_UART_Transmit(&huart,(uint8_t *)"\r\n",(uint16_t)strlen("\r\n"),1000);
}

void Delete_Buffer(UART_BUFFER *sUart)
{
	sUart->countBuffer=0;
	sUart->buffer=0x00;
	for(int i = 0; i < LENGTH_BUFFER_UART; i++)
	{
		sUart->sim_rx[i] = 0;
	}
}


