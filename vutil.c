
#include "vutil.h"



/***********************************************************************************/
/***********************************************************************************/


static int        _vlog_NeedBackupFile  (void* self, u64t ts);
static int        _vlog_BackupFile      (char* path, u64t ts);

static DateTime   _TS_to_DT         (u64t ts);
static char*      _TS_shift         (char* buf, int len, u64t ts, int digit);
static char*      _TS_cut           (char* buf, int len, u64t ts, int digit);
static u64t       _STR_to_BYTE      (const char* GMKB);
static u64t       _STR_to_MS        (const char* UYMWDhms);
static char*      _FMT_to_STR       (char* buf, int bufLen, const char* fmt, u64t ts, const char* szFunc, int nLine, va_list ap);
static char*      _FMT_to_STRxColor (char* str, int size);

static int        NumberOfBits      (const char* str);
static char*      _ms_to_str (char* buf32, u64t ms);



/***********************************************************************************/
/***********************************************************************************/

const char* VStatusStr (int s)
{
	if (s==VStatus_Init)
		return "Init";
	else if (s==VStatus_Create)
		return "Create";
	else if (s==VStatus_Start)
		return "Start";
	else if (s==VStatus_Resume)
		return "Resume";
	else if (s==VStatus_Running)
		return "Running";
	else if (s==VStatus_Pause)
		return "Pause";
	else if (s==VStatus_Stop)
		return "Stop";
	else if (s==VStatus_Restart)
		return "Restart";
	else if (s==VStatus_Destroy)
		return "Destroy";
	else if (s==VStatus_GET)
		return "GET";
	else if (s==VStatus_POST)
		return "POST";
	else if (s==VStatus_PUT)
		return "PUT";
	else if (s==VStatus_DELETE)
		return "DELETE";
	else
		return "???";
	/*
	else if (s==VStatus_)
		return "";
	*/
}

VTimer* vtimer_create  ()
{
	VTimer* pTimer    = (VTimer*) vc_malloc (sizeof(VTimer)); 
	if (!pTimer) {return NULL;}

	pTimer->resume    = vtimer_resume;
	pTimer->diffus    = vtimer_diffus;
	pTimer->diffms    = vtimer_diffms;
	pTimer->now       = vtimer_now;
	pTimer->nowStr    = vtimer_nowString;
	pTimer->tsStr     = vtimer_tsString;

	return (VTimer*)pTimer;
}

void vtimer_destroy (VTimer* pTimer)
{
	vc_free (pTimer, sizeof(VTimer));
	return ;
}

u64t vtimer_now (void)
{
	struct timeval tv;

	gettimeofday (&tv, NULL);
	u64t ms = (tv.tv_sec)*1000 + (tv.tv_usec)/1000 ;

	return ms;
}

char* vtimer_nowString (char* buf, int bufLen, const char* fmt)
{
	u64t ts = vtimer_now ();

	return _FMT_to_STR (buf, bufLen, fmt, ts, NULL, 0, NULL);
}

char* vtimer_tsString (char* buf, int bufLen, const char* fmt, u64t ts)
{
	return _FMT_to_STR (buf, bufLen, fmt, ts, NULL, 0, NULL);
}


void vtimer_resume (void* self)
{
	VTimer* pTimer = (VTimer*) self;
    gettimeofday (&pTimer->tv , NULL);
	return ;
}

u64t vtimer_diffms (void* self)
{
    return vtimer_diffus(self)>>10;
}

u64t vtimer_diffus (void* self)
{
	VTimer* pTimer = (VTimer*) self;
	struct timeval  now;
    n64t           sec=0;
    n64t           usec=0;

    gettimeofday (&now, NULL);
    sec = (now.tv_sec) - ((pTimer->tv).tv_sec);
    usec = (now.tv_usec) - ((pTimer->tv).tv_usec);
    if ( usec <0 )
	{
        sec--;
        usec += 1000000;
    }
    
	return (usec + (sec*1000000)) ;
}





/***********************************************************************************/
/***********************************************************************************/

VLog* vlog_create  (int mode, const char* folder, const char* file, const char* FmtScreen, const char* FmtFile)
{
	if (file==NULL   && strlen(file)==0)
	{
		return NULL;
	}
	if (folder==NULL && strlen(folder)==0)
	{
		return NULL;
	}

	const char* DFL_FmtScreen = "V";

	VLog* pLog = (VLog*) vc_malloc (sizeof(VLog)); 
	if (!pLog) {return NULL;}

	/* method. */
	pthread_mutex_init (&pLog->mutex, NULL);
	pLog->lock      = vlog_lock;
	pLog->unlock    = vlog_unlock;
	pLog->print     = vlog_print;
	pLog->set       = vlog_set;
	pLog->setSize   = vlog_setSize;
	pLog->setPath   = vlog_setPath;
	pLog->setDay    = vlog_setDay;
	
	/* data. */
	pLog->mode      = mode;
	if (FmtScreen==NULL || strlen(FmtScreen)==0)
	{
		memcpy (pLog->FmtScreen, DFL_FmtScreen, strlen(DFL_FmtScreen)+1);
	}
	else
	{
		memcpy (pLog->FmtScreen, FmtScreen, strlen(FmtScreen)+1);
	}
	if (FmtFile==NULL || strlen(FmtFile)==0)
	{
		memcpy (pLog->FmtFile, pLog->FmtScreen, strlen(pLog->FmtScreen)+1);
	}
	else
	{
		memcpy (pLog->FmtFile, FmtFile, strlen(FmtFile)+1);
	}
	vlog_setPath   (pLog, folder, file);
	vlog_setSize   (pLog, "1G");
	vlog_setDay    (pLog, "U");
	
	return (VLog*)pLog;
}



void vlog_destroy (VLog* pLog)
{
	vc_free (pLog, sizeof(VLog));
	return;
}



/***********************************************************************************/
/***********************************************************************************/

void vlog_lock (void* self)
{
	VLog* pLog = (VLog*) self;
	pthread_mutex_lock (&pLog->mutex);
	return ;
}
void vlog_unlock (void* self)
{
	VLog* pLog = (VLog*) self;
	pthread_mutex_unlock (&pLog->mutex);
	return ;
}

