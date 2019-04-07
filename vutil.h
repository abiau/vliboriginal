#ifndef __VVV_UTIL_H__
#define __VVV_UTIL_H__


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <execinfo.h> 
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h> 
#include <fcntl.h> 
#include <libgen.h>
#include <assert.h>
#include <stdarg.h>
#include <sys/time.h>
#include <ctype.h>
#include <limits.h> 


#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long long  u64t;
typedef          long long  n64t;
typedef unsigned int        u32t;
typedef          int        n32t;
typedef pthread_mutex_t     vmutex_t;
typedef pthread_t           vthread_t;

typedef void (*void_fn)();
typedef  int (*i_fn)();
typedef  int (*comp_ft)(void* item, void* arg);
typedef  int (*dtor_ft)(void* arg);
typedef  int (*todo_ft)(void* arg);

/* Error Code Definition. */
#define OKAY        (   0)
#define FAIL        (  -1)
#define TIMEOUT     (  -2)

#define LEN32       (  32) 
#define LEN64       (  64) 
#define LEN128      ( 128) 
#define LEN256      ( 256) 
#define LEN512      ( 512) 
#define LEN1024     (1024) 
#define LEN2048     (2048) 
#define LEN4096     (4096) 
#define LEN8192     (8192) 

/* Macro Definition. */
//#define max(a,b)          do{ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; }while(0)
//#define min(a,b)          do{ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a < _b ? _a : _b; }while(0)
#define vzero(addr,size)  do{memset(addr, 0, size);}while(0)
#define VACCERT(x)        ;if((x)==0){exit(1);}


/* Color. */

#define CCr      "\033[0;31m"
#define CCg      "\033[0;32m"
#define CCy      "\033[0;33m" 
#define CCb      "\033[0;34m"
#define CCm      "\033[0;35m"
#define CCc      "\033[0;36m"
#define CCw      "\033[0;37m"

#define CCR      "\033[1;31m"
#define CCG      "\033[1;32m"
#define CCY      "\033[1;33m"
#define CCB      "\033[1;34m"
#define CCM      "\033[1;35m"
#define CCC      "\033[1;36m"
#define CCW      "\033[1;37m"

#define BBr      "\033[41m"
#define BBg      "\033[42m"
#define BBy      "\033[43m"
#define BBb      "\033[44m"
#define BBm      "\033[45m"
#define BBc      "\033[46m"
#define BBw      "\033[47m"

#define CCe      "\033[m"
#define DDLINE   "@@@ %s(),%d"

#define EEE      do{printf(CCe);}while(0)
#define DDD      do{printf(     DDLINE       "\n", __func__, __LINE__);}while(0)
#define DDr      do{printf(CCr  DDLINE  CCe  "\n", __func__, __LINE__);}while(0)
#define DDg      do{printf(CCg  DDLINE  CCe  "\n", __func__, __LINE__);}while(0)
#define DDy      do{printf(CCy  DDLINE  CCe  "\n", __func__, __LINE__);}while(0)
#define DDb      do{printf(CCb  DDLINE  CCe  "\n", __func__, __LINE__);}while(0)
#define DDm      do{printf(CCm  DDLINE  CCe  "\n", __func__, __LINE__);}while(0)
#define DDc      do{printf(CCc  DDLINE  CCe  "\n", __func__, __LINE__);}while(0)
#define DDw      do{printf(CCw  DDLINE  CCe  "\n", __func__, __LINE__);}while(0)

#define DDR      do{printf(CCR  DDLINE  CCe  "\n", __func__, __LINE__);}while(0)
#define DDG      do{printf(CCG  DDLINE  CCe  "\n", __func__, __LINE__);}while(0)
#define DDY      do{printf(CCY  DDLINE  CCe  "\n", __func__, __LINE__);}while(0)
#define DDB      do{printf(CCB  DDLINE  CCe  "\n", __func__, __LINE__);}while(0)
#define DDM      do{printf(CCM  DDLINE  CCe  "\n", __func__, __LINE__);}while(0)
#define DDC      do{printf(CCC  DDLINE  CCe  "\n", __func__, __LINE__);}while(0)
#define DDW      do{printf(CCW  DDLINE  CCe  "\n", __func__, __LINE__);}while(0)

