#ifndef TIMER_H_
#define TIMER_H_
#include "error_types.h"
#include <stdio.h>
#define F_CPU 16000000UL  // 16 МГц

/*
 * С помощью таймера определяем количество выборок в секунду
 */
uint8_t  init_timer(uint8_t samples_per_sec);



#endif /* TIMER_H_ */