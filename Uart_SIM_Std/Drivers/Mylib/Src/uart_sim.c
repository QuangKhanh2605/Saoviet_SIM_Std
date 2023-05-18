#include "uart_sim.h"

uint8_t check10Times=0;

uint8_t check_Transmit=0;
uint8_t count_Transmit_TCP=0;
uint8_t count_Transmit_SMS=0;
uint8_t count_Transmit_Uart_SIM=0;
uint8_t count_Transmit_CFUN=0;
uint8_t count_Connect_Server=0;

uint8_t time_10_TCP=0;
uint8_t time_10_SMS=0;
uint8_t time_10_Uart_SIM=0;
uint8_t time_3_Uart_SIM=0;
uint8_t time_10_CFUN=0;

uint32_t get_systick_command=0;
uint8_t check_timeOut=0;

uint32_t check_systick_Sim = 0;
uint8_t check_config_Sim = 0;


uint8_t check_systick_CCLK=0;
uint32_t get_systick_CCLK=0;

void Setup_On_Off_Sim(GPIO_TypeDef* GPIO1, uint16_t GPIO_Pin_On_Off_Sim, 
                      GPIO_TypeDef* GPIO2, uint16_t GPIO_Pin_PWKEY,  
                      GPIO_TypeDef* GPIO3, uint16_t GPIO_Pin_RESET, uint32_t timeOut);

int8_t Setup_SIM(UART_BUFFER *sUart1, UART_BUFFER *sUart3);
int8_t Transmit_CFUN(UART_BUFFER *sUart1, UART_BUFFER *sUart3);

int8_t Config_TCP(UART_BUFFER *sUart1,UART_BUFFER *sUart3);
int8_t Config_SMS(UART_BUFFER *sUart1, UART_BUFFER *sUart3);
int8_t Config_Uart_Sim(UART_BUFFER *sUart1, UART_BUFFER *sUart3);
int8_t Check_Command_Config(UART_BUFFER *sUart1,UART_BUFFER *sUart3, char* command,char* response, uint8_t *time_10, uint32_t Time_Resend_Command);

int8_t Compare_Uart1_RX_Uart3_TX_True_Display(UART_BUFFER *sUart1, UART_BUFFER *sUart3,char* response)
{
	uint8_t answer = 0;
	if(Check_CountBuffer_Complete_Uart1(sUart1)==1)
	{
		Transmit_Data_Uart(*sUart1->huart,sUart1->sim_rx);	
		HAL_UART_Transmit(sUart3->huart, (uint8_t*)sUart1->sim_rx, (uint8_t)strlen(sUart1->sim_rx), 1000);
		Delete_Buffer(sUart1);
	}
	
	if(Check_CountBuffer_Complete_Uart3(sUart3)==1)
	{	
		if(strstr(sUart3->sim_rx,response) != NULL) 
		{
			Transmit_Data_Uart(*sUart1->huart,sUart3->sim_rx);
			Delete_Buffer(sUart3);
			answer = 1;
		}
		else 
		{
			answer = 0;
		}	
	}
	return answer;
}

int8_t Compare_Uart1_RX_Uart3_TX(UART_BUFFER *sUart1, UART_BUFFER *sUart3,char* response)
{
	uint8_t answer = 0;
	if(Check_CountBuffer_Complete_Uart1(sUart1)==1)
	{
		Transmit_Data_Uart(*sUart1->huart,sUart1->sim_rx);	
		HAL_UART_Transmit(sUart3->huart, (uint8_t*)sUart1->sim_rx, (uint8_t)strlen(sUart1->sim_rx), 1000);
		Delete_Buffer(sUart1);
	}
	
	if(Check_CountBuffer_Complete_Uart3(sUart3)==1)
	{	
		Transmit_Data_Uart(*sUart1->huart,sUart3->sim_rx);
		if(strstr(sUart3->sim_rx,response) != NULL) 
		{
			answer = 1;
		}
		else 
		{
			answer = 0;
		}
		Delete_Buffer(sUart3);
	}
	return answer;
}

