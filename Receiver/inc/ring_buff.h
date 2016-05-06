#ifndef __RING_BUFF_H__
#define __RING_BUFF_H__

#include "error_types.h"
#include "stdint.h"
#include "stddef.h"

#define BUFFER_SIZE		(10)
#define MAX_FRAME_SIZE (128)

struct circ_buff;

volatile uint8_t* get_elem(void);

enum error_type put_elem(uint8_t* elem, uint8_t size);

enum error_type clear(void);

#endif

