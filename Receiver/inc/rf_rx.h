#include "avr/interrupt.h"

#ifndef RF_RX_H_
#define RF_RX_H_

/*
 * Обработчик прерывания по окончанию передачи фрейма по радиоканалу
 */
ISR(TRX24_RX_END_vect);


#endif /* RF_RX_H_ */