void vlog_set (void* self, VlogOption_e type, ...)
{
	VLog* pLog = (VLog*) self;
	va_list ap;
	char*   file;
	char*   folder;
	char*   str;
	
	va_start (ap, type);
	switch (type)
	{
	case PATH:
		folder=va_arg (ap, char*);
		file  =va_arg (ap, char*);
		vlog_setPath (pLog, folder, file);
		break;
	case SIZE:
		str  =va_arg (ap, char*);
		vlog_setSize (pLog, str);
		break;
	case DAY:
		str  =va_arg (ap, char*);
		vlog_setDay (pLog, str);
		break;
	default:
		break;
	}
	va_end (ap);

	return ;
}

void vlog_setSize (void* self, const char* GMKB)
{
	VLog* pLog = (VLog*) self;

	pLog->lock(pLog);
	pLog->MaxFileSize = _STR_to_BYTE(GMKB);
	if (pLog->MaxFileSize <= (1<<10))
	{
		pLog->MaxFileSize = 1 << 10;

	}
	pLog->unlock(pLog);
		
	return ;
}

void vlog_setDay (void* self, const char* UYMWDhms)
{
	VLog* pLog = (VLog*) self;
	if (!UYMWDhms || strlen(UYMWDhms)==0)
	{
		return ;
	}
	pLog->lock(pLog);
	pLog->MaxDayMs = _STR_to_MS (UYMWDhms);
	pLog->unlock(pLog);
	return ;
}

void vlog_setPath (void* self, const char* folder, const char* file)
{
	VLog* pLog = (VLog*) self;
	vzero  (pLog->folder, sizeof(pLog->folder));
	vzero  (pLog->path, sizeof(pLog->path));

	pLog->lock(pLog);
	if ((!folder)||strlen(folder)==0)
	{
		snprintf (pLog->folder, sizeof(pLog->folder), "./");
	}
	else
	{
		snprintf (pLog->folder, sizeof(pLog->folder), "%s", folder);
	}
	if ((!file)||strlen(file)==0)
	{
		snprintf (pLog->path,   sizeof(pLog->path),   "%s/%s", pLog->folder, "out.log");
	}
	else
	{
		snprintf (pLog->path,   sizeof(pLog->path),   "%s/%s", pLog->folder, file);
	}
	pLog->unlock(pLog);
	
	fd_mkdir (folder);
	return ;
}

void vlog_print (void* self, const char* szFunc, int nLine, ...)
{
	VLog* pLog = (VLog*) self;
	va_list ap;
	
	pLog->lock(pLog);
	if (pLog->mode & 0x0003)
	{
		u64t ts = vtimer_now ();

		va_start (ap, nLine);
		_FMT_to_STR (pLog->buf, sizeof(pLog->buf), pLog->FmtScreen, ts, szFunc, nLine, ap);
		va_end (ap);
	
		if (pLog->mode & 0x0001)
		{
			printf ("%s", pLog->buf);
		}
	
		if (pLog->mode & 0x0002)
		{
			if (_vlog_NeedBackupFile (self, ts))
			{
				_vlog_BackupFile (pLog->path, pLog->lastTs);
			}

			_FMT_to_STRxColor (pLog->buf, sizeof(pLog->buf));
			/* Real. */
			_vlog_WriteFile (pLog->path, pLog->buf);
		}
		pLog->lastTs = ts;
	}
	pLog->unlock(pLog);

	return;
}

static int _vlog_NeedBackupFile (void* self, u64t ts)
{
	VLog* pLog = (VLog*) self;

	/* Check size. */
	if ((fd_getFileSize (pLog->path) >= pLog->MaxFileSize))
	{
		return 1;
	}

	/* Check day. */
	if (pLog->MaxDayMs!=0)
	{
		if ((ts % pLog->MaxDayMs) < (pLog->lastTs % pLog->MaxDayMs))
		{
			return 1;
		}
	}
	return 0;
}

static int _vlog_BackupFile (char* path, u64t ts)
{
	char bakName[LEN256+64];
	char Time[64];

	vtimer_tsString (Time, sizeof(Time), "YMDhms", ts);
	snprintf (bakName, sizeof(bakName), "%s.%s", path, Time);
	
	return fd_rename (path, bakName);
}

int _vlog_WriteFile (const char* path, char* str)
{
	int fd = fd_open (path);
	if (fd>=0)
	{
		ssize_t size = write (fd, str, strlen(str));
		if (size<0) { ;/*ERROR*/ }
		fd_close (fd);
		return size;
	}
	else
	{
		return fd;
	}
}







/***********************************************************************************/
/***********************************************************************************/


static DateTime _TS_to_DT (u64t ts)
{
    DateTime    DT;
	u64t        sec_u64   = ts/1000;
	time_t      sec_timet = sec_u64;
	struct tm   stTimeInfo;

	gmtime_r (&sec_timet, &stTimeInfo);
	DT.ts    = ts;
	DT.Y     = stTimeInfo.tm_year +1900;
	DT.M     = stTimeInfo.tm_mon +1;
	DT.D     = stTimeInfo.tm_mday;
	DT.h     = stTimeInfo.tm_hour;
	DT.m     = stTimeInfo.tm_min;
	DT.s     = stTimeInfo.tm_sec;

	return DT;
}

static char* _TS_shift (char* buf, int bufLen, u64t ts, int digit)
{
	if (!buf) { return NULL; }
	if (digit<0) { return NULL; }
	memset (buf, 0, bufLen);

	int  i;
	int  d=1;
	u64t integer;
	u64t decimal;
	
	for (i=digit; i>0; i--)
	{
		d = d*10;
	}
	integer = ts / d;
	decimal = ts % d;
	if (decimal==0)
	{
		snprintf (buf, bufLen, "%llu", integer);
	}
	else
	{
		snprintf (buf, bufLen, "%llu.%0*llu", integer, digit, decimal);
	}

	buf[bufLen-1] = '\0';

	return buf;
}

