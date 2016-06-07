#ifndef BMA180_CONFIG_H_
#define BMA180_CONFIG_H_
#include <stdio.h>


// Старший и младший биты байта
#define MSB			7
#define LSB			0

enum axis {
	X,
	Y,
	Z
};

/* BMA180 */
#define BMA180_ID	  3
#define SOFT_RESET_VAL 0xB6

#define OFFSET_X_VAL (0x92C)//e
#define OFFSET_Y_VAL (0x7a6)
#define OFFSET_Z_VAL (0x6d5)//16g = (0x6f6)

/* Регистры BMA180 */
#define CHIP_ID			0x00

#define ACC_X_LSB		0x02
#define ACC_X_MSB		0x03
#define ACC_Y_LSB		0x04
#define ACC_Y_MSB		0x05
#define ACC_Z_LSB		0x06
#define ACC_Z_MSB		0x07

#define CTRL_REG0		0x0D
#define CTRL_REG1		0x0E
#define CTRL_REG2		0x0F
#define CTRL_REG3		0x21
#define CTRL_REG4		0x22


#define RESET			0x10

#define BW_TCS			0x20

#define OFFSET_LSB1		0x35
#define OFFSET_LSB2     0x36

#define OFFSET_X        0x38
#define OFFSET_Y        0x39
#define OFFSET_Z        0x3A

/* Биты регистра ctrl_reg0 */
#define DIS_WAKEUP      0
#define EE_W			4
#define ST0				2
#define RESET_INT		6

/* Биты регистра ctrl_reg1 */
#define EN_OFFSET_X		7
#define EN_OFFSET_Y		6
#define EN_OFFSET_Z		5


/* Биты регистра bw_tcs */
#define TCS0			0
#define TCS1			1
#define TCS2			2
#define TCS3			3

#define BW0				4
#define BW1				5
#define BW2				6
#define BW3				7		

/* Биты range регистра offset_lsb1 */
#define RANGE0			1 
#define RANGE1			2
#define RANGE2			3

/* Биты регистра ctrl_reg3 */
#define NEW_DATA_INT	1

/* Режимы автокалибровки offset_finetuning */
#define FINE			1
#define COARSE			2
#define FULL			3

/* Диапазон измерений акселерометра (ДИА) */
enum bma180_acceleration_range {
	BMA180_1G,
	BMA180_1P5G,
	BMA180_2G,
	BMA180_3G,
	BMA180_4G,
	BMA180_8G,
	BMA180_16G
};

/* Полоса пропускания (пропускная способность) фильтра (ППФ) */
enum bma180_bandwidth {
	BMA180_10HZ,
	BMA180_20HZ,
	BMA180_40HZ,
	BMA180_75HZ,
	BMA180_150HZ,
	BMA180_300HZ,
	BMA180_600HZ, 
	BMA180_1200HZ,
	BMA180_HIGH_PASS,
	BMA180_BAND_PASS
};


uint8_t  init_bma180(enum bma180_acceleration_range range, enum bma180_bandwidth bandwidth);
uint8_t read_from_bma180(uint8_t address);
void write_to_bma180(uint8_t address, char data);
void write_to_bma180_masked(uint8_t address, uint8_t data, uint8_t mask);


void calibrate(uint16_t offset_x, uint16_t offset_y, uint16_t offset_z);
void enable_img_reg_write(void);
void disable_img_reg_write(void);
void calibrate_axis(enum axis);

#endif /* BMA180_CONFIG_H_ */