#define EEr      do{printf(CCr);}while(0)  //red
#define EEg      do{printf(CCg);}while(0)  //green
#define EEy      do{printf(CCy);}while(0)  //yellow
#define EEb      do{printf(CCb);}while(0)  //blue
#define EEm      do{printf(CCm);}while(0)  //purple
#define EEc      do{printf(CCc);}while(0)  //green-blue
#define EEw      do{printf(CCw);}while(0)  //white

#define EER      do{printf(CCR);}while(0)
#define EEG      do{printf(CCG);}while(0)
#define EEY      do{printf(CCY);}while(0)
#define EEB      do{printf(CCB);}while(0)
#define EEM      do{printf(CCM);}while(0)
#define EEC      do{printf(CCC);}while(0)
#define EEW      do{printf(CCW);}while(0)

/***************************************************************************/
/***************************************************************************/



typedef struct VNode{
	u64t    id;
	void*   arg;
	struct  VNode* F;
	struct  VNode* L;
	struct  VNode* R;
	struct  VNode* prev;
	struct  VNode* next;
} VNode;

typedef enum {
	VStatus_Init    =   0,

	VStatus_Create  = 101, /*  Create <--> Destroy                 */
	VStatus_Start   = 102, /*   Start <-->    Stop (<--> Restart)  */
	VStatus_Resume  = 103, /*  Resume <-->   Pause                 */
	VStatus_Running = 104,
	VStatus_Pause   = 105, /*  Resume <-->   Pause                 */
	VStatus_Stop    = 106, /*   Start <-->    Stop (<--> Restart)  */
	VStatus_Restart = 107, /*   Start <-->    Stop (<--> Restart)  */
	VStatus_Destroy = 108, /*  Create <--> Destroy                 */

	VStatus_GET     = 201, /*  Retrieve                            */
	VStatus_POST    = 202, /*  Create                              */
	VStatus_PUT     = 203, /*  Update                              */
	VStatus_DELETE  = 204, /*  Remove                              */
	
	VStatus_WAITING = 301, /*                                      */
	VStatus_DONE    = 302, /*                                      */
} VStatus_e;

const char* VStatusStr (int s);


typedef enum {
	PATH = 1,
	SIZE = 2,
	DAY  = 3,
} VlogOption_e;

typedef struct {
	u64t ts;
	int  Y;
	int  M;
	int  D;
	int  h;
	int  m;
	int  s;
} DateTime;

typedef enum {
	FIRST = 1,
	LAST  = 2,
} POS_e;

/***************************************************************************/
/***************************************************************************/

#define VLOG_BASE \
	struct {\
		void    (*print)     (void* self, const char* szFunc, int nLine, ...);\
		void    (*set)       (void* self, VlogOption_e type, ...);\
		void    (*setPath)   (void* self, const char* folder, const char* file);\
		void    (*setSize)   (void* self, const char* GMKB);\
		void    (*setDay)    (void* self, const char* UYMWDhms);\
	};

#define VLOCK_BASE \
	struct {\
		void      (*lock)        (void* self);\
		void      (*unlock)      (void* self);\
	};

#define VDATA_BASE \
	struct {\
		void      (*travel)      (void* self, todo_ft todo);\
		VNode*    (*insert)      (void* self, comp_ft less, void* arg);\
		VNode*    (*search)      (void* self, comp_ft equal, void* arg);\
		void      (*Delete)      (void* self, VNode* node, dtor_ft del);\
		void      (*seek)        (void* self);\
		VNode*    (*foreach)     (void* self, comp_ft filter, void* arg);\
		int       (*getSize)     (void* self);\
	}; 

/***************************************************************************/
/***************************************************************************/

typedef struct {
	/* method. */
	struct {
		void          (*lock)           (void* self);\
		void          (*unlock)         (void* self);\
	}; 
	/* data. */
	vmutex_t      mutex; 
} VLock; 

/* VLOG */
typedef struct {
	/* method. */
	struct {
		void          (*lock)           (void* self);
		void          (*unlock)         (void* self);
	};
	struct {
		void    (*print)     (void* self, const char* szFunc, int nLine, ...);
		void    (*set)       (void* self, VlogOption_e type, ...);
		void    (*setPath)   (void* self, const char* folder, const char* file);
		void    (*setSize)   (void* self, const char* GMKB);
		void    (*setDay)    (void* self, const char* UYMWDhms);
	};

	/* data. */
	vmutex_t         mutex; 
	int              mode;
	n64t             MaxFileSize;
	u64t             lastTs;
	n64t             MaxDayMs;
	char             bakFolder[LEN128];
	char             folder   [LEN128];
	char             path     [LEN256];
	char             FmtScreen[LEN64];
	char             FmtFile  [LEN64];
	char             buf      [LEN2048];
} VLog;