static char* _TS_cut (char* buf, int bufLen, u64t ts, int digit)
{
	if (!buf) { return NULL; }
	if (digit<0) { return NULL; }
	memset (buf, 0, bufLen);

	int  i;
	int  d=1;
	u64t integer;

	for (i=digit; i>0; i--)
	{
		d = d*10;
	}
	integer = ts / d;
	snprintf (buf, bufLen, "%llu", integer);
	return buf;
}

static u64t _STR_to_MS (const char* UYMWDhms)
{
	u64t ms=0;
	const char* p = UYMWDhms;
	while (1)
	{
		n64t nNum = atoi (p);
		int nDigit = NumberOfBits (p);
		char fmt = *(p+nDigit);
		if (fmt=='\0')
		{
			break;
		}
		if (nDigit==0)
		{
			nNum = 1;
		}

		switch (fmt)
		{
		case 'U':
			ms = LLONG_MAX;
			break;
		case 'Y':
			ms += nNum * 1000 * 86400 * 365;
			break;
		case 'M':
			ms += nNum * 1000 * 86400 * 30;
			break;
		case 'W':
			ms += nNum * 1000 * 86400 * 7;
			break;
		case 'D':
			ms += nNum * 1000 * 86400;
			break;
		case 'h':
			ms += nNum * 1000 * 3600;
			break;
		case 'm':
			ms += nNum * 1000 * 60;
			break;
		case 's':
			ms += nNum * 1000;
			break;
		default:
			break;
		}
		p = (p+nDigit)+1;
	}

	return ms;
}

u64t _STR_to_BYTE (const char* GMKB)
{
	u64t size=0;
	const char* p = GMKB;
	while (1)
	{
		n64t nNum = atoi (p);
		int nDigit = NumberOfBits (p);
		char fmt = *(p+nDigit);
		if (fmt=='\0')
		{
			break;
		}
		if (nDigit==0)
		{
			nNum = 1;
		}

		switch (fmt)
		{
		case 'G':
			size += nNum << 30;
			break;
		case 'M':
			size += nNum << 20;
			break;
		case 'K':
			size += nNum << 10;
			break;
		case 'B':
			size += nNum;
			break;
		default:
			break;
		}
		p = (p+nDigit)+1;
	}

	return size;
}


static int NumberOfBits (const char* str)
{
	int i=0;
	while ( isdigit(*(str+i)) != 0 )
	{
		i++;
	}
	return i;
}


static char* _FMT_to_STRxColor (char* src, int size)
{
	const char* Color    = "\033[";
	char*       pRead;
	char*       pReadNext;
	char*       pWrite;
	int         lenColor = strlen(Color);
	int         i;
	int         offset;
	
	pRead = strstr(src, Color);
	if (pRead==NULL)
	{
		return src;
	}
	pWrite = pRead;
	while (*pRead != '\0')
	{
		pRead = pRead + lenColor;
		while (1)
		{
			if (*(pRead)=='m')
			{
				pRead++;
				break;
			}

			if (pRead>=(src+size-1))
			{
				break;
			}

			pRead++;
		}
		pReadNext = strstr(pRead, Color);
		offset = (pReadNext) ? (size_t)(pReadNext - pRead) : strlen(pRead);
		for (i=0; i<offset; i++)
		{
			*(pWrite) = *(pRead);
			pWrite++;
			pRead++;
		}
		*(pWrite) = '\0';
	}

	return src;
}

static char* _FMT_to_STR (char* buf, int bufLen, const char* fmt, u64t ts, const char* szFunc, int nLine, va_list ap)
{
	/* fmt=[Y/M/D h:m:s.u3 | F10():L4 | S | X | V] */

	if (buf==NULL || fmt==NULL)
	{
		return NULL;
	}

	char*        p = buf;
	int          space = bufLen;
	int          fmtLen = strlen(fmt);
    DateTime     DT = _TS_to_DT (ts);
	int          offset;
	int          i;
	int          nWidth;
	int          nDigit;
	char*        _p;

	memset (buf, 0, bufLen);
	for (i=0; i<fmtLen; i++)
	{
		switch (fmt[i])
		{
		/* User string */
			case 'X':
				if (ap)
				{
					_p=va_arg (ap, char*);
					(void)(_p);
				}
				break;
			case 'V':
				if (ap)
				{
					_p=va_arg (ap, char*);
					vsnprintf (p, space, _p, ap);
				}
				break;
			case 'S':
				if (ap)
				{
					nWidth = atoi (fmt+i+1);
					nDigit = NumberOfBits (fmt+i+1);
					i = i + nDigit;
					_p=va_arg (ap, char*);
					snprintf (p, space, "%*s", nWidth, _p);
				}
				break;
		/* Func & Line */
			case 'F':
				nWidth = atoi (fmt+i+1);
				nDigit = NumberOfBits (fmt+i+1);
				i = i + nDigit;
				snprintf (p, space, "%*s", nWidth, szFunc);
				break;
			case 'L':
				nWidth = atoi (fmt+i+1);
				nDigit = NumberOfBits (fmt+i+1);
				i = i + nDigit;
				snprintf (p, space, "%*d", nWidth, nLine);
				break;
		/* Time */
			case 'Y':
				snprintf (p, space, "%04d", DT.Y);
				break;
			case 'M':
				snprintf (p, space, "%02d", DT.M);
				break;
			case 'D':
				snprintf (p, space, "%02d", DT.D);
				break;
			case 'h':
				snprintf (p, space, "%02d", DT.h);
				break;
			case 'm':
				snprintf (p, space, "%02d", DT.m);
				break;
			case 's':
				snprintf (p, space, "%02d", DT.s);
				break;
			case 'u':
				nWidth = atoi (fmt+i+1);
				nDigit = NumberOfBits (fmt+i+1);
				i = i + nDigit;
				if (nWidth==1)
				{
					snprintf (p, space, "%0*llu", nWidth, (ts%1000)/100);
				}
				else if (nWidth==2)
				{
					snprintf (p, space, "%0*llu", nWidth, (ts%1000)/10);
				}
				else
				{
					snprintf (p, space, "%0*llu", 3, (ts%1000));
				}
				break;
			case 'T':
				if (*(fmt+(i+1))=='.')
				{
					nWidth = atoi (fmt+(i+1)+1);
					nDigit = NumberOfBits (fmt+(i+1)+1);
					i = (i+1) + nDigit;
					{
						char T[32];
						_TS_shift (T, sizeof(T), DT.ts, nWidth);
						snprintf (p, space, "%s", T);
					}
				}
				else if (*(fmt+(i+1))=='-')
				{
					nWidth = atoi (fmt+(i+1)+1);
					nDigit = NumberOfBits (fmt+(i+1)+1);
					i = (i+1) + nDigit;
					{
						char T[32];
						_TS_cut (T, sizeof(T), DT.ts, nWidth);
						snprintf (p, space, "%s", T);
					}
				}
				else
				{
					snprintf (p, space, "%c", fmt[i]);
				}
				break;
			default:
				snprintf (p, space, "%c", fmt[i]);
				break;
		}
		offset = strlen(p);
		space = space - offset; 
		p = p + offset;
	}
	buf[bufLen-1] = '\0';

	return buf;
}


