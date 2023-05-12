#include "user_sim.h"

char News[BYTE_OF_THE_NEWS];
uint32_t count_news=0;
uint32_t getTick_error_cipsend=0;
uint8_t check_cipsend=0;
uint8_t send_data=0;
uint8_t check_packing_news=0;

char Sim_Control[4]= "CTL+";

uint32_t flash_addr_write=FLASH_ADDR_PAGE_NEWS_START;
uint32_t flash_addr_read =FLASH_ADDR_PAGE_NEWS_START;
uint32_t flash_page_write=FLASH_ADDR_PAGE_NEWS_START;
uint32_t flash_page_read =FLASH_ADDR_PAGE_NEWS_START;

void Control_Write_News_Flash(void);
void Control_Read_News_Flash(void);
void Write_Data_News(REAL_TIME *RTC_Current, char News[], uint32_t lengthNews, uint32_t countNews);
void Uint_To_Char_Sim(char time[], uint32_t stamp, uint16_t *location);
int8_t Error_Cipsend(void);
	
int8_t SendData_Server(UART_BUFFER *sUart1, UART_BUFFER *sUart3, REAL_TIME *RTC_Current)
{
	if(Error_Cipsend() == 1) return -1;
	
	if(send_data == 1)
	{
		if(check_cipsend == 0) 
		{
			if(RTC_Current->Send_Data_Server == 1)
			{
				send_data = 0;
				return 0;
			}
			if(flash_addr_read != flash_addr_write)
			{
				FLASH_ReadNews(flash_addr_read, News);
				if(News[31] == ' ')
				{
					Transmit_Data_Uart(*sUart3->huart,"AT+CIPSEND=1,32");
					Transmit_Data_Uart(*sUart1->huart,"AT+CIPSEND=1,32");
					check_cipsend=1;
					getTick_error_cipsend = HAL_GetTick();
				}
				else
				{
					Control_Read_News_Flash();
				}
			}
		}
		if(check_cipsend == 1)
		{
			if(Check_CountBuffer_Complete_Uart3(sUart3)==1)
			{
				if(Check_Disconnect_Error(sUart1, sUart3)!=1) 
				{
					if(strstr(sUart3->sim_rx,">") != NULL) 
					{
						Transmit_Data_Uart(*sUart1->huart, sUart3->sim_rx);
						Transmit_Data_Uart(*sUart3->huart,News);
						Transmit_Data_Uart(*sUart1->huart,News);
						check_cipsend = 2;
						getTick_error_cipsend = HAL_GetTick();
					}
					if(Check_Receive_sendData_Control(sUart1,sUart3)==1) Delete_Buffer(sUart3);
				}
				else
				{
					check_cipsend = 0;
					return 1;
				}
			}
		}
			
		if(check_cipsend==2)
		{
			if(Check_CountBuffer_Complete_Uart3(sUart3)==1)
			{
				if(Check_Disconnect_Error(sUart1, sUart3)!=1) 
				{
					if(strstr(sUart3->sim_rx,"SUCCESS") != NULL) 
					{
						Transmit_Data_Uart(*sUart1->huart, sUart3->sim_rx);
						if(Check_Receive_sendData_Control(sUart1,sUart3)==1) Delete_Buffer(sUart3);
						Control_Read_News_Flash();
						check_cipsend=0;
						send_data=0;
						FLASH_Write_Addr_Page_Write_Read(FLASH_ADDR_PAGE_253, flash_addr_read, flash_addr_write);
					}
					if(strstr(sUart3->sim_rx,"FAIL") != NULL) 
					{
						check_cipsend = 0;
						Delete_Buffer(sUart3);
						return 1;
					}
				}
				else
				{
					check_cipsend = 0;
					return 1;
				}
			}
		}
	}
	return 0;
}

void Reset_SendData(void)
{
	check_cipsend=0;
}