/* VTIMER */
typedef struct {
	/* method. */
	void             (*resume)    (void* self);
	u64t             (*diffms)    (void* self);
	u64t             (*diffus)    (void* self);
	u64t             (*now)       (void);
	char*            (*nowStr)    (char* buf, int bufLen, const char* fmt);
	char*            (*tsStr)     (char* buf, int bufLen, const char* fmt, u64t ms);
	/* data. */
	struct timeval   tv;
} VTimer;

typedef struct {
	/* method. */
	void      (*lock)    (void* self);
	void      (*unlock)  (void* self);
	int       (*push)    (void* self, void* arg);
	void*     (*pop)     (void* self);
	int       (*getSize) (void* self);

	/* data. */
	vmutex_t  mutex; 
	int       nMaxSize;
	int       size;
	VNode*    cur;
	VNode*    head; /* bottom. */
	VNode*    tail; /* top. */
} VQueue;

VQueue* vqueue_create  (int nMaxSize);
void    vqueue_destroy (VQueue* pQueue);
void    vqueue_lock    (void* self);
void    vqueue_unlock  (void* self);
int     vqueue_push    (void* self, void* arg);
void*   vqueue_pop     (void* self);
int     vqueue_getSize (void* self);

typedef struct {
	/* method. */
	void      (*lock)    (void* self);
	void      (*unlock)  (void* self);
	int       (*push)    (void* self, void* arg);
	void*     (*pop)     (void* self);
	int       (*getSize) (void* self);

	/* data. */
	vmutex_t  mutex; 
	int       nMaxSize;
	int       size;
	VNode*    cur;
	VNode*    head; /* bottom. */
	VNode*    tail; /* top. */
} VStack;

VStack* vstack_create  (int nMaxSize);
void    vstack_destroy (VStack* pStack);
void    vstack_lock    (void* self);
void    vstack_unlock  (void* self);
int     vstack_push    (void* self, void* arg);
void*   vstack_pop     (void* self);
int     vstack_getSize (void* self);

typedef struct {
	/* method. */
	struct {
		void      (*lock)        (void* self);
		void      (*unlock)      (void* self);
	};
	struct {
		void      (*travel)      (void* self, todo_ft todo);
		VNode*    (*insert)      (void* self, comp_ft less, void* arg);
		VNode*    (*search)      (void* self, comp_ft equal, void* arg);
		void      (*Delete)      (void* self, VNode* node, dtor_ft del);
		void      (*seek)        (void* self);
		VNode*    (*foreach)     (void* self, comp_ft filter, void* arg);
		int       (*getSize)     (void* self);
	}; 

	/* data. */
	VLock*    Lock;
	int       size;
	VNode*    cur;
	VNode*    head;
	VNode*    tail;
} VList;

typedef struct {
	/* method. */
	struct {
		void      (*lock)        (void* self);
		void      (*unlock)      (void* self);
	};
	struct {
		void      (*travel)      (void* self, todo_ft todo);
		VNode*    (*insert)      (void* self, comp_ft less, void* arg);
		VNode*    (*search)      (void* self, comp_ft equal, void* arg);
		void      (*Delete)      (void* self, VNode* node, dtor_ft del);
		void      (*seek)        (void* self);
		VNode*    (*foreach)     (void* self, comp_ft filter, void* arg);
		int       (*getSize)     (void* self);
	}; 

	/* data. */
	VLock*    Lock;
	int       size;
	VNode*    cur;
	VNode*    head;
} VTree;


	

/*************************************************************************************/
/*************************************************************************************/

VLock*    vlock_create ();
void      vlock_destroy (VLock* pLock);
void      vlock_lock (void* self);
void      vlock_unlock (void* self);

/*************************************************************************************/
/*************************************************************************************/

VLog*     vlog_create            (int mode, const char* folder, const char* file, const char* FmtScreen, const char* FmtFile);
void      vlog_destroy           (VLog* pLog);
void      vlog_print             (void* self, const char* szFunc, int nLine, ...);
void      vlog_set               (void* self, VlogOption_e type, ...);
void      vlog_setPath           (void* self, const char* folder, const char* file);
void      vlog_setSize           (void* self, const char* GMKB);
void      vlog_setDay            (void* self, const char* UYMWDhms);
void      vlog_lock              (void* self);
void      vlog_unlock            (void* self);
int       _vlog_WriteFile        (const char* path, char* str);