/***********************************************************************************/
/***********************************************************************************/





VLock* vlock_create ()
{
	VLock* pLock = (VLock*) vc_malloc (sizeof(VLock));
	pLock->lock = vlock_lock;
	pLock->unlock = vlock_unlock;
	pthread_mutex_init (&pLock->mutex, NULL);
	return pLock;
}

void vlock_destroy (VLock* pLock)
{
	vc_free (pLock, sizeof(VLock));
	return ;
}

void vlock_lock (void* self)
{
	VLock* pLock = (VLock*) self;
	pthread_mutex_lock (&pLock->mutex);
	return ;
}
void vlock_unlock (void* self) 
{
	VLock* pLock = (VLock*) self;
	pthread_mutex_unlock (&pLock->mutex);
	return ;
}





void fd_Backtrace (char* buf, int len)
{
    void*   bt[1024];
    int     bt_size;
    char**  bt_syms;
    int     i;

    bt_size = backtrace(bt, 1024);
    bt_syms = backtrace_symbols(bt, bt_size);
    
    for (i=1; i<bt_size; i++)
	{
		//snprintf (buf, sizeof(buf), "%s\n", bt_syms[i]);
		printf ("%s\n", bt_syms[i]);
	}
	free(bt_syms);
}

int get_NthDigit (u64t number, int index)
{
	int i;
	u64t Quo;
	u64t Div=1;

	for (i=1; i<index; i++)
	{
		Div *= 10;
	}
	Quo = (number / Div);
	return Quo%10;
}

#if 1
static char* _ms_to_str (char* buf32, u64t ms)
{
	/* 2017/04/06 05:28:21 */
	DateTime DT = _TS_to_DT (ms);
	int i;
	char* p=buf32;
	/* clear buffer. */
	for (i=0; i<32; i++)
	{
		buf32[i]='\0';
	}
	/* Y/M/D */
	for (i=0; i<4; i++)
	{
		*p = 48 + get_NthDigit(DT.Y, 4-i);
		p++;
	}
	*p='/'; p++;
	for (i=0; i<2; i++)
	{
		*p = 48 + get_NthDigit(DT.M, 2-i);
		p++;
	}
	*p='/'; p++;
	for (i=0; i<2; i++)
	{
		*p = 48 + get_NthDigit(DT.D, 2-i);
		p++;
	}
	*p=' '; p++;
	/* h:m:s */
	for (i=0; i<2; i++)
	{
		*p = 48 + get_NthDigit(DT.h, 2-i);
		p++;
	}
	*p=':'; p++;
	for (i=0; i<2; i++)
	{
		*p = 48 + get_NthDigit(DT.m, 2-i);
		p++;
	}
	*p=':'; p++;
	for (i=0; i<2; i++)
	{
		*p = 48 + get_NthDigit(DT.s, 2-i);
		p++;
	}
	*p='\n'; p++;
	*p='\0';
	return buf32;
}

void fd_Backtrace_file (const char* path)
{
	int      fd;
    void*    bt[1024];
    int      bt_size;
	u64t     now;
	char     buf[32];

	bt_size = backtrace(bt, 1024);
	fd = fd_open (path);
	if (fd>=0)
	{
		now = vtimer_now();
		_ms_to_str (buf, now);
		ssize_t size = write (fd, buf, 20);
		if (size<0) { ;/*ERROR*/ }
		backtrace_symbols_fd (bt, bt_size, fd);
		fd_close (fd);
	}

	return ;
}
#endif



int fd_open (const char* path)
{
	int flags = O_WRONLY | O_APPEND;
	int fd;
	
	fd = open (path, flags);
	if (fd<0)
	{
		if (errno==ENOENT)
		{
		    fd = creat (path, 0644); // fd = open (path, O_WRONLY | O_CREAT| O_TRUNK, 0644);
			if (fd<0)
			{
				return fd;
			}
		}
		else
		{
			return fd;
		}
	}

	return fd;
}

int fd_close (int fd)
{
	return close (fd);
}

int fd_rename (const char *oldPath, const char* newPath)
{
	return rename (oldPath, newPath);
}

int fd_isFileExist (const char *path)
{
	struct stat s;

	return (stat(path , &s)==0)?1:0 ;
}


