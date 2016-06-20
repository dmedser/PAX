#include <avr/io.h> 
#include "error_types.h"
#ifndef RF_TX_H_
#define RF_TX_H_

uint8_t rf_transmit(float *sample);

#endif /* RF_TX_H_ */