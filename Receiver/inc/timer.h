#ifndef TIMER_H_
#define TIMER_H_
#include <stdio.h>
#include "error_types.h"
#define F_CPU 16000000UL  // 16 ���

/*
 * � ������� ������� ���������� ���������� ����������� �� ���������� ������ ������� � �������
 */
enum error_type init_timer(uint8_t samples_per_sec);


#endif /* TIMER_H_ */