int fd_isFolderExist (const char *folder)
{
	struct stat s;
	int ret;

	ret = stat(folder , &s);
	if (ret==-1)
	{
		if (errno == ENOENT)
		{
			/* does not exist. */
			return 0;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		if(S_ISDIR(s.st_mode))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
}

n64t fd_getFileSize (const char* path)
{
	struct stat st;
	memset (&st, 0, sizeof(struct stat));
	stat (path, &st);
	
	return st.st_size;
}

int fd_mkdir(const char *folder)
{
	mode_t mode = (S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	char *q, *r = NULL, *path = NULL, *up = NULL;
	int ret;

	ret = -1;
	if (strcmp(folder, ".") == 0 || strcmp(folder, "/") == 0)
		return (0);

	if ((path = strdup(folder)) == NULL)
		exit(1);

	if ((q = strdup(folder)) == NULL)
		exit(1);

	if ((r = dirname(q)) == NULL)
		goto out;

	if ((up = strdup(r)) == NULL)
		exit(1);

	if ((fd_mkdir(up) == -1) && (errno != EEXIST))
		goto out;

	if ((mkdir(path, mode) == -1) && (errno != EEXIST))
		ret = -1;
	else
		ret = 0;

out:
	if (up != NULL)
		free(up);
	free(q);
	free(path);

	return (ret);
}










#if 0
void vzero (void* dest, int size)
{
	memset (dest, 0, size);
	return ;
}
#endif

int vrandom (int n)
{
	static int bInitSrand=0;
	if (bInitSrand==0)
	{
		srand(time(NULL));
		bInitSrand=1;
	}
	return (rand()%n);
}

void vmsleep (int msec)
{
	int sec;
	int usec;

	sec = msec/1000;
	usec = (msec%1000)*1000;
	sleep (sec);
	usleep (usec);

	return ;
}

n64t vc_addMemUsage (int size)
{
	static n64t gMemUsage=0;
	return  __sync_add_and_fetch (&gMemUsage, size);
}

n64t vc_getMemUsage (void)
{
	return vc_addMemUsage(0);
}

void vc_printMemUsage (void)
{
	printf ("[VC_MEMORY]: %llu B\n", vc_addMemUsage(0));
}

void* vc_malloc (int size)
{
	if (size<=0)
	{
		return NULL;
	}
	
	void* p;
	p = malloc (size); 
	if (p!=NULL)
	{
		vc_addMemUsage (size);
		//vzero (p, size);
		memset (p, 0, size);
	}
	return p;
}

int vc_free (void* p, int size)
{
	if (p==NULL || size<=0)
	{
		return -1;
	}

	vc_addMemUsage ((-1)*size);
	free (p);
	p = NULL;
	return 0;
}






/*************************************************************/
/*************************************************************/

int* int_new (int n)
{
	int* p = (int*) vc_malloc (sizeof(int));
	*p = n;
	return p;
}

void int_del (int* p)
{
	vc_free (p, sizeof(int));
}

/*************************************************************/
/*************************************************************/

u64t vnode_newId (void)
{
	static u64t id=0;
	return  __sync_add_and_fetch (&id, 1);
}

VNode* vnode_create (void* arg)
{
	VNode* node = (VNode*) vc_malloc (sizeof(VNode));
	if (!node) { return NULL; }

	node->id = vnode_newId();
	node->arg = arg;
	return node;
}

void vnode_destroy (VNode* node)
{
	vc_free (node, sizeof(VNode));
	return ;
}


/*************************************************************/
/*************************************************************/

VQueue* vqueue_create  (int nMaxSize)
{
	VQueue* pQueue = (VQueue*) vc_malloc (sizeof(VQueue)); 
	if (!pQueue) {return NULL;}

	pQueue->getSize= vqueue_getSize;
	pQueue->push   = vqueue_push;
	pQueue->pop    = vqueue_pop;
	pQueue->lock   = vqueue_lock;
	pQueue->unlock = vqueue_unlock;

	pthread_mutex_init (&pQueue->mutex, NULL);
	pQueue->nMaxSize = nMaxSize;

	return pQueue;
}

void vqueue_destroy (VQueue* pQueue)
{
	vc_free (pQueue, sizeof(VQueue));
	return ;
}

void vqueue_lock (void* self)
{
	VQueue* pQueue = (VQueue*) self;
	pthread_mutex_lock (&pQueue->mutex);
}

void vqueue_unlock (void* self)
{
	VQueue* pQueue = (VQueue*) self;
	pthread_mutex_unlock (&pQueue->mutex);
}

int vqueue_getSize (void* self)
{
	VQueue* pQueue = (VQueue*) self;
	return pQueue->size;
}

int vqueue_push (void* self, void* arg)
{
	VQueue* pQueue = (VQueue*) self;

	int result=0;
	
	vqueue_lock (self);
	do {
		if (pQueue->size == pQueue->nMaxSize)
		{
			result = FAIL;
			break;
		}


		VNode* node = vnode_create (arg);
		if (!node)
		{
			result = FAIL;
			break;
		}

		if (pQueue->tail)
		{
			VNode* p = pQueue->tail;
			p->next = node;
			node->prev = p;
			pQueue->tail = node;
		}
		else
		{
			pQueue->head = node;
			pQueue->tail = node;
			node->prev = NULL;
			node->next = NULL;
		}

		pQueue->size++;
		result = pQueue->size;
	} while(0);
	vqueue_unlock (self);


	return result;
}

void* vqueue_pop (void* self)
{
	VQueue* pQueue = (VQueue*) self;
	
	VNode* p=NULL;
	void* arg=NULL;

	vqueue_lock (self);
	do {
		if (!pQueue->head)
		{
			break;
		}
		
		p = pQueue->head;

		if (p->next)
		{
			pQueue->head = p->next;
			pQueue->head->prev = NULL;
			p->next = NULL;
		}
		else
		{
			/* Last one. */
			pQueue->tail = NULL;
			pQueue->head = NULL;
			p->next = NULL;
		}

		pQueue->size--;
		arg = p->arg;
		vnode_destroy (p);
	} while(0);
	vqueue_unlock (self);


	return arg;
}


/*************************************************************/
/*************************************************************/

VStack* vstack_create  (int nMaxSize)
{
	VStack* pStack = (VStack*) vc_malloc (sizeof(VStack)); 
	if (!pStack) {return NULL;}

	pStack->getSize= vstack_getSize;
	pStack->push   = vstack_push;
	pStack->pop    = vstack_pop;
	pStack->lock   = vstack_lock;
	pStack->unlock = vstack_unlock;

	pthread_mutex_init (&pStack->mutex, NULL);
	pStack->nMaxSize = nMaxSize;

	return pStack;
}

void vstack_destroy (VStack* pStack)
{
	vc_free (pStack, sizeof(VStack));
	return ;
}

void vstack_lock (void* self)
{
	VStack* pStack = (VStack*) self;
	pthread_mutex_lock (&pStack->mutex);
}

void vstack_unlock (void* self)
{
	VStack* pStack = (VStack*) self;
	pthread_mutex_unlock (&pStack->mutex);
}

int vstack_getSize (void* self)
{
	VStack* pStack = (VStack*) self;
	return pStack->size;
}

int vstack_push (void* self, void* arg)
{
	VStack* pStack = (VStack*) self;

	int result=0;
	
	vstack_lock (self);
	do {
		if (pStack->size == pStack->nMaxSize)
		{
			result = FAIL;
			break;
		}


		VNode* node = vnode_create (arg);
		if (!node)
		{
			result = FAIL;
			break;
		}

		if (pStack->tail)
		{
			VNode* p = pStack->tail;
			p->next = node;
			node->prev = p;
			pStack->tail = node;
		}
		else
		{
			pStack->head = node;
			pStack->tail = node;
			node->prev = NULL;
			node->next = NULL;
		}

		pStack->size++;
		result = pStack->size;
	} while(0);
	vstack_unlock (self);


	return result;
}

void* vstack_pop (void* self)
{
	VStack* pStack = (VStack*) self;
	
	VNode* p;
	void* arg=NULL;
	vstack_lock (self);
	p = pStack->tail;
	do {
		if (!pStack->tail)
		{
			break;
		}

		if (p->prev)
		{
			pStack->tail = p->prev;
			pStack->tail->next = NULL;
			p->prev = NULL;
		}
		else
		{
			/* Last one. */
			pStack->tail = NULL;
			pStack->head = NULL;
			p->prev = NULL;
		}

		pStack->size--;
		arg = p->arg;
		vnode_destroy (p);
	} while(0);
	vstack_unlock (self);


	return arg;
}


/*************************************************************/
/*************************************************************/

VList* vlist_create  (void)
{
	VList* pList = (VList*) vc_malloc (sizeof(VList)); 
	if (!pList) {return NULL;}
	
	/* method. */
	pList->lock        = vlist_lock;
	pList->unlock      = vlist_unlock;
	pList->insert      = vlist_insert;
	pList->search      = vlist_search;
	pList->Delete      = vlist_delete;
	pList->seek        = vlist_seek;
	pList->foreach     = vlist_foreach;
	pList->travel      = vlist_travel;
	pList->getSize     = vlist_getSize;
	/* data. */
	pList->Lock        = vlock_create();
	
	/* inheritance. */

	return pList;
}

void vlist_destroy (VList* pList)
{
	vlock_destroy(pList->Lock);
	vc_free (pList, sizeof(VList));
	return ;
}

#if 1
void vlist_lock (void* self)
{
	VList* pList = (VList*) self;
	pList->Lock->lock(pList->Lock);
}
void vlist_unlock (void* self) 
{
	VList* pList = (VList*) self;
	pList->Lock->unlock(pList->Lock);
}
#endif


int vlist_getSize (void* self)
{
	VList* pList = (VList*) self;
	return pList->size;
}





void vlist_travel (void* self, todo_ft todo)
{
	VList* pList = (VList*) self;
	VNode* pNode=NULL;

	if (todo)
	{
		pNode = pList->head;
		while (pNode)
		{
			todo (pNode->arg);
			pNode = pNode->next;
		}
	}
	return ;
}


VNode* vlist_insertFrom (void* self, POS_e from, comp_ft less, void* arg)
{
	VList* pList = (VList*) self;
	VNode* node = vnode_create (arg);
	if (!node)
	{
		return NULL;
	}
	pList->size++;

	if (!less)
	{
		_vlist_pushNode (pList, LAST, node);
	}
	else
	{
		VNode* pNode = _vlist_searchFalse (pList, from, less, arg);

			if (from==FIRST)
			{
				if (pNode)
				{
					if (pNode->prev)
					{
						/*  ...qNp...  */
						node->prev = pNode->prev;
						node->next = pNode;
						pNode->prev->next = node;
						pNode->prev = node;
					}
					else
					{
						/*         Np...  */
						_vlist_pushNode (pList, FIRST, node);
					}
				}
				else
				{
						/*         N         */
						/*      ...N         */
						_vlist_pushNode (pList, LAST, node);
				}
			}
			else if (from==LAST)
			{
				if (pNode)
				{
					if (pNode->next)
					{
						/*  ...pNq...  */
						node->next = pNode->next;
						node->prev = pNode;
						pNode->next->prev = node;
						pNode->next = node;
					}
					else
					{
						/*  ...pN      */
						_vlist_pushNode (pList, LAST, node);
					}
				}
				else
				{
						/*      N      */
						/*      N...   */
						_vlist_pushNode (pList, FIRST, node);
				}
			}
	}

	return node;
}

VNode* vlist_insert (void* self, comp_ft less, void* arg)
{
	return vlist_insertFrom (self, FIRST, less, arg);
}


VNode* vlist_searchFrom (void* self, POS_e from, comp_ft equal, void* arg)
{
	VList* pList = (VList*) self;
	if (!equal)
	{
		return NULL;
	}
	
	VNode* pNode=NULL;
	if (from==FIRST)
	{
		pNode = pList->head;
	}
	else if (from==LAST)
	{
		pNode = pList->tail;
	}

	while (pNode)
	{
		if (equal(pNode->arg, arg)==OKAY)
		{
			break;
		}
		else
		{
			if (from==FIRST)
			{
				pNode = pNode->next;
			}
			else if (from==LAST)
			{
				pNode = pNode->prev;
			}
		}
	}

	return pNode;
}

VNode* vlist_search (void* self, comp_ft equal, void* arg)
{
	return vlist_searchFrom (self, FIRST, equal, arg);
}

void vlist_delete (void* self, VNode* node, dtor_ft del)
{
	VList* pList = (VList*) self;
	VNode* pNode = node;
	if (pNode)
	{
		if (pList->cur == pNode)
		{
			pList->cur = NULL;
		}
		if (pNode->prev)
		{
			pNode->prev->next = pNode->next;
		}
		else
		{
			pList->head = pNode->next;
		}
		if (pNode->next)
		{
			pNode->next->prev = pNode->prev;
		}
		else
		{
			pList->tail = pNode->prev;
		}
		if (del)
		{
			del(pNode->arg);
		}
		vnode_destroy (pNode);
		pNode = NULL;
		pList->size--;
	}
	return ;
}

void vlist_seekNode (void* self, VNode* node)
{
	VList* pList = (VList*) self;
	if (node)
	{
		pList->cur = node;
	}
	else
	{
		pList->cur = pList->head;
	}
	return ;
}

void  vlist_seekFrom (void* self, POS_e from)
{
	VList* pList = (VList*) self;
	if (from==FIRST)
	{
		pList->cur = pList->head;
	}
	else if (from==LAST)
	{
		pList->cur = pList->tail;
	}
	else
	{
		pList->cur = NULL;
	}
	return ;
}

void vlist_seek (void* self)
{
	vlist_seekFrom (self, FIRST);
	return ;
}

VNode* vlist_foreachFrom   (void* self, POS_e from, comp_ft filter, void* arg)
{
	VList* pList = (VList*) self;
	VNode* pNode=NULL;

	if (filter && arg)
	{
		while ((pNode = pList->cur))
		{
			if (from == FIRST)
			{
				pList->cur = pList->cur->next;
			}
			else if (from == LAST)
			{
				pList->cur = pList->cur->prev;
			}
			else
			{
				pList->cur = NULL;
			}
			if (filter(pNode->arg, arg)==OKAY)
			{
				break;
			}
		}
	}
	else
	{
		if ((pNode = pList->cur))
		{
			if (from == FIRST)
			{
				pList->cur = pList->cur->next;
			}
			else if (from == LAST)
			{
				pList->cur = pList->cur->prev;
			}
			else
			{
				pList->cur = NULL;
			}
		}
	}
	return pNode;
}

VNode* vlist_foreach (void* self, comp_ft filter, void* arg)
{
	return vlist_foreachFrom (self, FIRST, filter, arg);
}


void _vlist_pushNode (void* self, POS_e to, VNode* node)
{
	VList* pList = (VList*) self;
	
	if (to==FIRST)
	{
		if (pList->head)
		{
			/*   N...   */
			node->next = pList->head;
			pList->head->prev = node;
			pList->head = node;
		}
		else
		{
			/*   N   */
			pList->head = node;
			pList->tail = node;
		}
	}
	else if (to==LAST)
	{
		if (pList->tail)
		{
			/*   ...N   */
			node->prev = pList->tail;
			pList->tail->next = node;
			pList->tail = node;
		}
		else
		{
			/*   N   */
			pList->head = node;
			pList->tail = node;
		}
	}

	return ;
}


VNode* _vlist_searchFalse (void* self, POS_e from, comp_ft comp, void* arg)
{
	VList* pList = (VList*) self;
	if (!comp)
	{
		return NULL;
	}
	
	VNode* pNode=NULL;
	if (from==FIRST)
	{
		pNode = pList->head;
	}
	else if (from==LAST)
	{
		pNode = pList->tail;
	}

	while (pNode)
	{
		if (comp(pNode->arg, arg)==OKAY)
		{
			if (from==FIRST)
			{
				pNode = pNode->next;
			}
			else if (from==LAST)
			{
				pNode = pNode->prev;
			}
		}
		else
		{
			break;
		}
	}
	return pNode;
}



/*************************************************************/
/*************************************************************/

























/*************************************************************/
/*************************************************************/




VTree* vtree_create (void)
{
	VTree* pTree = (VTree*) vc_malloc (sizeof(VTree)); 
	if (!pTree) { return NULL; }

	/* method. */
	pTree->lock        = vtree_lock;
	pTree->unlock      = vtree_unlock;
	pTree->insert      = vtree_insert;
	pTree->search      = vtree_search;
	pTree->Delete      = vtree_delete;
	pTree->seek        = vtree_seek;
	pTree->travel      = vtree_travel;
	pTree->foreach     = vtree_foreach;
	pTree->getSize     = vtree_getSize;
	/* data. */
	pTree->Lock        = vlock_create();

	return pTree;
}

void vtree_destroy (VTree* pTree)
{
	vlock_destroy (pTree->Lock);
	vc_free (pTree, sizeof(VTree));
	return ;
}

void vtree_lock (void* self)
{
	VTree* pTree = (VTree*) self;
	pTree->Lock->unlock(pTree->Lock);
	//pthread_mutex_lock (&pTree->mutex);
}
void vtree_unlock (void* self) 
{
	VTree* pTree = (VTree*) self;
	pTree->Lock->unlock(pTree->Lock);
}

int vtree_getSize (void* self)
{
	VTree* pTree = (VTree*) self;
	return pTree->size;
}



void vtree_travel (void* self, todo_ft todo)
{
	VTree* pTree = (VTree*) self;
	if (todo)
	{
		VNode* pNode = _vtree_mostL (self, pTree->head);
		if (pNode)
		{
			todo (pNode->arg);
		}
		while ((pNode = _vtree_next(self, pNode)))
		{
			todo (pNode->arg);
		}
	}
	return ;
}

VNode* vtree_insert (void* self, comp_ft less, void* arg)
{
	if (!less)
	{
		return NULL;
	}
	VTree* pTree = (VTree*) self;
	VNode* node = vnode_create (arg);
	if (!node)
	{
		return NULL;
	}
	pTree->size++;

	VNode* pNode = pTree->head;
	if (!pNode)
	{
		pTree->head = node;
	}
	else
	{
		while (pNode)
		{
			/* |            Node            |
			 * |          /      \          |
			 * |     L:False     R:True     | */
			if(less(pNode->arg, arg)==OKAY)
			{
				if (!pNode->R)
				{
					pNode->R = node;
					node->F = pNode;
					break;
				}
				else
				{
					pNode = pNode->R;
					continue;
				}
			}
			else
			{
				if (!pNode->L)
				{
					pNode->L = node;
					node->F = pNode;
					break;
				}
				else
				{
					pNode = pNode->L;
					continue;
				}
			}
		}
	}
	return node;
}


VNode* vtree_search (void* self, comp_ft equal, void* arg)
{
	VTree* pTree = (VTree*) self;
	VNode* pNode=pTree->head;
	if (!pNode || !equal)
	{
		return NULL;
	}

	while (pNode)
	{
		int ret=equal(pNode->arg, arg);
		if (ret==0)
		{
			/* Found. */
			break;
		}
		else if (ret<0)
		{
			/* Find L. */
			pNode = pNode->L;
		}
		else if (ret>0)
		{
			/* Find R. */
			pNode = pNode->R;
		}
	}
	return pNode;
}

void vtree_delete (void* self, VNode* node, dtor_ft del)
{
	VTree* pTree = (VTree*) self;
	VNode* pNode = node;
	if (!pNode)
	{
		return ;
	}

	void* pArg = pNode->arg;
	while (pNode)
	{
		if (!pNode->L && !pNode->R)
		{
			/* 0 child. */
			if (_vtree_isLorR(self, pNode)<0)
			{
				pNode->F->L = NULL;
			}
			else if (_vtree_isLorR(self, pNode)>0)
			{
				pNode->F->R = NULL;
			}
			else if (_vtree_isLorR(self, pNode)==0)
			{
				pTree->head = NULL;
			}
			vnode_destroy (pNode);
			pNode = NULL;
			pTree->size--;
		}
		else if (pNode->L && pNode->R)
		{
			/* 2 children. */
			VNode* pCandidate = _vtree_mostR (self, pNode->L);
			pNode->arg = pCandidate->arg;
			pNode = pCandidate;
		}
		else
		{
			/* 1 child. */
			if (pNode->L)
			{
				if (_vtree_isLorR(self, pNode)<0)
				{
					pNode->F->L = pNode->L;
					pNode->L->F = pNode->F;
				}
				else if (_vtree_isLorR(self, pNode)>0)
				{
					pNode->F->R = pNode->L;
					pNode->L->F = pNode->F;
				}
				else if (_vtree_isLorR(self, pNode)==0)
				{
					pTree->head = pNode->L;
					pNode->L->F = NULL;
				}
			}
			else if (pNode->R)
			{
				if (_vtree_isLorR(self, pNode)<0)
				{
					pNode->F->L = pNode->R;
					pNode->R->F = pNode->F;
				}
				else if (_vtree_isLorR(self, pNode)>0)
				{
					pNode->F->R = pNode->R;
					pNode->R->F = pNode->F;
				}
				else if (_vtree_isLorR(self, pNode)==0)
				{
					pTree->head = pNode->R;
					pNode->R->F = NULL;
				}
			}
			vnode_destroy (pNode);
			pNode = NULL;
			pTree->size--;
		}
	}

	if (del)
	{
		del(pArg);
	}
	return ;
}


void vtree_seekNode (void* self, VNode* node)
{
	VTree* pTree = (VTree*) self;
	if (node)
	{
		pTree->cur = node;
	}
	else
	{
		pTree->cur = _vtree_mostL (self, pTree->head);
	}
	return ;
}

void vtree_seekFrom (void* self, POS_e from) 
{
	VTree* pTree = (VTree*) self;

	if (from==FIRST)
	{
		pTree->cur = _vtree_mostL (self, pTree->head);
	}
	else if (from==LAST)
	{
		pTree->cur = _vtree_mostR (self, pTree->head);
	}
	else
	{
		pTree->cur = NULL;
	}
	return ;
}

void vtree_seek (void* self)
{
	vtree_seekFrom (self, FIRST);
	return ;
}

VNode* vtree_foreachFrom (void* self, POS_e from, comp_ft filter, void* arg)
{
	VTree* pTree = (VTree*) self;
	VNode* pNode = NULL;
	
	if (filter && arg)
	{
		while ((pNode = pTree->cur))
		{
			if (from == FIRST)
			{
				pTree->cur = _vtree_next(self, pNode);
			}
			else if (from == LAST)
			{
				pTree->cur = _vtree_prev(self, pNode);
			}
			else
			{
				pTree->cur = NULL;
			}
			if (filter(pNode->arg, arg)==OKAY)
			{
				break;
			}
		}
	}
	else
	{
		if ((pNode = pTree->cur))
		{
			if (from == FIRST)
			{
				pTree->cur = _vtree_next(self, pNode);
			}
			else if (from == LAST)
			{
				pTree->cur = _vtree_prev(self, pNode);
			}
			else
			{
				pTree->cur = NULL;
			}
		}
	}
	return pNode;
}

VNode* vtree_foreach (void* self, comp_ft filter, void* arg)
{
	return vtree_foreachFrom (self, FIRST, filter, arg);
}


VNode* _vtree_mostR (void* self, VNode* node)
{
	VNode* pNode=node;
	if (pNode)
	{
		while (pNode->R)
		{
			pNode = pNode->R;
		}
	}
	return pNode;
}

VNode* _vtree_mostL (void* self, VNode* node)
{
	VNode* pNode=node;
	if (pNode)
	{
		while (pNode->L)
		{
			pNode = pNode->L;
		}
	}
	return pNode;
}

VNode* _vtree_next (void* self, VNode* node)
{
	VNode* pNode = node;
	if (!pNode)
	{
		return NULL;
	}

	if (pNode->R)
	{
		return _vtree_mostL (self, pNode->R);
	}
	else
	{
		VNode* parent = pNode->F;
		while (parent && pNode==parent->R)
		{
			pNode = parent;
			parent = parent->F;
		}
		return parent;
	}
}

VNode* _vtree_prev (void* self, VNode* node)
{
	VNode* pNode=node;
	if (!pNode)
	{
		return NULL;
	}

	if (pNode->L)
	{
		return _vtree_mostR (self, pNode->L);
	}
	else
	{
		VNode* parent = pNode->F;
		while (parent && pNode==parent->L)
		{
			pNode = parent;
			parent = parent->F;
		}
		return parent;
	}
}


int _vtree_isLorR (void* self, VNode* node)
{
	if (node->F)
	{
		if (node==node->F->L)
		{
			return -1;
		}
		else if (node==node->F->R)
		{
			return 1;
		}
	}
	return 0;
}




