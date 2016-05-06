#include <string.h>
#include "rf_config.h"
#include "rf_tx.h"
#include "assertion.h"

#define SAMPLE_FLOAT_SIZE  3

uint8_t  rf_transmit(float *sample)
{
	RT_ASSERT(sample != NULL, ERROR_TYPE_WRONG_PARAM);
	
	uint8_t sample_bytes = sizeof(float)*SAMPLE_FLOAT_SIZE;
		
	/* ������ ���� ������ - ��� ����� = payload + 2 ����� ����������� ����� */
	TRXFBST = sample_bytes + 2; 
	
	void *byte_ptr = (void *)&sample[0];
	memcpy((void *)(&TRXFBST + 1), byte_ptr, sample_bytes);
		
	/* ���������� ������ */
	//uint8_t dump[128];	
	//uint8_t *p = &TRXFBST;
	//for(uint8_t i = 0; i < 128; i++) {
	//	dump[i] = *p++;
	//}
	
	/* �������� ���������  */
	//float accs[3];
	//byte_ptr = (&TRXFBST + 1);
	//float *float_ptr = byte_ptr;
	//accs[0] = (*float_ptr);
	//accs[1] = (*(++float_ptr));
	//accs[2] = (*(++float_ptr));
	
	/* ������ �������� �� ������������ ���� SLPTR �� 1 � 0 */
	TRXPR |=  (1 << SLPTR);   
	TRXPR &= ~(1 << SLPTR);   
	  
	return ERROR_TYPE_OK;
}

