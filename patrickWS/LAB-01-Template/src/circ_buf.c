/*
 * circ_buf.c
 *
 *  Created on: Sep 3, 2019
 *      Author: lovep
 */

#include "circ_buf.h"
#include <string.h>

circ_buf_t CircBufMake(void* buffer, uint32_t size, uint32_t span)
{
	circ_buf_t ret;
	ret.buffer = buffer;
	ret.size = size;
	ret.span = span;
	ret.head = 0;
	return ret;
}
void* CircBufGet(circ_buf_t* buf, uint32_t index)
{
	uint32_t offset = (buf->head+index)%buf->size;
	return buf->buffer + offset*buf->span;
}
void* CircBufLatest(circ_buf_t* buf)
{
	return CircBufGet(buf, buf->size-1);
}
void* CircBufOldest(circ_buf_t* buf)
{
	return CircBufGet(buf, 0);
}
void CircBufAdd(circ_buf_t* buf, void* val)
{
	memcpy(buf->buffer + (buf->head*buf->span), val, buf->span);
	buf->head = (buf->head+1)%buf->size;
}
