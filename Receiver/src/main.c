#ifndef _PAX_RX

#include "stdbool.h"
#include "avr/io.h"
#include "rf_config.h"
#include "uart_config.h"

/* Частота тактирования CPU = 16 МГц */
#define F_CPU (16000000)

void check_error(uint8_t  error)
{
    volatile uint8_t  error_type = error;
    if (error_type == ERROR_TYPE_OK)
        return;
    while(true);
}

/*
 * Приемник ожидает передачи данных по радиоканалу.
 * По окончанию передачи фрейма данных срабатывает прерывание, в котором
 * данные извлекаются из фрейма и передаются на ПК через UART
 */

int main(void) 
{
    uint8_t  ret_val;
	struct rf_init rf_init;
	
	/* Глобальное разрешение прерываний */
	SREG |= (1 << SREG_I);
	
	
	/* Инициализация радиомодуля */
	{
		rf_init.enable_crc = true;
		rf_init.channel_no = RF_CHANNEL_11;
		ret_val = init_rf(&rf_init); 
		check_error(ret_val);
		
		ret_val = enable_rf(RECEIVER);
		check_error(ret_val);
	}
	
	/* Инициализация UART */
	{
		struct uart_init uart_init;
        uart_init.baud_rate = 115200;
        uart_init.stop_bits = UART_STOP_BITS_2;
        uart_init.data_bits = UART_DATA_BITS_8;
        ret_val = init_uart(&uart_init, F_CPU);
        check_error(ret_val);
        ret_val = enable_uart();
        check_error(ret_val);
    }
	
	while(true);
}
#endif
