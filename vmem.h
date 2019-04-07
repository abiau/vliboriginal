#ifndef __VVV_MEM_H__
#define __VVV_MEM_H__

#include "vutil.h"
#include "vhash.h"

#ifdef __cplusplus

extern "C" {
#endif

typedef  int  (*filter_ft)(void* value, void* item);
typedef  u32t (*hashcode_ft)(size_t nMaxBucket, char* key);
typedef  int  (*match_ft)(void* key, void* item);
typedef  int (*dtor_ft)(void* arg);
typedef  int  (*todo_ft)(void* arg);

typedef struct {
	char buf[4096];
} VMemSlot;

typedef struct {
	/* callback_function. */

	/* data. */
	vmutex_t   mutex; 
	int        size;
} VMemPool;

VMemSlot* vmemslot_create();
void vmemslot_destroy();
VMemPool* vmempool_create ();
void vmempool_destroy (VMemPool* pM);
void vmempool_lock (VMemPool* pHT);
void vmempool_unlock (VMemPool* pHT);
void* vmempool_getmem (VMemPool* pHT);


#ifdef __cplusplus
}
#endif

#endif //__VVV_MEM_H__

