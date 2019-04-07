#ifndef __VSAAS_MQ_H__
#define __VSAAS_MQ_H__

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long long u64t;
typedef          long long n64t;
typedef unsigned int       u32t;
typedef          int       n32t;

#define MQ_KEY_UTD        (15501)
#define MQ_KEY_PUBSTATUS  (15502)


typedef enum {
	eTs    = 0x0001,
	eIcon  = 0x0002,
	eEvent = 0x0004,
} FileType_e;

typedef struct {
	int   type;           /* FileType_e) eTs | eIcon | eEvent */
	int   reserve;        /* VStatus_e)  VStatus_Start / VStatus_Running / VStatus_Stop */
	char  uid[24];
	u64t  T;              /* timestamp (ms.) */
	u64t  D;              /* duration (ms.)  */
	u64t  frag;           /* # of fragment.  */
    char  json[128];
} UTD_st;

typedef struct {
	int   status;         /* VStatus_Start / VStatus_Stop */
	char  uid[24];
	u64t  T;              /* timestamp (ms.) */
} PubStatus_st;

typedef struct {
	int   ret;
	u64t  qnum;
	u64t  cbytes;
	u64t  qbytes;
} MQINFO;

typedef struct {
	long    mtype;
	char    mtext[200];
	//char    mtext[2048];
} MQBUF;



const char* GetEventType (int act);
char*   GetUTDString (char* buf, int bufLen, UTD_st* p);
char*   GetStatusString (char* buf, int bufLen, PubStatus_st* p);
int     SetUTD  (UTD_st* p, char* uid, u64t T, u64t D, u64t frag, int eFileType, char* json);

/* Old API */
int     MQ_init (int key, int flag);         /* flag => 0:recv, IPC_CREAT:send */
int     MQ_deinit (int mqid);
int     MQ_send (int mqid, MQBUF* msg);
int     MQ_recv (int mqid, MQBUF* msg);
int     MQ_stat (int mqid, MQINFO* info);
int     MQ_clear (int mqid);
int	    MQ_send_bykey(int key, MQBUF* msg);

/* New API */
int MQ_getid (int key);
int MQ_push(int key, MQBUF* msg);
int MQ_pop (int key, MQBUF* msg);
int MQ_kill (int key);
MQINFO MQ_info (int key);

#ifdef __cplusplus
}
#endif

#endif //__VSAAS_MQ_H__

