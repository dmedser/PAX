#include <util/delay.h>
#include <avr/io.h>
#include "stdbool.h"
#include "bma180_config.h"
#include "timer.h"
#include "rf_config.h"
#include "spi_config.h"


void check_error(uint8_t error)
{
	volatile uint8_t error_type = error;
	if (error_type == ERROR_TYPE_OK)
		return;
	while(true);
}

/*
 * ѕередатчик посылает значени€ ускорений rf_transfers_per_sec раз в секунду
 */

int main (void)
{
	volatile uint8_t ret_val;
	volatile struct rf_init rf_init;
	uint8_t rf_transfers_per_sec = 1;
	
	/* √лобальное разрешение прерываний */
	SREG |= (1 << SREG_I);
	
	/* „астота SCK = (16ћ√ц/4) * 2 = 8ћ√ц */
	struct spi_init_struct spi_init = {0};
	spi_init.data_order		= SPI_MSB_FIRST;
	spi_init.mode			= SPI_MASTER_MODE;
	spi_init.clock_polarity	= SPI_FALLING_EDGE_START;
	spi_init.clock_phase	= SPI_FALLING_EDGE_SAMPLE;
	spi_init.clock_rate		= SPI_CLK_DIV_16;
	spi_init.double_speed	= SPI_DOUBLE_SPEED_ON;
	
	ret_val = init_spi(&spi_init);
	check_error(ret_val);
	
	ret_val = init_bma180(BMA180_2G, BMA180_1200HZ);
	check_error(ret_val);
		

	ret_val = init_timer(rf_transfers_per_sec);
	check_error(ret_val);

	rf_init.enable_crc = true;
	rf_init.channel_no = RF_CHANNEL_11;
	
	ret_val = init_rf(&rf_init);
	check_error(ret_val);
	
	ret_val = enable_rf(TRANSMITTER);
	check_error(ret_val);
		
	
	while(true);
}
