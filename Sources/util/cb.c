/* Circular buffer
 * 
 * Grupo 2 - 2012
*/

#include "../common.h"
#include "cb.h"


cbuf cb_create(u8 *mem, s8 len)
{
	cbuf r;
	
	r.mem = mem;
	r.w = r.r = r.mem;
	r.len = len;
	r.status = CB_EMPTY;
	
	return r;
}

s16 cb_push(cbuf* buffer, u8 data) 
{
	if (buffer->status == CB_FULL)
		return -E_NOMEM;
	
	*buffer->w++ = data;
	
	if ((buffer->w - buffer->mem) >= buffer->len)
		buffer->w = buffer->mem;
	
	if (buffer->w == buffer->r)
		buffer->status = CB_FULL;
	else
		buffer->status = CB_READY;
	
	return -E_OK;
}

s16 cb_pop(cbuf* buffer) 
{
	u8 readVal;
	
	if (buffer->status == CB_EMPTY)
		return -E_NOTREADY;
	
	readVal = *(buffer->r++);
	
	if ((buffer->r - buffer->mem) >= buffer->len)
		buffer->r = buffer->mem;
	
	if (buffer->r == buffer->w)
		buffer->status = CB_EMPTY;
	else
		buffer->status = CB_READY;
	
	return readVal;
}


s16 cb_flush(cbuf* buffer) 
{
	s16 erasedSize;
	
	if (buffer->status == CB_READY) {
		if(buffer->w > buffer->r){
			erasedSize = buffer->w - buffer->r;
		} else if (buffer->w < buffer->r) {
			erasedSize = buffer->len - (buffer->r - buffer->w);
		} else {
			return -E_OTHER;
		}
	} else if (buffer->status == CB_FULL) {
		erasedSize = buffer->len;
	} else {
		erasedSize = 0;
	}
	
	buffer->r = buffer->w = buffer->mem;
	buffer->status = CB_EMPTY;
	return erasedSize;
}
