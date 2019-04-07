
#include "mq.h"


int SetUTD (UTD_st* p, char* uid, u64t T, u64t D, u64t frag, int eFileType, char* json)
{
	memset (p, 0, sizeof(UTD_st));
	if (strlen(uid)<20)
	{
		memcpy (p->uid, uid, 1+strlen(uid));
	}
	else
	{
		memcpy (p->uid, uid, 20);
	}
	p->T      = T;
	p->D      = D;
	p->frag   = frag;
	p->type   = eFileType;

	snprintf (p->json, sizeof(p->json), "%s", json);

	return 0;
}

const char* GetEventType (int type)
{

	if (type == (eTs|eIcon|eEvent))
	{
		return "Ts|Icon|Event";
	}
	else if (type == (eTs|eIcon))
	{
		return "Ts|Icon";
	}
	else if (type == (eTs|eEvent))
	{
		return "Ts|Event";
	}
	else if (type == eTs)
	{
		return "Ts";
	}
	else if (type == eIcon)
	{
		return "Icon";
	}
	else if (type == eEvent)
	{
		return "Event";
	}
	else
	{
		return "???";
	}
}

char* GetUTDString (char* buf, int bufLen, UTD_st* p)
{
	if (p->type & eEvent)
	{
		snprintf (buf, bufLen, "<%s, U:%s, T:%llu, D:%llu, f:%llu>, json=%s", GetEventType(p->type), p->uid, p->T, p->D, p->frag, p->json);
	}
	else
	{
		snprintf (buf, bufLen, "<%s, U:%s, T:%llu, D:%llu, f:%llu>", GetEventType(p->type), p->uid, p->T, p->D, p->frag);
	}

	return buf;
}

char* GetStatusString (char* buf, int bufLen, PubStatus_st* p)
{
	snprintf (buf, bufLen, "<[%d], U:%s, T:%llu>", p->status, p->uid, p->T);

	return buf;
}

int MQ_init (int key, int flag)
{
	int msgflg = flag | 0666;

	int mqid;
	mqid = msgget (key, msgflg);
	if (mqid<0)
	{
		perror ("msgget");
	}
	return mqid;
}

int MQ_deinit (int mqid)
{
	struct msqid_ds stat;

	return msgctl (mqid, IPC_RMID, &stat);
}


int MQ_send_bykey(int key, MQBUF* msg){
    int mqid = MQ_init (key, IPC_CREAT);
    if (mqid<0) {
        // printf ("ERROR num=%d\n", MQid);
        return mqid;
    }
    return MQ_send(mqid,msg);
}

int MQ_send (int mqid, MQBUF* msg) 
{
	if (mqid<0)
	{
		return -1;
	}

	size_t msg_len=sizeof(msg->mtext);

	msg->mtype = 1;
#if 0
	/* non-blocking. */
	return msgsnd(mqid, msg, msg_len, IPC_NOWAIT);
#else
	/* blocking. */
	return msgsnd(mqid, msg, msg_len, 0);
#endif
}

int MQ_stat (int mqid, MQINFO* info)
{
	int ret;
	struct msqid_ds stat;
	ret = msgctl(mqid, IPC_STAT, &stat);
	memset(info, 0, sizeof(MQINFO));
	info->qnum   = stat.msg_qnum;
	info->qbytes = stat.msg_qbytes;
	info->cbytes = stat.msg_cbytes;
	return ret;
}

int MQ_recv (int mqid, MQBUF* msg)
{
	if (mqid<0)
	{
		return -1;
	}
	size_t msg_len=sizeof(msg->mtext);
	memset (msg, 0, sizeof(MQBUF));
	return msgrcv(mqid, msg, msg_len, 1, IPC_NOWAIT);
}

int MQ_clear (int mqid)
{
    if (mqid<0)
    {   
        printf ("ERROR\n");
        return -1; 
    }   

    typedef struct {
        long    mtype;
        char    mtext[1024];
    } Msg2000_st;

    Msg2000_st msg;

    memset (&msg, 0, sizeof(Msg2000_st));
    return msgrcv(mqid, &msg, sizeof(msg.mtext), 1, IPC_NOWAIT);
}












int MQ_getid (int key)
{
	return msgget (key, 0666|IPC_CREAT);
}

int MQ_push(int key, MQBUF* msg)
{
	int mqid = MQ_getid (key);
    if (mqid<0) {
        return mqid;
    } else {
		size_t msg_len=sizeof(msg->mtext);
		msg->mtype = 1;
		return msgsnd(mqid, msg, msg_len, 0); /* 0:blocking */
	}
}

int MQ_pop (int key, MQBUF* msg)
{
	int mqid = MQ_getid (key);
    if (mqid<0) {
        return mqid;
    } else {
		size_t msg_len=sizeof(msg->mtext);
		memset (msg, 0, sizeof(MQBUF));
		return msgrcv(mqid, msg, msg_len, 1, IPC_NOWAIT); /* IPC_NOWAIT:non-blocking */
	}
}

int MQ_kill (int key)
{
	int mqid = MQ_getid (key);
    if (mqid<0) {
        return mqid;
    } else {
		struct msqid_ds stat;
		return msgctl (mqid, IPC_RMID, &stat);
	}
}

MQINFO MQ_info (int key)
{
	MQINFO info;
	memset(&info, 0, sizeof(MQINFO));
	
	int mqid = MQ_getid (key);
    if (mqid<0) {
		info.ret = mqid;
    } else {
		struct msqid_ds stat;
		info.ret = msgctl(mqid, IPC_STAT, &stat);
		info.qnum   = stat.msg_qnum;
		info.qbytes = stat.msg_qbytes;
		info.cbytes = stat.msg_cbytes;
	}
	return info;
}




