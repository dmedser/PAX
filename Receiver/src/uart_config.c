#include "uart_config.h"
#include "assertion.h"
#include "stddef.h"
#include "avr/io.h"

uint8_t init_uart(const struct uart_init* init_struct, uint32_t uart_freq)
{
	RT_ASSERT(init_struct != NULL, ERROR_TYPE_NULL_PTR);
	RT_ASSERT(init_struct->baud_rate > 0 && init_struct->baud_rate <= 115200, 
		  ERROR_TYPE_WRONG_PARAM);
	RT_ASSERT(assertCorrectDataBits(init_struct->data_bits), 
		  ERROR_TYPE_WRONG_PARAM);
	RT_ASSERT(assertCorrectStopBits(init_struct->stop_bits), 
		  ERROR_TYPE_WRONG_PARAM);
	RT_ASSERT(uart_freq > 0, ERROR_TYPE_WRONG_PARAM);
	
	uint8_t data_bits = 0;
	
	switch (init_struct->data_bits) {
	case UART_DATA_BITS_5 : data_bits = 0; break;
	case UART_DATA_BITS_6 : data_bits = 1; break;
	case UART_DATA_BITS_7 : data_bits = 2; break;
	case UART_DATA_BITS_8 : data_bits = 3; break;
	case UART_DATA_BITS_9 : data_bits = 7; break;
	default: break; //unreacheable
	}
	
	uint8_t stop_bits = 0;
	
	switch (init_struct->stop_bits) {
	case UART_STOP_BITS_1 : stop_bits = 0; break;
	case UART_STOP_BITS_2 : stop_bits = 1; break;
	default: break; //unreacheable
	}
	
	/* BaudRateDel = freq/(16*BaudRate) - 1
	 * ���������� - (int) (x + 0.5f)
	 */
	uint16_t ubbr = (uint16_t)(((float)(uart_freq)/(16*init_struct->baud_rate) - 1) + 0.5f);
	UCSR1C = (data_bits << UCSZ10) | (stop_bits << USBS1);
	UBRR1L = (ubbr  & 0x00FF);
	UBRR1H = (ubbr & 0x0F00) >> 8;
	
	return ERROR_TYPE_OK;
}

uint8_t enable_uart(void)
{
	RT_ASSERT(UBRR1H != 0 || UBRR1L != 0, ERROR_TYPE_BAUD_RATE_NOT_SET);
	UCSR1B =  (1 << RXEN1) | (1 << TXEN1);
	return ERROR_TYPE_OK;
}

/* 
 * ��� UDRE1 ��������������� � 1 �� ���������� �������� 
 * �������� ���������� ��������, ���� UDRE1 == 0 
 */ 
void uart_transmit(uint8_t data) 
{
	while(!(UCSR1A & (1 << UDRE1)));
	UDR1 = data;
}

void uart_transmit_float(float data)
{
	uint8_t *byte_ptr;
	byte_ptr = (uint8_t *)&data;
	for (uint8_t i = 0; i < sizeof(float); i++) {
		uart_transmit(*byte_ptr++);
	}
}