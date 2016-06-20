#include "timer.h"
#include "avr/interrupt.h"
#include "bma180_config.h"
#include "rf_tx.h"
#include "assertion.h"
#include "uart_config.h"

#define SAMPLE_SIZE	3

uint8_t  init_timer(uint8_t samples_per_sec)
{
	RT_ASSERT(1 <= samples_per_sec, ERROR_TYPE_WRONG_PARAM);
	SREG   |= (1 << SREG_I);
	TIMSK1 |= (1 << OCIE1A);
	
	float tmp = (float)((1/(float)samples_per_sec)*F_CPU/(float)256);
	uint8_t tmpl = (uint8_t)((uint32_t)tmp & 0xFF);
	uint8_t tmph = (uint8_t)((uint32_t)tmp >> 8);
	OCR1A = tmp;
	TCCR1B = 0x0;
	TCCR1B |= (1 << WGM12);		/* CTC mode, TOP = OCR1A */
	TCCR1B |= 0x4;				/* cs2:0 = clk/256 = 62500 ò/ñ */
	return ERROR_TYPE_OK;
}

ISR(TIMER1_COMPA_vect) {
	volatile uint8_t xl, xh, yl, yh, zl, zh;
	int16_t tmp_buf[SAMPLE_SIZE];
	
	xl = read_from_bma180(ACC_X_LSB);
	xh = read_from_bma180(ACC_X_MSB);
	yl = read_from_bma180(ACC_Y_LSB);
	yh = read_from_bma180(ACC_Y_MSB);
	zl = read_from_bma180(ACC_Z_LSB);
	zh = read_from_bma180(ACC_Z_MSB);

	tmp_buf[0] = xl >> 2;
	tmp_buf[0]|= ((uint16_t) xh << 6);
	if (tmp_buf[0] & 0x2000) {
		tmp_buf[0] |= 0xC000;
	}
	
	tmp_buf[1] = yl >> 2;
	tmp_buf[1]|= ((uint16_t) yh << 6);
	if (tmp_buf[1] & 0x2000) { 
		tmp_buf[1] |= 0xC000;
	}
	
	tmp_buf[2] = zl >> 2;
	tmp_buf[2]|= ((uint16_t) zh << 6);
	if (tmp_buf[2] & 0x2000) {
		tmp_buf[2] |= 0xC000;
	}
	
	float sample[SAMPLE_SIZE];
	
	sample[0] = (float)2.0*tmp_buf[0]/8191.0;
	sample[1] = (float)2.0*tmp_buf[1]/8191.0;
	sample[2] = (float)2.0*tmp_buf[2]/8191.0;
	
	rf_transmit(sample);
}