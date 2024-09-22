#ifndef MEMORY_H
#define	MEMORY_H

#include "types.h"

byte * SRAMalloc(byte nBytes);
void SRAMfree(byte * pSRAM);
void SRAMInitHeap(void);


#endif	/* MEMORY_H */

