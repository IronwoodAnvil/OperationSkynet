/*
 * circ_buf.h
 *
 *  Created on: Sep 3, 2019
 *      Author: lovep
 */

#ifndef INC_CIRC_BUF_H_
#define INC_CIRC_BUF_H_

#include <stdint.h>

typedef struct {
	uint8_t* buffer;
	uint32_t size;
	uint32_t span;
	uint32_t head;
} circ_buf_t;

circ_buf_t CircBufMake(void* buffer, uint32_t size, uint32_t span);
void* CircBufGet(circ_buf_t* buf, uint32_t index);
void CircBufAdd(circ_buf_t* buf, void* val);
void* CircBufLatest(circ_buf_t* buf);
void* CircBufOldest(circ_buf_t* buf);


#endif /* INC_CIRC_BUF_H_ */
