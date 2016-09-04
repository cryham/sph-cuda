#include "radixsort.cuh"
#include "radixsort_kernel.cu"


extern "C"
{

////////////////////////////////////////////////////////////////////////////////
//! Perform a radix sort
//! Sorting performed in place on passed arrays.
//!
//! @param pData0	   input and output array - data will be sorted
//! @param pData1	   additional array to allow ping pong computation
//! @param elements	 number of elements to sort
////////////////////////////////////////////////////////////////////////////////
void RadixSort(KeyValuePair *pData0, KeyValuePair *pData1, uint elements, uint bits)
{
	// Round element count to total number of threads for efficiency
	uint elements_rounded_to_3072;
	int modval = elements % 3072;
	if( modval == 0 )
		elements_rounded_to_3072 = elements;
	else
		elements_rounded_to_3072 = elements + (3072 - (modval));

	// Iterate over n bytes of y bit word, using each byte to sort the list in turn
	for (uint shift = 0; shift < bits; shift += RADIX)
	{
		// Perform one round of radix sorting

		// Generate per radix group sums radix counts across a radix group
		RadixSum<<<NUM_BLOCKS, NUM_THREADS_PER_BLOCK, GRFSIZE>>>(pData0, elements, elements_rounded_to_3072, shift);
		// Prefix sum in radix groups, and then between groups throughout a block
		RadixPrefixSum<<<PREFIX_NUM_BLOCKS, PREFIX_NUM_THREADS_PER_BLOCK, PREFIX_GRFSIZE>>>();
		// Sum the block offsets and then shuffle data into bins
		RadixAddOffsetsAndShuffle<<<NUM_BLOCKS, NUM_THREADS_PER_BLOCK, SHUFFLE_GRFSIZE>>>(pData0, pData1, elements, elements_rounded_to_3072, shift); 

		// Exchange data pointers
		KeyValuePair* pTemp = pData0;
		pData0 = pData1;
		pData1 = pTemp;
   }
}

}
