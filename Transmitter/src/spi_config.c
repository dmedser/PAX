#include <avr/io.h>
#include <util/delay.h>
#include "stdbool.h"
#include "avr/interrupt.h"
#include "spi_config.h"
#include "stddef.h"
#include "assertion.h"

void set_spi_transfer_status(enum spi_transfer_status status)
{
	(status == ON) ? (PORTB &= ~(1 << CSB)) : (PORTB |= (1 << CSB));
}


/* ѕередача данных от Master'a к Slave'у */
inline void spi_transmit(char data)
{
	SPDR = data;
	while (!(SPSR & (1 << SPIF)));
}


/* ѕолучение Master'ом ответа от Slave'а */
inline char spi_receive(void)
{
	/* 
	 * Ёкспериментально установлено, что SPI тактируетс€ 
	 * только в при передаче данных, но не во врем€ приема.
	 * ƒл€ корректной работы сначала записываем в регистр 
	 * данных SPDR произвольное значение, затем выполн€етс€  
	 * обычна€ процедура приема
	 */
	SPDR = 0x55;
	while (!(SPSR & (1 << SPIF)));
	return SPDR;
}


uint8_t  init_spi(struct spi_init_struct *spi_init_struct) 
{
	RT_ASSERT(spi_init_struct != NULL, ERROR_TYPE_NULL_PTR);
	RT_ASSERT(assertCorrectSpiMode(spi_init_struct->mode), ERROR_TYPE_WRONG_PARAM);
	RT_ASSERT(assertCorrectSpiDataOrder(spi_init_struct->data_order), ERROR_TYPE_WRONG_PARAM);
	RT_ASSERT(assertCorrectSpiClockPolarity(spi_init_struct->clock_polarity), ERROR_TYPE_WRONG_PARAM);
	RT_ASSERT(assertCorrectSpiClockPhase(spi_init_struct->clock_phase), ERROR_TYPE_WRONG_PARAM);
	RT_ASSERT(assertCorrectSpiClockRate(spi_init_struct->clock_rate), ERROR_TYPE_WRONG_PARAM);
	RT_ASSERT(assertCorrectSpiDoubleSpeed(spi_init_struct->double_speed), ERROR_TYPE_WRONG_PARAM);
		
	SPCR = 0;
	SPSR = 0;

	SPCR |= (spi_init_struct->clock_rate      << SPR0)|
			(spi_init_struct->clock_phase	  << CPHA)|
			(spi_init_struct->clock_polarity  << CPOL)|
			(spi_init_struct->mode			  << MSTR)|
			(spi_init_struct->data_order	  << DORD)|
			(1 << SPE);
	SPSR |= (spi_init_struct->double_speed	  << SPI2X);
	
	/* Ќастройка DDR дл€ SPI */
	DDR_SPI = (1 << MOSI) | (1 << SCK) | (1 << CSB);
	/* Ќачальные уровни на SCK и CSB должны быть 1 */
	PINB = (1 << SCK) | (1 << CSB);
	
	return ERROR_TYPE_OK;
}

