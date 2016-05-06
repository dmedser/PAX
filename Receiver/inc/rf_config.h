#ifndef __RF_CONFIG_H__
#define  __RF_CONFIG_H__

#include "error_types.h"
#include "stdbool.h"

/**
 * \ingroup config
 * \defgroup rf_config Конфигурация радио-канала
 */

/**
 * \ingroup rf_config
 * Режим работы: приемник или передатчик
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
 * Номер канала
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
	RF_CHANNEL_26,
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
 * Структура настройки радио-канала
 * \struct rf_init
 * \var  rf_init::enable_crc
 * Включения автоматической проверки и подсчета CRC
 * \var  rf_init::channel_no
 * Номер канала
 */
struct rf_init {
	bool enable_crc;
	enum rf_channel channel_no;
};

/**
 * \ingroup rf_config
 * Инициализация радио-канала
 * \param [in] init_struct Структура настроек
 * \return Код ошибки
 */
enum error_type init_rf(const struct rf_init* init_struct);

enum error_type enable_rf(enum rf_role role);


#endif

