#include "ring_buff.h"
#include "assertion.h"
#include <string.h>

struct circ_buff {
	uint8_t buff[BUFFER_SIZE][MAX_FRAME_SIZE];		/* Хранение 10-ти выборок */
	uint8_t head;									/* Самый "старый" элемент */
	uint8_t tail;									/* Самый "новый" */
};

static struct circ_buff _circ_buff = {{{0}}};

volatile uint8_t* get_elem(void)
{
	volatile uint8_t* ret_elem = &_circ_buff.buff[_circ_buff.head];
	_circ_buff.head = (_circ_buff.head + 1) % BUFFER_SIZE;	/* head = 0, 1,..., 9, 0, 1,..., 9, 0, 1,... */
	return ret_elem;
}

enum error_type put_elem(uint8_t* elem, uint8_t size)
{
	RT_ASSERT(size <= MAX_FRAME_SIZE, ERROR_TYPE_WRONG_PARAM);
	uint8_t* buff_tail_ptr = _circ_buff.buff[_circ_buff.tail];
	_circ_buff.tail = (_circ_buff.tail + 1) % BUFFER_SIZE;
	memcpy(buff_tail_ptr, elem, size);
	return ERROR_TYPE_OK;
}

enum error_type clear(void) 
{
	_circ_buff.head = _circ_buff.tail = 0;
	return ERROR_TYPE_OK;
}