VTimer*   vtimer_create     ();
void      vtimer_destroy    (VTimer* pTimer);
void      vtimer_resume     (void* self);
u64t      vtimer_diffms     (void* self);
u64t      vtimer_diffus     (void* self);
u64t      vtimer_now        (void);
char*     vtimer_nowString  (char* buf, int bufLen, const char* fmt);
char*     vtimer_tsString   (char* buf, int bufLen, const char* fmt, u64t ts);

/*************************************************************************************/
/*************************************************************************************/

int       get_NthDigit       (u64t number, int index);
int       vrandom            (int n);
void      vmsleep            (int msec);
void*     vc_malloc          (int size);
int       vc_free            (void* p, int size);
n64t      vc_getMemUsage     (void);
n64t      vc_addMemUsage     (int size);
void      vc_printMemUsage   (void);

int       fd_rename          (const char *oldPath, const char* newPath);
int       fd_isFileExist     (const char *path);
n64t      fd_getFileSize     (const char* path);
int       fd_isFolderExist   (const char *folder);
int       fd_mkdir           (const char *folder);

int       fd_open            (const char* path);
int       fd_close           (int fd);
void      fd_Backtrace_file  (const char* path);
void      fd_Backtrace       (char* buf, int len);

/*************************************************************************************/
/*************************************************************************************/

int*      int_new           (int n);
void      int_del           (int* p);
u64t      vnode_newId       (void);
VNode*    vnode_create      (void* arg);
void      vnode_destroy     (VNode* node);

/*************************************************************************************/
/*************************************************************************************/

VList*    vlist_create         (void);
void      vlist_destroy        (VList* pList);
void      vlist_lock           (void* self);
void      vlist_unlock         (void* self);
int       vlist_getSize        (void* self);

void      vlist_travel         (void* self, todo_ft todo);
VNode*    vlist_insert         (void* self, comp_ft less, void* arg);
VNode*    vlist_search         (void* self, comp_ft equal, void* arg);
void      vlist_delete         (void* self, VNode* node, dtor_ft del);
void      vlist_seek           (void* self);
VNode*    vlist_foreach        (void* self, comp_ft filter, void* arg);

VNode*    vlist_insertFrom     (void* self, POS_e from, comp_ft less, void* arg);
VNode*    vlist_searchFrom     (void* self, POS_e from, comp_ft equal, void* arg);
void      vlist_seekFrom       (void* self, POS_e from);
void      vlist_seekNode       (void* self, VNode* node);
VNode*    vlist_foreachFrom    (void* self, POS_e from, comp_ft filter, void* arg);

VNode*    _vlist_searchFalse   (void* self, POS_e from, comp_ft comp, void* arg);
void      _vlist_pushNode      (void* self, POS_e to, VNode* node);

/*************************************************************************************/
/*************************************************************************************/

VTree*    vtree_create         (void);
void      vtree_destroy        (VTree* pTree);
void      vtree_lock           (void* self);
void      vtree_unlock         (void* self);

void      vtree_travel         (void* self, todo_ft todo);
VNode*    vtree_insert         (void* self, comp_ft less, void* arg);
VNode*    vtree_search         (void* self, comp_ft equal, void* arg);
void      vtree_delete         (void* self, VNode* node, dtor_ft del);
void      vtree_seek           (void* self);
VNode*    vtree_foreach        (void* self, comp_ft filter, void* arg);
int       vtree_getSize        (void* self);

void      vtree_seekFrom       (void* self, POS_e from);
void      vtree_seekNode       (void* self, VNode* node);
VNode*    vtree_foreachFrom    (void* self, POS_e from, comp_ft filter, void* arg);

int       _vtree_isLorR        (void* self, VNode* node);
VNode*    _vtree_mostL         (void* self, VNode* node);
VNode*    _vtree_mostR         (void* self, VNode* node);
VNode*    _vtree_next          (void* self, VNode* node);
VNode*    _vtree_prev          (void* self, VNode* node);

/*************************************************************************************/
/*************************************************************************************/



/*************************************************************************************/
/*************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif  /* __VVV_H__ */

//static pthread_mutex_t lock_memory = PTHREAD_MUTEX_INITIALIZER;


