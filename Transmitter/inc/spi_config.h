#ifndef SPI_CONFIG_H_
#define SPI_CONFIG_H_

#include "error_types.h"

#define DDR_SPI		DDRB
#define SCK			1
#define MOSI		2
#define MISO		3
#define CSB			0

enum spi_mode {
	SPI_SLAVE_MODE,
	SPI_MASTER_MODE
};

enum spi_data_order {
	SPI_MSB_FIRST,
	SPI_LSB_FIRST
};

enum spi_clock_polarity {
	SPI_RISING_EDGE_START,
	SPI_FALLING_EDGE_START
};

enum spi_clock_phase {
	SPI_RISING_EDGE_SAMPLE,
	SPI_FALLING_EDGE_SAMPLE
};

enum spi_clock_rate {
	SPI_CLK_DIV_4,
	SPI_CLK_DIV_16,
	SPI_CLK_DIV_64,
	SPI_CLK_DIV_128
};

enum spi_double_speed {
	SPI_DOUBLE_SPEED_OFF,
	SPI_DOUBLE_SPEED_ON
};

#define assertCorrectSpiMode(x) ( \
	((x) == SPI_SLAVE_MODE)  || \
	((x) == SPI_MASTER_MODE) \
)

#define assertCorrectSpiDataOrder(x) ( \
	((x) == SPI_MSB_FIRST) || \
	((x) == SPI_LSB_FIRST) \
)
	 
#define assertCorrectSpiClockPolarity(x) ( \
	((x) == SPI_RISING_EDGE_START)  || \
	((x) == SPI_FALLING_EDGE_START) \
)

#define assertCorrectSpiClockPhase(x) ( \
	((x) == SPI_RISING_EDGE_SAMPLE)  || \
	((x) == SPI_FALLING_EDGE_SAMPLE) \
)

#define assertCorrectSpiClockRate(x) ( \
	((x) == SPI_CLK_DIV_4)   || \
	((x) == SPI_CLK_DIV_16)  || \
	((x) == SPI_CLK_DIV_64)  || \
	((x) == SPI_CLK_DIV_128) \
)

#define assertCorrectSpiDoubleSpeed(x) ( \
	((x) == SPI_DOUBLE_SPEED_OFF)  || \
	((x) == SPI_DOUBLE_SPEED_ON)   \
)

struct spi_init_struct {
	enum spi_data_order			data_order;
	enum spi_mode				mode;
	enum spi_clock_polarity		clock_polarity;
	enum spi_clock_phase		clock_phase;
	enum spi_clock_rate			clock_rate;
	enum spi_double_speed		double_speed;
};

enum spi_transfer_status {
	ON,
	OFF
};


uint8_t   init_spi(struct spi_init_struct *spi_init_struct);
void set_spi_transfer_status(enum spi_transfer_status status);
void spi_transmit(char data);
char spi_receive(void);

#endif /* SPI_CONFIG_H_ */