int8_t Uart1_To_Uart3(UART_BUFFER *sUart1, UART_BUFFER *sUart3)
{
	uint8_t answer = 0;
	if(Check_CountBuffer_Complete_Uart1(sUart1)==1)
	{
		Transmit_Data_Uart(*sUart1->huart,sUart1->sim_rx);	
		HAL_UART_Transmit(sUart3->huart, (uint8_t*)sUart1->sim_rx, (uint8_t)strlen(sUart1->sim_rx), 1000);
		Delete_Buffer(sUart1);
	}
  if(Check_CountBuffer_Complete_Uart3(sUart3)==1)
	{
		Transmit_Data_Uart(*sUart1->huart,sUart3->sim_rx);
		Delete_Buffer(sUart3);
		answer=1;
	}
	return answer;
}

int8_t Config_SMS(UART_BUFFER *sUart1, UART_BUFFER *sUart3)
{
	switch (count_Transmit_SMS)
	{
		case 0:
			if(Check_Command_Config(sUart1, sUart3, "AT+CMGF=1", "OK", &time_10_SMS, TIME_RESEND_COMMAND_MODULE_SIM) ==1)
			{
				time_10_SMS=0;
				count_Transmit_SMS++;
			}
			if(time_10_SMS>10) 
			{
				time_10_SMS=0;
				count_Transmit_SMS=0;
				return -1;
			}
			break;
	
		default:
			if(Check_Command_Config(sUart1, sUart3, "AT+CNMI=1,2", "OK", &time_10_SMS, TIME_RESEND_COMMAND_MODULE_SIM) ==1)
			{
				time_10_SMS=0;
				count_Transmit_SMS=0;
				return 1;
			}
			if(time_10_SMS>10) 
			{
				time_10_SMS=0;
				count_Transmit_SMS=0;
				return -1;
			}
			break;
	}
	return 0;
}

int8_t Config_TCP(UART_BUFFER *sUart1, UART_BUFFER *sUart3)
{
	switch (count_Transmit_TCP)
	{
		case 0:
			if(Check_Command_Config(sUart1, sUart3, "AT+CIPTIMEOUT=30000,20000,40000,50000", "OK", &time_10_TCP, TIME_RESEND_COMMAND_MODULE_SIM) ==1)
			{
				time_10_TCP=0;
				count_Transmit_TCP++;
			}
			if(time_10_TCP>10) 
			{
				time_10_TCP=0;
				count_Transmit_TCP=0;
				return -1;
			}
			break;
	
		case 1:
			if(Check_Command_Config(sUart1, sUart3, "AT+CIPMODE=0", "OK", &time_10_TCP, TIME_RESEND_COMMAND_MODULE_SIM) ==1)
			{
				time_10_TCP=0;
				count_Transmit_TCP++;
			}
			if(time_10_TCP>10) 
			{
				time_10_TCP=0;
				count_Transmit_TCP=0;
				return -1;
			}
			break;
	
	
		case 2:
			if(Check_Command_Config(sUart1, sUart3, "AT+NETOPEN", "SUCCESS", &time_10_TCP, TIME_RESEND_COMMAND_CONNECT_SIM) ==1)
			{
				time_10_TCP=0;
				count_Transmit_TCP++;
			}
			if(time_10_TCP>10) 
			{
				time_10_TCP=0;
				count_Transmit_TCP=0;
				return -1;
			}
			break;
	
		case 3:
			if(Check_Command_Config(sUart1, sUart3, "AT+IPADDR", "SUCCESS", &time_10_TCP, TIME_RESEND_COMMAND_CONNECT_SIM) ==1)
			{
				time_10_TCP=0;
				count_Transmit_TCP++;
			}
			if(time_10_TCP>10) 
			{
				time_10_TCP=0;
				count_Transmit_TCP=0;
				return -1;
			}
			break;
	
		case 4:
			if(Check_Command_Config(sUart1, sUart3 , "AT+CIPRXGET=0,1", "OK", &time_10_TCP, TIME_RESEND_COMMAND_MODULE_SIM) ==1)
			{
				time_10_TCP=0;
				count_Transmit_TCP++;
			}
			if(time_10_TCP>10) 
			{
				time_10_TCP=0;
				count_Transmit_TCP=0;
				return -1;
			}
			break;

		default:
			if(Check_Command_Config(sUart1, sUart3, "AT+CIPOPEN=1,\"TCP\",\"113.190.240.47\",7577,0", "SUCCESS", &time_10_TCP, TIME_RESEND_COMMAND_CONNECT_SIM) ==1)
			{
				time_10_TCP=0;
				count_Transmit_TCP=0;
				return 1;
			}
			if(time_10_TCP>10) 
			{
				time_10_TCP=0;
				count_Transmit_TCP=0;
				return -1;
			}
			break;
	}
	return 0;
}

