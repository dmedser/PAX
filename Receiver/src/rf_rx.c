#include "avr/io.h"
#include "rf_rx.h"
#include "uart_config.h"
#include "stdbool.h"
#include <util/delay.h>
#define F_CPU 16000000UL  


/* Текущее количество значений ускорений в десятке */
static uint8_t frame_counter = 0;

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


static bool it_is_1st_frame = true;
static bool filter_is_on = false;

/*
 * Сглаживание 10-элементного массива  
 * по алгоритму скользящего среднего. 
 * Первое и последнее значение не усредняются, 
 * что приводит к разрыву непрерывной прямой на границах десяток 
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
	tmp_x[frame_counter] = *source++;
	tmp_y[frame_counter] = *source++;
	tmp_z[frame_counter] = *source;
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

static float st_d;						// СКО - среднеквадратическое отколнение
static uint32_t total_samples;			// Количество полученных значений ускорений, начиная со старта программы 
static float curr_avg = 0;				// Среднее значение всех полученных значений ускорений 
static float prev_avg;
static bool there_was_a_peak = false;
static bool there_was_a_pit = true;
static float curr_z_val;				// Текущее значение forward-ускорения, см. (*)

void calc_1st_avg(float* frame) 
{
	for(uint8_t i = 0; i < FRAME_SIZE; i++) {
		curr_avg += frame[i];
	}
	curr_avg /= total_samples;
}

void calc_avg(float new_val) {
	curr_avg = prev_avg + (new_val - prev_avg)/total_samples;
}

void calc_1st_std(float *frame) 
{
	float sum = 0.0;
	for(uint8_t i = 0; i < FRAME_SIZE; i++) {
		sum += (frame[i] - curr_avg)*(frame[i] - curr_avg);
	}
	st_d = sqrtf((1.0/(total_samples - 1))*sum);
}

void calc_std(float new_val) 
{
	st_d = sqrtf(((total_samples - 2)*st_d*st_d + (new_val - curr_avg)*(new_val - prev_avg))/(total_samples - 1));
}

/*
 * Ось Z акселерометра располагаем по направлению движения - forward-ускорение (*),  
 * Считаем, что момент шага наступает когда выполняются условия:
 * 1. Предварительно был зафиксирован пик, т.е. нога поднялась, значение forward-ускорения 
 *    стало больше чем модуль среднего + СКО * коэффициент (**)									
 * 2. Значение forward-ускорения стало меньше чем среднее * коэффициент - СКО * коэффициент (***)
 *	
 * На "обнаружительную способность" влияют коэффициеты. 
 * Для фильтрации по времени (чтобы диод не зажигался несколько раз в момент шага) можно пропускать 
 * проверку на пики/ямы в течение нескольких выборок после обнаружения момента шага
 */

ISR(TRX24_RX_END_vect)
{
	/* Проверка корректности контрольной суммы */
	if (PHY_RSSI & (1 << RX_CRC_VALID)) {
				
		float *float_ptr = (float*)&TRXFBST;
		read_xyz_to_tmp(float_ptr);
		curr_z_val = tmp_z[frame_counter];
		frame_counter++;
		total_samples++;
		
		PORTD |= (1 << PD4);
		
		//FILTER ON
		if(filter_is_on) {
			 /* Первую десятку записываем в буфер, но по UART не передаем */
			if(it_is_1st_frame) {
				if(frame_counter == FRAME_SIZE) {
					copy_tmp_to(PREVIOUS);
					it_is_1st_frame = false;
					frame_counter = 0;
					calc_1st_avg(&tmp_z[0]);
					calc_1st_std(&tmp_z[0]);
				}
			}
			else {
				prev_avg = curr_avg;
				calc_avg(curr_z_val);
				calc_std(curr_z_val);
				
				if(there_was_a_pit && (curr_z_val > (fabs(curr_avg) + 1.8*st_d))) { // (**)
					there_was_a_peak = true;
					there_was_a_pit = false;
				}
				
				if(there_was_a_peak && (curr_z_val < curr_avg)) { // (***)
					PORTD &= ~(1 << PD4);
					there_was_a_peak = false;
					there_was_a_pit = true;
				}
					
				if(frame_counter == FRAME_SIZE) {
					copy_tmp_to(CURRENT);
					/*
					 * 1. Сглаживаем предыдущую и текущую десятки
					 * 2. Отбрасываем последнее значение предыдущей десятки и первое значение 
					 *    текущей 
					 * 3. Выбираем новую десятку из оставшихся 18-ти значений - последние 5
					 *    из предыдущей и первые пять из текущей
					 * 4. Сглаживаем полученную десятку, отправляем ее по UART
					 */
					smooth(PREVIOUS);
					smooth(CURRENT);
					merge_prev_and_curr_to_tmp();
					smooth(TEMPORARY);
					send_frame_by_uart(true);
					send_header_by_uart();
					frame_counter = 0;
				}
			}
		}
		//FILTER OFF
		else { 
			if(frame_counter == FRAME_SIZE) {
				send_frame_by_uart(false);
				send_header_by_uart();
				frame_counter = 0;
			}
		}
	}
}
	