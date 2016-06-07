#include "avr/io.h"
#include "rf_rx.h"
#include "uart_config.h"
#include "stdbool.h"
#include <util/delay.h>
#define F_CPU 16000000UL  

static uint8_t frame_count = 0;

/*
 * Сглаживание 10-элементного массива  
 * по алгоритму скользящего среднего
 */

void moving_average_smoothing(float* f)
{
	float tmp[10];
	for(int i = 0; i < 10; i++) {
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


static float prev_x[10];
static float prev_y[10];
static float prev_z[10];

static float curr_x[10];
static float curr_y[10];
static float curr_z[10];


static bool it_is_1st_sample = true;
static bool filter_is_on = false;


ISR(USART1_RX_vect)
{
	char byte = UDR1;
	switch(byte) {
		case 'F': filter_is_on = true; break;
		case 'N': filter_is_on = false; break;
		default: break;
	}
}


ISR(TRX24_RX_END_vect)
{
	/* Проверка корректности контрольной суммы */
	if (PHY_RSSI & (1 << RX_CRC_VALID)) {

		/* Содержимое фрейма */
		//uint8_t tmp_frame[MAX_FRAME_SIZE];
		//uint8_t frame_length;
		//frame_length = TST_RX_LENGTH;
		//memcpy(&tmp_frame[0], (void*)&TRXFBST, frame_length);

		float sm_x[10];
		float sm_y[10];
		float sm_z[10];
		
		float *float_ptr = (float*)&TRXFBST;
		
		//FILTER ON
		if(filter_is_on) {
			if(it_is_1st_sample) {
				prev_x[frame_count] = *float_ptr++;
				prev_y[frame_count] = *float_ptr++;
				prev_z[frame_count] = *float_ptr;
				frame_count++;
				if(frame_count == 10) {
					it_is_1st_sample = false;
					frame_count = 0;
				}
			}
			else {
				curr_x[frame_count] = *float_ptr++;
				curr_y[frame_count] = *float_ptr++;
				curr_z[frame_count] = *float_ptr;
				frame_count++;
				if(frame_count == 10) {
					
					/*
					 * 1. Сглаживаем предыдущую и текущую десятки
					 * 2. Отбрасываем последнее значение предыдущей десятки и первое значение 
					 *    текущей 
					 * 3. Выбираем новую десятку из оставшихся 18-ти значений - последние 5
					 *    из предыдущей и первые пять из текущей
					 * 4. Сглаживаем полученную десятку
					 */
					moving_average_smoothing(&prev_x[0]);
					moving_average_smoothing(&prev_y[0]);
					moving_average_smoothing(&prev_z[0]);
					
					moving_average_smoothing(&curr_x[0]);
					moving_average_smoothing(&curr_y[0]);
					moving_average_smoothing(&curr_z[0]);
					
					for(int i = 4, j = 0; i < 9; i++, j++)
					sm_x[j] = prev_x[i];
					for(int i = 1, j = 5; i < 6; i++, j++)
					sm_x[j] = curr_x[i];
					for(int i = 4, j = 0; i < 9; i++, j++)
					sm_y[j] = prev_y[i];
					for(int i = 1, j = 5; i < 6; i++, j++)
					sm_y[j] = curr_y[i];
					for(int i = 4, j = 0; i < 9; i++, j++)
					sm_z[j] = prev_z[i];
					for(int i = 1, j = 5; i < 6; i++, j++)
					sm_z[j] = curr_z[i];
					
					moving_average_smoothing(&sm_x[0]);
					moving_average_smoothing(&sm_y[0]);
					moving_average_smoothing(&sm_z[0]);
					
					for (uint8_t i = 0; i < 10; i++)
					{
						uart_transmit_float(sm_x[i]);
						uart_transmit_float(sm_y[i]);
						uart_transmit_float(sm_z[i]);
						prev_x[i] = sm_x[i];
						prev_y[i] = sm_y[i];
						prev_z[i] = sm_z[i];
					}
					uart_transmit(0x7c);
					uart_transmit(0x6e);
					frame_count = 0;
				}
			}
		}
		else { 
			//FILTER OFF
			sm_x[frame_count] = *float_ptr++;
			sm_y[frame_count] = *float_ptr++;
			sm_z[frame_count] = *float_ptr;
			frame_count++;
			if(frame_count == 10) {
				for (uint8_t i = 0; i < 10; i++)
				{
					uart_transmit_float(sm_x[i]);
					uart_transmit_float(sm_y[i]);
					uart_transmit_float(sm_z[i]);
				}
			uart_transmit(0x7c);
			uart_transmit(0x6e);
			frame_count = 0;
			}
		}
				
		
	}
}
	