#include <avr/io.h>
#include <util/delay.h>
#include "stdbool.h"
#include "avr/interrupt.h"
#include "bma180_config.h"
#include "spi_config.h"
#include "error_types.h"

/* Частота тактирования CPU = 16 МГц */
#define F_CPU 16000000UL  


uint8_t read_from_bma180(uint8_t address)
{
	uint8_t byte;
	
	/* Старший бит адреса устанавливается в 1 - режим чтения */
	address |= (1 << MSB);
	set_spi_transfer_status(ON);
	spi_transmit(address);
	byte = spi_receive();
	set_spi_transfer_status(OFF);
	return byte;
}

void write_to_bma180(uint8_t address, char data)
{
	/* Старший бит адреса устанавливается в 0 - режим записи */
	address &= ~(1 << MSB);
	set_spi_transfer_status(ON);
	spi_transmit(address);
	spi_transmit(data);
	set_spi_transfer_status(OFF);
}

void write_to_bma180_masked(uint8_t address, uint8_t data, uint8_t mask)
{
	volatile uint8_t read_val = read_from_bma180(address);
	data = ((read_val & mask) | data);
	write_to_bma180(address, data);
}


uint8_t  init_bma180(enum bma180_acceleration_range range, enum bma180_bandwidth bandwidth)
{
	
	if(read_from_bma180(CHIP_ID) != BMA180_ID) {
		return ERROR_TYPE_BMA180_ID_MISMATCH;
	}

	write_to_bma180(RESET, SOFT_RESET_VAL);
	/* После перезапуска необходима задержка не менее 10 мкс */
	_delay_us(20);

	enable_img_reg_write();
	
	write_to_bma180_masked(BW_TCS, (bandwidth << 4), 0xF);
	write_to_bma180_masked(OFFSET_LSB1, (range << 1), 0xF1);
	
	disable_img_reg_write();
	
	calibrate(OFFSET_X_VAL, OFFSET_Y_VAL, OFFSET_Z_VAL);

	/*
	 * Экспериментально установлено, что для корректной работы акселерометра
	 * необходимо отключить режим WAKE_UP и выполнить selftest, 
	 */
	write_to_bma180(CTRL_REG0, (1 << DIS_WAKEUP)); 
	write_to_bma180(CTRL_REG0, (1 << ST0));
	/* Процедура selftest'а занимает менее 10 мкс */
	_delay_ms(20);	
	

	/*
	uint8_t x_msb;
	uint8_t y_msb;
	uint8_t z_msb;
	
	uint8_t offset_lsb2, offset_lsb1;
	
	x_msb = read_from_bma180(OFFSET_X);
	y_msb = read_from_bma180(OFFSET_Y);
	z_msb = read_from_bma180(OFFSET_Z);
	
	offset_lsb1 = read_from_bma180(OFFSET_LSB1);
	offset_lsb2 = read_from_bma180(OFFSET_LSB2);
	*/
	
	return ERROR_TYPE_OK;
}

void calibrate(uint16_t offset_x, uint16_t offset_y, uint16_t offset_z)
{
	enable_img_reg_write();
	write_to_bma180(OFFSET_X, (offset_x & 0xFF0) >> 4);
	write_to_bma180(OFFSET_Y, (offset_y & 0xFF0) >> 4);
	write_to_bma180(OFFSET_Z, (offset_z & 0xFF0) >> 4);
	write_to_bma180_masked(OFFSET_LSB1, (offset_x & 0x00F) << 4, 0x0F);
	write_to_bma180_masked(OFFSET_LSB2, (((offset_z & 0x00F) << 4) | (offset_y & 0x00F)), 0x00);

	disable_img_reg_write();
}

void enable_img_reg_write(void)
{
	write_to_bma180_masked(CTRL_REG0, 1 << EE_W, ~(1 << EE_W));
}

void disable_img_reg_write(void)
{
	write_to_bma180_masked(CTRL_REG0, ~(1 << EE_W), ~(1 << EE_W));
}


/*
 * После выполнения calibrate_axis можно считывать новое значение оффсета 
 * для выбранной оси
 */

void calibrate_axis(enum axis calibrated_axis) {
	uint8_t en_offset;
	uint8_t mask;
	switch(calibrated_axis) {
	case X: en_offset = EN_OFFSET_X, mask = 0x60; break;
	case Y: en_offset = EN_OFFSET_Y, mask = 0xA0; break;
	case Z: en_offset = EN_OFFSET_Z, mask = 0xC0; break;
	default: break;
	};
	
	enable_img_reg_write();
	
	/* Выбор режима автокалибровки - COARSE */
	write_to_bma180_masked(CTRL_REG4, COARSE, 0xFC);
	write_to_bma180_masked(CTRL_REG1, (1 << en_offset), mask);
	while ((read_from_bma180(CTRL_REG1) & (1 << en_offset)) != 0);
	
	disable_img_reg_write();
}