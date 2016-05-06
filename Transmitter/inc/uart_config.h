#ifndef __UART_CONFIG_H__
#define __UART_CONFIG_H__

#include "stdint.h"
#include "error_types.h"

/**
 * \ingroup config
 * \defgroup configUart ������������ UART
 */


/**
 * \ingroup configUart
 * ����� ��� ������
 */
enum uart_data_bits {
	UART_DATA_BITS_5, 
	UART_DATA_BITS_6, 
	UART_DATA_BITS_7, 
	UART_DATA_BITS_8, 
	UART_DATA_BITS_9 
};

/**
 * \ingroup configUart
 * ��������� �� ���������� ��������
 */
#define assertCorrectDataBits(x) ( \
				   ((x) == UART_DATA_BITS_5) || \
				   ((x) == UART_DATA_BITS_6) || \
				   ((x) == UART_DATA_BITS_7) || \
				   ((x) == UART_DATA_BITS_8) || \
				   ((x) == UART_DATA_BITS_9) \
				 ) 

/**
 * \ingroup configUart
 * ����� �������� ���
 */
enum uart_stop_bits {
	UART_STOP_BITS_1,
	UART_STOP_BITS_2
}; 

/**
 * \ingroup configUart
 * ��������� �� ���������� ��������
 */
#define assertCorrectStopBits(x) ( \
				   ((x) == UART_STOP_BITS_1) || \
				   ((x) == UART_STOP_BITS_2) \
				 ) 

/**
 * \ingroup configUart
 * ��������� ��������� UART
 * \struct uart_init
 * \var uart_init::baud_rate
 * Baudrate: 0 - 115200
 * \var uart_init::data_bits
 * ����� ��� ������
 * \var uart_init::stop_bits
 * ����� �������� ���
 */
struct uart_init  {
	uint32_t baud_rate; 
	enum uart_data_bits data_bits;
	enum uart_stop_bits stop_bits;
};


/**
 * \ingroup configUart
 * ��������� UART
 * \param [in] init_struct ��������� �� ��������� ��������� UART
 * \param [in] cpu_freq ������� ������������ UART
 * \return ��� ������
 */
enum error_type init_uart(const struct uart_init* init_struct, uint32_t cpu_freq);

/**
 * \ingroup configUart
 * �������� UART
 * \return ��� ������
 */
enum error_type enable_uart(void);


void uart_transmit(uint8_t data);
void uart_transmit_int16(int16_t data); 
void uart_transmit_float(float data);
#endif
