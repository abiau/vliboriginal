#include <stdio.h>
#include "vutil.h"
#include "vhash.h"
#include "vmem.h"

//static VHashTable* pMemHT;

VMemSlot* vmemslot_create()
{
	return NULL;
}
void vmemslot_destroy()
{
}

/***********************************************************/
/***********************************************************/

VMemPool* vmempool_create ()
{
	//pMemHT = vhashtable_create ();
	return NULL;
}

void vmempool_destroy (VMemPool* pM)
{
	return ;
}
void vmempool_lock (VMemPool* pHT)
{
	return ;
}
void vmempool_unlock (VMemPool* pHT)
{
	return ;
}
void* vmempool_getmem (VMemPool* pHT)
{
	return NULL;
}


