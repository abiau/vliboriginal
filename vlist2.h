#ifndef __VVV_LIST2_H__
#define __VVV_LIST2_H__

#include "vutil.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct VNode2{
	u64t    id;
	void*   arg;
	union {
		struct  VNode* F;
	}
	union {
		struct  VNode* L;
		struct  VNode* prev;
	}
	union {
		struct  VNode* R;
		struct  VNode* next;
	}
} VNode2;

typedef struct {
	/* callback_function. */

	/* data. */
	vmutex_t  mutex; 
	int       nMaxBucket;
	VBucket*  Bucket;
	int       iCurBucket;
	VSlot*    pCurSlot;
} VList2;


#ifdef __cplusplus
}
#endif

#endif //__VVV_LIST2_H__

