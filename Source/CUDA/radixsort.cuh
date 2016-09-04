/* Radixsort project which demonstrates the use of CUDA in a multi phase
 * sorting computation.
 * Type definitions. */

#ifndef _RADIXSORT_H_
#define _RADIXSORT_H_


#include <host_defines.h>

#define SYNCIT __syncthreads()

// Use 16 bit keys/values
#define SIXTEEN 0

typedef unsigned int uint;
typedef unsigned short ushort;

#if SIXTEEN
typedef struct __align__(4) {
	ushort key;
	ushort value;
#else
typedef struct __align__(8) {
	uint key;
	uint value;
#endif
} KeyValuePair;

extern "C" {
	void RadixSort(KeyValuePair *pData0, KeyValuePair *pData1, uint elements, uint bits);
}


#endif // #ifndef _RADIXSORT_H_
