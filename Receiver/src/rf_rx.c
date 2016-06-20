#include "avr/io.h"
#include "rf_rx.h"
#include "uart_config.h"
#include "stdbool.h"
#include <util/delay.h>
#define F_CPU 16000000UL  

static uint8_t frame_count = 0;

enum frame_types {
	PREVIOUS,
	CURRENT,
	TEMPORARY
};

#define FRAME_SIZE (10)

static float prev_x[FRAME_SIZE];
static float prev_y[FRAME_SIZE];
static float prev_z[FRAME_SIZE];

static float curr_x[FRAME_SIZE];
static float curr_y[FRAME_SIZE];
static float curr_z[FRAME_SIZE];

static float tmp_x[FRAME_SIZE];
static float tmp_y[FRAME_SIZE];
static float tmp_z[FRAME_SIZE];


static bool it_is_1st_sample = true;
static bool filter_is_on = false;
static bool step_detection_is_on = false;
static bool first_max_is_found = false;

/*
 * ����������� 10-����������� �������  
 * �� ��������� ����������� ��������
 */
void moving_average_frame_smoothing(float* f)
{
	float tmp[FRAME_SIZE];
	for(int i = 0; i < FRAME_SIZE; i++) {
		tmp[i] = f[i];
	}
	
	f[0] = tmp[0];
	f[1] = (tmp[0] + tmp[1] + tmp[2])/3;
	f[2] = (tmp[0] + tmp[1] + tmp[2] + tmp[3] + tmp[4])/5;
	f[3] = (tmp[0] + tmp[1] + tmp[2] + tmp[3] + tmp[4] + tmp[5] + tmp[6])/7;
	f[4] = (tmp[0] + tmp[1] + tmp[2] + tmp[3] + tmp[4] + tmp[5] + tmp[6] + tmp[7] + tmp[8])/9;
	f[5] = (tmp[1] + tmp[2] + tmp[3] + tmp[4] + tmp[5] + tmp[6] + tmp[7] + tmp[8] + tmp[9])/9;
	f[6] = (tmp[3] + tmp[4] + tmp[5] + tmp[6] + tmp[7] + tmp[8] + tmp[9])/7;
	f[7] = (tmp[5] + tmp[6] + tmp[7] + tmp[8] + tmp[9])/5;
	f[8] = (tmp[7] + tmp[8] + tmp[9])/3;
	f[9] = tmp[9];	
}

ISR(USART1_RX_vect)
{
	char byte = UDR1;
	switch(byte) {
	case 'F': filter_is_on = true; break;
	case 'N': filter_is_on = false; break;
	case 'S': step_detection_is_on = true; break;
	default: break;
	}
}

void send_header_by_uart() 
{
	uart_transmit(0x7c);
	uart_transmit(0x6e);
}

void read_xyz_to_tmp(float* source) 
{
	tmp_x[frame_count] = *source++;
	tmp_y[frame_count] = *source++;
	tmp_z[frame_count] = *source;
}

void copy_tmp_to(enum frame_types target) 
{
	float *destination_x;
	float *destination_y;
	float *destination_z;

	switch(target) {
	case PREVIOUS: { 	
		destination_x = &prev_x[0];
		destination_y = &prev_y[0];
		destination_z = &prev_z[0];
		break;
	}
	case CURRENT: {
		destination_x = &curr_x[0];
		destination_y = &curr_y[0];
		destination_z = &curr_z[0];
		break;
	}
	default: break;	
	}
	
	for(uint8_t i = 0; i < FRAME_SIZE; i++) {
		destination_x[i] = tmp_x[i];
		destination_y[i] = tmp_y[i];
		destination_z[i] = tmp_z[i];
	} 
}

void smooth(enum frame_types target) 
{
	switch(target) {
	case PREVIOUS: {
		moving_average_frame_smoothing(&prev_x[0]);
		moving_average_frame_smoothing(&prev_y[0]);
		moving_average_frame_smoothing(&prev_z[0]);
		break;	
	}
	case CURRENT: {
		moving_average_frame_smoothing(&curr_x[0]);
		moving_average_frame_smoothing(&curr_y[0]);
		moving_average_frame_smoothing(&curr_z[0]);
		break;
	}
	case TEMPORARY: {
		moving_average_frame_smoothing(&tmp_x[0]);
		moving_average_frame_smoothing(&tmp_y[0]);
		moving_average_frame_smoothing(&tmp_z[0]);
		break;
	}
	default: break;
	}
}

void merge_prev_and_curr_to_tmp() {
	for(uint8_t i = 4, j = 0; i < 9; i++, j++) {
		tmp_x[j] = prev_x[i];
	}
	for(uint8_t i = 1, j = 5; i < 6; i++, j++) {
		tmp_x[j] = curr_x[i];
	}
	for(uint8_t i = 4, j = 0; i < 9; i++, j++) {
		tmp_y[j] = prev_y[i];
	}
	for(uint8_t i = 1, j = 5; i < 6; i++, j++) {
		tmp_y[j] = curr_y[i];
	}
	for(uint8_t i = 4, j = 0; i < 9; i++, j++) {
		tmp_z[j] = prev_z[i];
	}
	for(uint8_t i = 1, j = 5; i < 6; i++, j++) {
		tmp_z[j] = curr_z[i];
	}
}

void send_frame_by_uart(bool save_prev_frame) {
	for (uint8_t i = 0; i < FRAME_SIZE; i++) {
		uart_transmit_float(tmp_x[i]);
		uart_transmit_float(tmp_y[i]);
		uart_transmit_float(tmp_z[i]);
	}
	if(save_prev_frame) {
		for (uint8_t i = 0; i < FRAME_SIZE; i++) {	
				prev_x[i] = tmp_x[i];
				prev_y[i] = tmp_y[i];
				prev_z[i] = tmp_z[i];
			}
	}
}


ISR(TRX24_RX_END_vect)
{
	/* �������� ������������ ����������� ����� */
	if (PHY_RSSI & (1 << RX_CRC_VALID)) {
				
		float *float_ptr = (float*)&TRXFBST;
		read_xyz_to_tmp(float_ptr);
		frame_count++;
		
		//FILTER ON
		if(filter_is_on) {
			 /* ������ ������� ���������� � �����, �� �� UART �� �������� */
			if(it_is_1st_sample) {
				if(frame_count == FRAME_SIZE) {
					copy_tmp_to(PREVIOUS);
					it_is_1st_sample = false;
					frame_count = 0;
				}
			}
			else {
				if(frame_count == FRAME_SIZE) {
					copy_tmp_to(CURRENT);
					/*
					 * 1. ���������� ���������� � ������� �������
					 * 2. ����������� ��������� �������� ���������� ������� � ������ �������� 
					 *    ������� 
					 * 3. �������� ����� ������� �� ���������� 18-�� �������� - ��������� 5
					 *    �� ���������� � ������ ���� �� �������
					 * 4. ���������� ���������� �������, ���������� �� �� UART
					 */
					smooth(PREVIOUS);
					smooth(CURRENT);
					merge_prev_and_curr_to_tmp();
					smooth(TEMPORARY);
					send_frame_by_uart(true);
					send_header_by_uart();
					frame_count = 0;
				}
			}
		}
		//FILTER OFF
		else { 
			if(frame_count == FRAME_SIZE) {
				send_frame_by_uart(false);
				send_header_by_uart();
				frame_count = 0;
			}
		}
	}
}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	

		
		