int8_t Transmit_CFUN(UART_BUFFER *sUart1, UART_BUFFER *sUart3)
{
	switch (count_Transmit_CFUN)
	{
		case 0:
			if(Check_Command_Config(sUart1 ,sUart3, "AT+CFUN=4", "OK", &time_10_CFUN, TIME_RESEND_COMMAND_MODULE_SIM) ==1)
			{
				time_10_CFUN=0;
				count_Transmit_CFUN++;
			}
			if(time_10_CFUN>10) 
			{
				time_10_CFUN=0;
				count_Transmit_CFUN=0;
				return -1;
			}
			break;
	
		default:
			if(Check_Command_Config(sUart1, sUart3, "AT+CFUN=1", "OK", &time_10_CFUN, TIME_RESEND_COMMAND_MODULE_SIM) ==1)
			{
				time_10_CFUN=0;
				count_Transmit_CFUN=0;
				return 1;
			}
			if(time_10_CFUN>10) 
			{
				time_10_CFUN=0;
				count_Transmit_CFUN=0;
				return -1;
			}
			break;
	}
	return 0;
}
int8_t Config_Uart_Sim(UART_BUFFER *sUart1, UART_BUFFER *sUart3)
{
	switch (count_Transmit_Uart_SIM)
	{
		case 0:
			if(Check_Command_Config(sUart1, sUart3, "AT", "OK", &time_10_Uart_SIM, TIME_RESEND_COMMAND_MODULE_SIM) ==1)
			{
				time_10_Uart_SIM=0;
				count_Transmit_Uart_SIM++;
			}
			if(time_10_Uart_SIM>10) 
			{
				time_10_Uart_SIM=0;
				return -1;
			}
			break;
	
		case 1:
			if(Check_Command_Config(sUart1, sUart3, "AT+CIPCLOSE=1", "OK", &time_10_Uart_SIM, TIME_RESEND_COMMAND_MODULE_SIM) ==1)
			{
				time_10_Uart_SIM=0;
				count_Transmit_Uart_SIM++;
			}
			if(time_10_Uart_SIM>10) 
			{
				count_Transmit_Uart_SIM=7;
				time_10_Uart_SIM=0;
			}
			break;
	
		case 2:
			if(Check_Command_Config(sUart1, sUart3, "AT+CPIN?", "OK", &time_10_Uart_SIM, TIME_RESEND_COMMAND_MODULE_SIM) ==1)
			{
				time_10_Uart_SIM=0;
				count_Transmit_Uart_SIM++;
			}
			if(time_10_Uart_SIM>10) 
			{
				count_Transmit_Uart_SIM=7;
				time_10_Uart_SIM=0;
			}
			break;
	
		case 3:
			if(Check_Command_Config(sUart1, sUart3, "AT+CICCID", "OK", &time_10_Uart_SIM, TIME_RESEND_COMMAND_MODULE_SIM) ==1)
			{
				time_10_Uart_SIM=0;
				count_Transmit_Uart_SIM++;
			}
			if(time_10_Uart_SIM>10) 
			{
				count_Transmit_Uart_SIM=7;
				time_10_Uart_SIM=0;
			}
			break;
	
		case 4:
			if(Check_Command_Config(sUart1, sUart3, "AT+CSQ", "OK", &time_10_Uart_SIM, TIME_RESEND_COMMAND_MODULE_SIM) ==1)
			{
				time_10_Uart_SIM=0;
				count_Transmit_Uart_SIM++;
			}
			if(time_10_Uart_SIM>10) 
			{
				count_Transmit_Uart_SIM=7;
				time_10_Uart_SIM=0;
			}
			break;
	
		case 5:
			if(Check_Command_Config(sUart1, sUart3, "AT+CGREG?", "OK", &time_10_Uart_SIM, TIME_RESEND_COMMAND_MODULE_SIM) ==1)
			{
				time_10_Uart_SIM=0;
				count_Transmit_Uart_SIM++;
			}
			if(time_10_Uart_SIM>10) 
			{
				count_Transmit_Uart_SIM=7;
				time_10_Uart_SIM=0;
			}
			break;
	
		case 6:
			if(Check_Command_Config(sUart1, sUart3, "AT+CGATT?", "CGATT: 1", &time_10_Uart_SIM, TIME_RESEND_COMMAND_CONNECT_SIM) ==1)
			{
				time_10_Uart_SIM=0;
				time_3_Uart_SIM=0;
				count_Transmit_Uart_SIM=0;
				return 1;
			}
			if(time_10_Uart_SIM>10) 
			{
				count_Transmit_Uart_SIM=7;
				time_10_Uart_SIM=0;
			}
			break;
	
		case 7:
			if(Check_Command_Config(sUart1, sUart3, "AT+CFUN=4", "OK", &time_10_Uart_SIM, TIME_RESEND_COMMAND_MODULE_SIM) ==1)
			{
				time_10_Uart_SIM=0;
				count_Transmit_Uart_SIM++;
			}
			if(time_10_Uart_SIM>10) 
			{
				time_3_Uart_SIM=0;
				time_10_Uart_SIM=0;
				return -1;
			}
			break;
	
		default:
			if(Check_Command_Config(sUart1, sUart3, "AT+CFUN=1", "OK", &time_10_Uart_SIM,TIME_RESEND_COMMAND_MODULE_SIM) ==1)
			{
				time_10_Uart_SIM=0;
				count_Transmit_Uart_SIM=1;
				time_3_Uart_SIM++;
				if(time_3_Uart_SIM >= 3) 
				{
					time_3_Uart_SIM=0;
					count_Transmit_Uart_SIM=0;
					return -1;
				}
			}
			if(time_10_Uart_SIM>10) 
			{
				time_3_Uart_SIM=0;
				time_10_Uart_SIM=0;
				return -1;
			}
			break;
	}
	
	if(time_3_Uart_SIM >= 3) 
	{
		time_3_Uart_SIM=0;
		count_Transmit_Uart_SIM=0;
		return -1;
	}
	return 0;
}


