#include "rf_config.h"
#include "stddef.h"
#include "assertion.h"
#include "avr/io.h"
#include "avr/interrupt.h"

uint8_t  init_rf(struct rf_init* init_struct)
{
//	RT_ASSERT(init_struct != NULL, ERROR_TYPE_NULL_PTR);
	RT_ASSERT(is_rf_channel(init_struct->channel_no), ERROR_TYPE_WRONG_PARAM);
	/* Сброс состояния в SLEEP */
	TRXPR |= (1 << TRXRST);

	while ((TRX_STATUS & 0x1F) == STATE_TRANSITION_IN_PROGRESS) {
		/* EMPTY */
	}

	IRQ_MASK = 0;

	/* Установка состояния в TRX_OFF */
	TRX_STATE = (TRX_STATE & 0xE0) | TRX_OFF;

	while ((TRX_STATUS & 0x1F) == STATE_TRANSITION_IN_PROGRESS) {
		/* EMPTY */
	}
	if (init_struct->enable_crc) {
		/* Автоподсчет CRC */
		TRX_CTRL_1 |= (1<<TX_AUTO_CRC_ON);
	}
	volatile uint16_t channel_no = 0;
	switch (init_struct->channel_no) {
		case RF_CHANNEL_11 : channel_no = 11; break;
		case RF_CHANNEL_12 : channel_no = 12; break;
		case RF_CHANNEL_13 : channel_no = 13; break;
		case RF_CHANNEL_14 : channel_no = 14; break;
		case RF_CHANNEL_15 : channel_no = 15; break;
		case RF_CHANNEL_16 : channel_no = 16; break;
		case RF_CHANNEL_17 : channel_no = 17; break;
		case RF_CHANNEL_18 : channel_no = 18; break;
		case RF_CHANNEL_19 : channel_no = 19; break;
		case RF_CHANNEL_20 : channel_no = 20; break;
		case RF_CHANNEL_21 : channel_no = 21; break;
		case RF_CHANNEL_22 : channel_no = 22; break;
		case RF_CHANNEL_23 : channel_no = 23; break;
		case RF_CHANNEL_24 : channel_no = 24; break;
		case RF_CHANNEL_25 : channel_no = 25; break;
		case RF_CHANNEL_26 : channel_no = 26; break;
		default: return ERROR_TYPE_WRONG_PARAM;
	}
	/* Установка канала */
	PHY_CC_CCA |= (channel_no & 0x1F);
	//channel_no  = 0;
	return ERROR_TYPE_OK;
}

uint8_t  enable_rf(enum rf_role role)
{
	RT_ASSERT(is_rf_role(role), ERROR_TYPE_WRONG_PARAM);
	RT_ASSERT((TRX_STATE & 0x1F) == TRX_OFF, ERROR_TYPE_ILLEGAL_STATE);
	
	switch (role) {
	case RECEIVER: {
			IRQ_MASK  = (1 << RX_END_EN);
			TRX_STATE = (TRX_STATE & 0xE0) | RX_ON;
			while ((TRX_STATUS & 0x1F) == STATE_TRANSITION_IN_PROGRESS);
			break;
		}
	case TRANSMITTER: {
			TRX_STATE = (TRX_STATE & 0xE0) | PLL_ON;
			while ((TRX_STATUS & 0x1F) == STATE_TRANSITION_IN_PROGRESS);
			break;
		}
	}
	return ERROR_TYPE_OK;
}