void Packing_News(REAL_TIME *RTC_Current)
{
	if(check_cipsend == 0)
	{
		send_data = 0;
	}
	
	if(send_data == 0)
	{
		if(RTC_Current->Send_Data_Server == 1 )
		{
			if(check_packing_news == 1)
			{
				count_news++;	
				Write_Data_News(RTC_Current, News, BYTE_OF_THE_NEWS, count_news);
				if(flash_addr_write == flash_page_write)
				{
					FLASH_WriteNews_Earse(flash_addr_write, News, FLASH_ADDR_PAGE_253, flash_addr_read, flash_addr_write);
				}
				else
				{
					FLASH_WriteNews(flash_addr_write, News, FLASH_ADDR_PAGE_253, flash_addr_read, flash_addr_write);
				}
				Control_Write_News_Flash();
				send_data = 1;
				check_packing_news = 0;
			}
			RTC_Current->Send_Data_Server = 0;
		}
		
		if(flash_addr_read != flash_addr_write )
		{
			send_data = 1;
		}
	}
	
	if(RTC_Current->Seconds >28 && RTC_Current->Seconds <32)
	{
		check_packing_news = 1;
	}
}

void Control_Write_News_Flash(void)
{
	if(flash_addr_write + BYTE_OF_THE_NEWS > flash_page_write + FLASH_BYTE_OF_PAGE)
	{
		flash_page_write = flash_page_write + FLASH_BYTE_OF_PAGE;
		flash_addr_write = flash_page_write;
	}
	else
	{
		flash_addr_write = flash_addr_write + BYTE_OF_THE_NEWS;
		if(flash_addr_write == flash_page_write + FLASH_BYTE_OF_PAGE)
		{
			flash_page_write = flash_page_write + FLASH_BYTE_OF_PAGE;
		}
	}
	
	if(flash_page_write == FLASH_ADDR_PAGE_NEWS_END )
	{
		flash_page_write = FLASH_ADDR_PAGE_NEWS_START;
		flash_addr_write = flash_page_write;
	}	
	
	if(flash_addr_write <= flash_addr_read)
	{
		if(flash_page_write == flash_page_read)
		{
			flash_page_read = flash_page_read + FLASH_BYTE_OF_PAGE;
			flash_addr_read = flash_page_read ;
			if(flash_page_read == FLASH_ADDR_PAGE_NEWS_END )
			{
				flash_page_read = FLASH_ADDR_PAGE_NEWS_START;
				flash_addr_read = flash_page_read;
			}
		}
	}
}

void Control_Read_News_Flash(void)
{
	if(flash_addr_read + BYTE_OF_THE_NEWS > flash_page_read + FLASH_BYTE_OF_PAGE)
	{
		flash_page_read = flash_page_read + FLASH_BYTE_OF_PAGE;
		flash_addr_read = flash_page_read;
	}
	else
	{
		flash_addr_read = flash_addr_read + BYTE_OF_THE_NEWS;
		if(flash_addr_read == flash_page_read + FLASH_BYTE_OF_PAGE)
		{
			flash_page_read = flash_page_read + FLASH_BYTE_OF_PAGE;
		}
	}
	
	if(flash_page_read == FLASH_ADDR_PAGE_NEWS_END )
	{
		flash_page_read = FLASH_ADDR_PAGE_NEWS_START;
		flash_addr_read = flash_page_read;
	}
}

int8_t Error_Cipsend(void)
{
	if(check_cipsend == 1)
	{
		if(HAL_GetTick() - getTick_error_cipsend > TIME_ERROR_CIPSEND) 
		{
			check_cipsend = 0;
			return 1;
		}
	}
	
	if(check_cipsend == 2)
	{
		if(HAL_GetTick() - getTick_error_cipsend > TIME_ERROR_CIPSEND) 
		{
			check_cipsend = 0;
			return 1;
		}
	}
	return 0;
}