int8_t Check_Command_Config(UART_BUFFER *sUart1, UART_BUFFER *sUart3, char* command,char* response, uint8_t *time_10, uint32_t Time_Resend_Command)
{
	if(check_Transmit==0 )
	{
		Transmit_Data_Uart(*sUart3->huart, command);
		Transmit_Data_Uart(*sUart1->huart, command);
		check_Transmit=1;
		check_timeOut=1;
		get_systick_command = HAL_GetTick();
	}
	
	if(check_Transmit==1)
	{
		if(Check_CountBuffer_Complete_Uart3(sUart3)==1)
		{
			Transmit_Data_Uart(*sUart1->huart, sUart3->sim_rx );
			if(strstr(sUart3->sim_rx,response) != NULL) 
			{
				*time_10=0;
				Delete_Buffer(sUart3);
				check_timeOut=1;
				return 1;
			}
			else
			{
				Delete_Buffer(sUart3);
				check_timeOut=2;
			}
		}
	}
	
	if(check_timeOut==1)
	{
		if(HAL_GetTick() - get_systick_command > TIME_RESEND_COMMAND_IF_NOT_RECEIVE)
		{
			(*time_10)++;
			check_Transmit=0;
			check_timeOut=0;
		}
	}
	
	if(check_timeOut==2)
	{
		if(HAL_GetTick() - get_systick_command > Time_Resend_Command)
		{
			(*time_10)++;
			check_Transmit=0;
			check_timeOut=0;
		}
	}
	return 0;
}

int8_t Config_SIM(UART_BUFFER *sUart1, UART_BUFFER *sUart3)
{
	if(check_config_Sim==0) 
	{
		check_systick_Sim=HAL_GetTick();
		check_config_Sim++;
		Transmit_Data_Uart(*sUart1->huart, "Setup Module Sim");
	}
	if(check_config_Sim==1 ) 
	{
		if(Setup_SIM(sUart1,sUart3)==1) check_config_Sim++;
	}
	
	if(check_config_Sim==2)
	{
		int8_t config_uart_sim = Config_Uart_Sim(sUart1,sUart3);
		if(config_uart_sim ==1) check_config_Sim++;
		if(config_uart_sim ==-1) 
		{
			check_config_Sim=0;
		}
	}

	if(check_config_Sim==3) 
	{
		int8_t config_sms = Config_SMS(sUart1, sUart3);
		if(config_sms==1) check_config_Sim++;
		if(config_sms==-1) 
		{
			check_config_Sim=0;
		}
	}
	
	if(check_config_Sim==4) 
	{
		int8_t config_tcp=Config_TCP(sUart1, sUart3);
		if(config_tcp==1) 
		{
			check_config_Sim=0;
			return 1;
		}
		if(config_tcp==-1) 
		{
			check_config_Sim=0;
		}
	}
	return 0;
}

