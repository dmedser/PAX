#include "timer.h"
#include "avr/interrupt.h"
#include "assertion.h"
#include "ring_buff.h"
#include "uart_config.h"

enum error_type init_timer(uint8_t samples_per_sec)
{
	RT_ASSERT(1 <= samples_per_sec, ERROR_TYPE_WRONG_PARAM);
	SREG   |= (1 << SREG_I);
	TIMSK1 |= (1 << OCIE1A);

	OCR1A  = (1/samples_per_sec)*F_CPU/256;
	
	TCCR1B |= (1 << WGM12);	 /* CTC mode, TOP = OCR1A */
	TCCR1B |= 0x4;           /* cs2:0 = clk/256 = 62500 ò/ñ */
	return ERROR_TYPE_OK;
}

ISR(TIMER1_COMPA_vect) 
{

	volatile int16_t accs[3];
	volatile uint16_t *byte_ptr = get_elem();
	accs[0] = (int16_t)(*byte_ptr);
	accs[1] = (int16_t)(*(byte_ptr + 4));
	accs[2] = (int16_t)(*(byte_ptr + 8));
	
	if(accs) {
		uart_transmit_float(accs[0]);
		uart_transmit_float(accs[1]);
		uart_transmit_float(accs[2]);
			
		//for(uint8_t i = 0; i < SAMPLE_SIZE; i++) 
		//	uart_transmit_float(accs[i]);
	}
}