void Get_Addr_Read_Write(void)
{
	if(FLASH_ReadData32(FLASH_ADDR_PAGE_253) != 0 && FLASH_ReadData32(FLASH_ADDR_PAGE_253+4) != 0)
	{
		flash_addr_read = FLASH_ReadData32(FLASH_ADDR_PAGE_253);
		flash_page_read = (flash_addr_read/1024)*1024;
		flash_addr_write= FLASH_ReadData32(FLASH_ADDR_PAGE_253 + 4) + BYTE_OF_THE_NEWS;
		flash_page_write= (flash_addr_write/1024)*1024;
	}
}

int8_t Receive_Control_Setup(UART_BUFFER *sUart1, UART_BUFFER *sUart3, uint8_t *check_connect, uint32_t *time1, uint32_t *time2,uint32_t *time3)
{
	if(Check_CountBuffer_Complete_Uart3(sUart3)==1)
	{
		if(Check_Disconnect_Error(sUart1, sUart3)!=1) 
		{
			if(strstr(sUart3->sim_rx,Sim_Control) != NULL)
			{
				Transmit_Data_Uart(*sUart1->huart, sUart3->sim_rx);
				if(strstr(sUart3->sim_rx,"ONLED5") != NULL)
				{
					HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
				}
				if(strstr(sUart3->sim_rx,"OFFLED5") != NULL)
				{
					HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
				}
				if(strstr(sUart3->sim_rx,"RESET") != NULL)
				{
					if(Check_Receive_sendData_Control(sUart1,sUart3)==1) Delete_Buffer(sUart3);
					return 1;
				}
				if(Check_Receive_sendData_Control(sUart1,sUart3)==1) Delete_Buffer(sUart3);
			}	
		}
		else
		{
			*check_connect=1;
		}
	}
	return 0;
}

void Write_Data_News(REAL_TIME *RTC_Current, char News[], uint32_t lengthNews, uint32_t countNews)
{
	uint16_t i=0;
	if(RTC_Current->Year<10)
	{
		News[i]='0';
		i++;
	}
	Uint_To_Char_Sim(News, RTC_Current->Year, &i);
	News[i]='/';
	i++;
	
	if(RTC_Current->Month<10)
	{
		News[i]='0';
		i++;
	}
	Uint_To_Char_Sim(News, RTC_Current->Month, &i);
	News[i]='/';
	i++;
	
	if(RTC_Current->Date<10)
	{
		News[i]='0';
		i++;
	}
	Uint_To_Char_Sim(News, RTC_Current->Date, &i);
	
	News[i]=',';
	i++;
	if(RTC_Current->Hour<10)
	{
		News[i]='0';
		i++;
	}
	Uint_To_Char_Sim(News, RTC_Current->Hour, &i);
	News[i]=':';
	i++;
	if(RTC_Current->Minutes<10)
	{
		News[i]='0';
		i++;
	}
	Uint_To_Char_Sim(News, RTC_Current->Minutes, &i);
	News[i]=':';
	i++;
	if(RTC_Current->Seconds<10)
	{
		News[i]='0';
		i++;
	}
	Uint_To_Char_Sim(News, RTC_Current->Seconds, &i);
	News[i]=',';
	i++;
	Uint_To_Char_Sim(News, countNews, &i);
	while(i<lengthNews-1)
	{
		News[i]=' ';
		i++;
	}
	News[i]=' ';
}

int8_t Check_Receive_sendData_Control(UART_BUFFER *sUart1,UART_BUFFER *sUart3)
{
	uint8_t answer=0;
	if(strstr(sUart3->sim_rx,"SUCCESS") != NULL) 
	{
		answer++;
	}
	
	if(strstr(sUart3->sim_rx,Sim_Control) != NULL) 
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
	return 1;
}

void Uint_To_Char_Sim(char time[], uint32_t stamp, uint16_t *location)
{
	uint16_t lengthStamp=1;
	uint32_t Division=10;
	while(stamp/Division>=1)
	{
		Division=Division*10;
		(lengthStamp)++;
	}
	int j=lengthStamp+*location -1;
	for(;j>=*location;j--)
	{
	time[j]=stamp%10+ASCII_VALUE_NUMBER;
	stamp=stamp/10;
	}
	*location=*location + lengthStamp;
}