int8_t Setup_SIM(UART_BUFFER *sUart1, UART_BUFFER *sUart3)
{
	uint32_t run_Time=0;
	if(HAL_GetTick() - check_systick_Sim<=TIME_ON_SIM)
	{
		run_Time=(HAL_GetTick() - check_systick_Sim )/1000;
		Setup_On_Off_Sim(GPIOB, On_Off_Sim,
										 GPIOB, Pin_PWKEY,
										 GPIOC, Pin_RESET, run_Time);
		return 0;
	}
	else
	{
		Delete_Buffer(sUart3);	
		return 1;
	}
}

int8_t Setup_SIM_Before_While(UART_BUFFER *sUart1, UART_BUFFER *sUart3, uint32_t runTime )
{
	uint32_t get_systick_Sim = HAL_GetTick();
	uint32_t run_Time=0;
	while(HAL_GetTick() - get_systick_Sim <=TIME_ON_SIM)
	{
	run_Time=(HAL_GetTick() - get_systick_Sim )/1000;
	Setup_On_Off_Sim(GPIOB, On_Off_Sim,
                   GPIOB, Pin_PWKEY,
									 GPIOC, Pin_RESET, run_Time);
	}
	Delete_Buffer(sUart3);
	return 1;
}

void Setup_On_Off_Sim(GPIO_TypeDef* GPIO1, uint16_t GPIO_Pin_On_Off_Sim, 
                      GPIO_TypeDef* GPIO2, uint16_t GPIO_Pin_PWKEY,  
                      GPIO_TypeDef* GPIO3, uint16_t GPIO_Pin_RESET, uint32_t timeOut)
{
	if(timeOut<=2)
	{
	HAL_GPIO_WritePin(GPIOB, GPIO_Pin_On_Off_Sim,GPIO_PIN_RESET);
	}
	
	if(timeOut>2 && timeOut<=3)
	{
	HAL_GPIO_WritePin(GPIOB, GPIO_Pin_PWKEY,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_Pin_RESET,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_Pin_On_Off_Sim,GPIO_PIN_SET);
	}
	
	if(timeOut>3 && timeOut <=6)
	{
	HAL_GPIO_WritePin(GPIOB, GPIO_Pin_PWKEY,GPIO_PIN_SET);
	}
	if(timeOut>6 && timeOut <=21)
	{
	HAL_GPIO_WritePin(GPIOB, GPIO_Pin_PWKEY,GPIO_PIN_RESET);
	}
}

int8_t Wait_SMS_Receive(UART_BUFFER *sUart1, UART_BUFFER *sUart3,char* response)
{
	uint8_t answer = 0;
		if(strstr(sUart3->sim_rx,response) != NULL) 
		{
			answer = 1;
		}
		else 
		{
			answer = 0;
		}
	return answer;
}


void Get_Real_Time(UART_BUFFER *sUart1, UART_BUFFER *sUart3, REAL_TIME *RTC_Current, uint8_t *Get_RTC, uint8_t *Get_RTC_Complete)
{
	if(*Get_RTC==1)
	{
		if(check_systick_CCLK == 0)
		{
			Transmit_Data_Uart(*sUart3->huart,"AT+CCLK?");
			get_systick_CCLK = HAL_GetTick();
			check_systick_CCLK = 1;
		}
		if(check_systick_CCLK == 1)
		{
			if(HAL_GetTick() - get_systick_CCLK > TIME_RESEND_COMMAND_IF_NOT_RECEIVE)
			{
				check_systick_CCLK = 0;
			}
		}
	}
	
	if(Check_CountBuffer_Complete_Uart3(sUart3)==1)
	{
			if(strstr(sUart3->sim_rx,"CCLK:") != NULL) 
			{
				Transmit_Data_Uart(*sUart1->huart, sUart3->sim_rx);
				for(uint16_t i=0; i<sUart3->countBuffer;i++)
				{
					if(sUart3->sim_rx[i] == '"' && sUart3->sim_rx[i+21] == '"')
					{
						RTC_Current->Seconds = (sUart3->sim_rx[i+16]- 48)*10 + sUart3->sim_rx[i+17] -48;
						RTC_Current->Minutes = (sUart3->sim_rx[i+13]- 48)*10 + sUart3->sim_rx[i+14] -48;
						RTC_Current->Hour    = (sUart3->sim_rx[i+10]- 48)*10 + sUart3->sim_rx[i+11] -48;
						
						RTC_Current->Date  = (sUart3->sim_rx[i+7]- 48)*10 + sUart3->sim_rx[i+8] -48;
						RTC_Current->Month = (sUart3->sim_rx[i+4]- 48)*10 + sUart3->sim_rx[i+5] -48;
						RTC_Current->Year  = (sUart3->sim_rx[i+1]- 48)*10 + sUart3->sim_rx[i+2] -48;
						*Get_RTC_Complete=1;
						check_systick_CCLK = 0;
						*Get_RTC=0;
						break;	
					}
				}
				if(Check_Receive_sendData_Control(sUart1,sUart3)==1) Delete_Buffer(sUart3);
			}
	}
}

