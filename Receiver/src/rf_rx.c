#include "avr/io.h"
#include "rf_rx.h"
#include "uart_config.h"

ISR(TRX24_RX_END_vect)
{
	/* Проверка корректности контрольной суммы */
	if (PHY_RSSI & (1 << RX_CRC_VALID)) {

		/* Содержимое фрейма */
		//uint8_t tmp_frame[MAX_FRAME_SIZE];
		//uint8_t frame_length;
		//frame_length = TST_RX_LENGTH;
		//memcpy(&tmp_frame[0], (void*)&TRXFBST, frame_length);
		
		volatile float accs[3];

		float *float_ptr = (float*)&TRXFBST;
		accs[0] = *float_ptr++;
		accs[1] = *float_ptr++;
		accs[2] = *float_ptr;
		
		uart_transmit_float(accs[0]);
		uart_transmit_float(accs[1]);
		uart_transmit_float(accs[2]);
	}
}
