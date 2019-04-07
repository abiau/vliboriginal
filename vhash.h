#ifndef __VVV_HASH_H__
#define __VVV_HASH_H__

#include "vutil.h"

#ifdef __cplusplus

extern "C" {
#endif

typedef  int (*filter_ft)(void* Bound, void* item);
typedef  u32t (*hashcode_ft)(size_t nMaxBucket, char* key);
typedef  int (*match_ft)(void* key, void* item);
typedef  int (*dtor_ft)(void* arg);
typedef  int (*todo_ft)(void* arg);

typedef struct VSlot{
	void*         arg;
	char          key[128];
	struct VSlot* prev;
	struct VSlot* next;
} VSlot;

typedef struct {
	int    size;
	VSlot* head;
	VSlot* tail;
} VBucket;

typedef struct {
	/* callback_function. */
	u32t      (*hashcode)(size_t nMaxBucket, char* key);
	int       (*match)(void* key, void* arg);
	int       (*dtor)(void* arg);

	/* data. */
	vmutex_t  mutex; 
	int       nMaxBucket;
	VBucket*  Bucket;
	int       iCurBucket;
	VSlot*    pCurSlot;
} VHashTable;

VHashTable* vhashtable_create (int nMaxBucket, dtor_ft dtor);
void vhashtable_destroy (VHashTable* pHT);
void vhashtable_traversal (VHashTable* pHT, todo_ft todo);
void vhashtable_lock (VHashTable* pHT);
void vhashtable_unlock (VHashTable* pHT);
int vhashtable_getsize (VHashTable* pHT);
int vhashtable_insert (VHashTable* pHT, char* key, void* arg);
int vhashtable_delete (VHashTable* pHT, char* key);
void* vhashtable_find (VHashTable* pHT, char* key);
void vhashtable_filterANDdelete (VHashTable* pHT, filter_ft filter, void* Bound);


#ifdef __cplusplus
}
#endif

#endif //__VVV_HASH_H__