int8_t Connect_Server_SIM(UART_BUFFER *sUart1, UART_BUFFER *sUart3)
{
	if(check_config_Sim==0) 
	{
		check_systick_Sim=HAL_GetTick();
		count_Connect_Server++;
		check_config_Sim++;
	}
	if(check_config_Sim==1 ) 
	{
		int8_t transmit_cfun = Transmit_CFUN(sUart1,sUart3);
		if(transmit_cfun ==1) check_config_Sim++;
		if(transmit_cfun ==-1) 
		{
			check_config_Sim=0;
		}
	}
	
	if(check_config_Sim==2)
	{
		int8_t config_uart_sim = Config_Uart_Sim(sUart1,sUart3);
		if(config_uart_sim ==1) check_config_Sim++;
		if(config_uart_sim ==-1) 
		{
			check_config_Sim=0;
		}
	}

	if(check_config_Sim==3) 
	{
		int8_t config_sms = Config_SMS(sUart1, sUart3);
		if(config_sms==1) check_config_Sim++;
		if(config_sms==-1) 
		{
			check_config_Sim=0;
		}
	}
	
	if(check_config_Sim==4) 
	{
		int8_t config_tcp=Config_TCP(sUart1, sUart3);
		if(config_tcp==1) 
		{
			check_config_Sim=0;
			count_Connect_Server=0;
			return 1;
		}
		if(config_tcp==-1) 
		{
			check_config_Sim=0;
		}
	}
	if(count_Connect_Server >= 3 - 1)
	{
		check_config_Sim=0;
		count_Connect_Server=0;
		return -1;
	}
	return 0;
}

int8_t Check_Disconnect_Error(UART_BUFFER *sUart1, UART_BUFFER *sUart3)
{
	if(Check_CountBuffer_Complete_Uart3(sUart3) == 1)
	{
		if(strstr(sUart3->sim_rx,"DISCONNECTED") != NULL) 
		{
			Transmit_Data_Uart(*sUart1->huart, sUart3->sim_rx);
			Delete_Buffer(sUart3);
			return 1;
		}
		
		if(strstr(sUart3->sim_rx,"ERROR") != NULL) 
		{
			Transmit_Data_Uart(*sUart1->huart, sUart3->sim_rx);
			Delete_Buffer(sUart3);
			return 1;
		}
	}
	return 0;
}

int8_t Check_Receive_sendData_Control(UART_BUFFER *sUart1,UART_BUFFER *sUart3)
{
	uint8_t answer=0;
	if(strstr(sUart3->sim_rx,"SUCCESS") != NULL) 
	{
		answer++;
	}
	
	if(strstr(sUart3->sim_rx,"CTL+") != NULL) 
	{
		answer++;
	}
	
	if(strstr(sUart3->sim_rx,">") != NULL) 
	{
		answer++;
	}
	if(strstr(sUart3->sim_rx,"DISCONNECTED") != NULL) 
	{
		answer++;
	}
	if(strstr(sUart3->sim_rx,"ERROR") != NULL) 
	{
		answer++;
	}
	if(strstr(sUart3->sim_rx,"FAIL") != NULL) 
	{
		answer++;
	}
	if(strstr(sUart3->sim_rx,"CCLK:") != NULL) 
	{
		answer++;
	}
	return answer;
}




