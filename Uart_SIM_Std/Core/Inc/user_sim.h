#ifndef __USER_SIM_
#define __USER_SIM_

#include "uart_sim.h"
#include "math.h"
#include "Flash_Memory.h"
#include "stdio.h"

#define TIME_ERROR_CIPSEND 1000
#define ASCII_VALUE_NUMBER 48

int8_t Receive_Control_Setup(UART_BUFFER *sUart1, UART_BUFFER *sUart3, uint8_t *check_connect, uint32_t *time1, uint32_t *time2,uint32_t *time3);
int8_t Check_Receive_sendData_Control(UART_BUFFER *sUart1,UART_BUFFER *sUart3);
int8_t SendData_Server(UART_BUFFER *sUart1, UART_BUFFER *sUart3, REAL_TIME *RTC_Current);
void Packing_News(REAL_TIME *RTC_Current);

void Get_Addr_Read_Write(void);
void Reset_SendData(void);
#endif

