#ifndef RF_CONFIG_H_
#define RF_CONFIG_H_


#ifndef __RF_CONFIG_H__
#define  __RF_CONFIG_H__

#include "error_types.h"
#include "stdbool.h"
#include "stdint.h"
/**
 * \ingroup config
 * \defgroup rf_config ������������ �����-������
 */

/**
 * \ingroup rf_config
 * ����� ������: �������� ��� ����������
 */

enum rf_role {
	TRANSMITTER,
	RECEIVER
};

#define is_rf_role(x) (\
		       (x == TRANSMITTER) || \
		       (x == RECEIVER)\
		       )
/**
 * \ingroup rf_config
 * ����� ������
 */
enum rf_channel {
	RF_CHANNEL_11,
	RF_CHANNEL_12,
	RF_CHANNEL_13,
	RF_CHANNEL_14,
	RF_CHANNEL_15,
	RF_CHANNEL_16,
	RF_CHANNEL_17,
	RF_CHANNEL_18,
	RF_CHANNEL_19,
	RF_CHANNEL_20,
	RF_CHANNEL_21,
	RF_CHANNEL_22,
	RF_CHANNEL_23,
	RF_CHANNEL_24,
	RF_CHANNEL_25,
	RF_CHANNEL_26
};

#define is_rf_channel(x) (\
			  (x == RF_CHANNEL_11) || \
			  (x == RF_CHANNEL_12) || \
			  (x == RF_CHANNEL_13) || \
			  (x == RF_CHANNEL_14) || \
			  (x == RF_CHANNEL_15) || \
			  (x == RF_CHANNEL_16) || \
			  (x == RF_CHANNEL_17) || \
			  (x == RF_CHANNEL_18) || \
			  (x == RF_CHANNEL_19) || \
			  (x == RF_CHANNEL_20) || \
			  (x == RF_CHANNEL_21) || \
			  (x == RF_CHANNEL_22) || \
			  (x == RF_CHANNEL_23) || \
			  (x == RF_CHANNEL_24) || \
			  (x == RF_CHANNEL_25) || \
			  (x == RF_CHANNEL_26)    \
			 )


/**
 * \ingroup rt_config
 * ��������� ��������� �����-������
 * \struct rf_init
 * \var  rf_init::enable_crc
 * ��������� �������������� �������� � �������� CRC
 * \var  rf_init::channel_no
 * ����� ������
 */
struct rf_init {
	bool enable_crc;
	enum rf_channel channel_no;
};

/**
 * \ingroup rf_config
 * ������������� �����-������
 * \param [in] init_struct ��������� ��������
 * \return ��� ������
 */
uint8_t init_rf(struct rf_init* init_struct);

uint8_t enable_rf(enum rf_role role);

#endif




#endif /* RF_CONFIG_H_ */