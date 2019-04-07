#include <stdio.h>
#include <pthread.h>
#include "vutil.h"
#include "vhash.h"

typedef struct {
	char uid[24];
	u64t T;
	int  id;
} uidEntry;

VHashTable* ht; 

uidEntry* uidEntry_create  (char* uid, int id)
{
	uidEntry* e = (uidEntry*)vc_malloc (sizeof(uidEntry));
	snprintf(e->uid, sizeof(e->uid), "%s", uid);
	e->id = id;
	return e;
}

void uidEntry_destroy  (uidEntry* e)
{
	vc_free (e, sizeof(uidEntry));
	return ;
}

void uidEntry_print  (uidEntry* e)
{
	printf ("(uid=%s | id=%d)\n", e->uid, e->id);
}

u32t hashcode (size_t n, char* key)
{
	u32t sum=0;
	size_t i;
	for (i=0; i<strlen(key); i++)
	{
		sum = sum + key[i];
	}
	return sum % n;
}

int match (void* key, void* item)
{
	uidEntry* eItem = (uidEntry*) item;
	
	return (strcmp((char*)key, eItem->uid)==0) ? OKAY : FAIL;
}

int filter (void* Bound, void* item)
{
	uidEntry* eItem = (uidEntry*) item;
	int n = *(int*) Bound;
	if (eItem->id < n) {
		//printf (CCr "[%d]\n" CCe, eItem->id);
		return OKAY;
	} else {
		return FAIL;
	}
}

int printHT(void* arg)
{
	uidEntry* eItem = (uidEntry*) arg;
	//printf ("[%s|%d]", eItem->uid, eItem->id);
	printf ("[%d]", eItem->id);
	return 0;
}

void uidAdd (char* uid)
{
	int  ret;
	uidEntry* e;
	//printf ("-----------------------------------\n");
	
	e = uidEntry_create  (uid, vrandom(1000));
	vhashtable_lock (ht);
	ret = vhashtable_insert (ht, e->uid, e);
	if (ret==OKAY) {
		; //printf ("key=%s\n", e->uid);
	} else {
		uidEntry_destroy (e);
	}
	vhashtable_unlock (ht);
}

void uidDel (char* uid)
{
	vhashtable_lock (ht);
	vhashtable_delete (ht, uid);
	vhashtable_unlock (ht);
}

void uidFind (char* uid)
{
	uidEntry* e;
	e = vhashtable_find (ht, uid);
	if (e) {
		uidEntry_print (e);
	}
}

void* routine_add (void* arg)
{
	int i=0;
	char buf[100];
	for (i=0; i<10000; i++)
	{
		int r = vrandom(200);
		snprintf (buf, sizeof(buf), "%03d", r);
		uidAdd (buf);
		vmsleep (1);
	}

	return NULL;
}

void* routine_del (void* arg)
{
	int i=0;
	char buf[100];
	for (i=0; i<10000; i++)
	{
		int r = vrandom(200);
		snprintf (buf, sizeof(buf), "%03d", r);
		uidDel (buf);
		vmsleep (1);
	}

	return NULL;
}


int main (int argc, char* argv[])
{
	//ht = vhashtable_create  (5, hashcode, match, (dtor_ft)uidEntry_destroy);
	ht = vhashtable_create  (5, (dtor_ft)uidEntry_destroy);
	//vc_printMemUsage ();

#if 1
	const int NUM = 5;
	pthread_t tid[NUM];

	int i;
	for (i=0; i<NUM; i++)
	{
		if (i%2==0) {
			pthread_create (&tid[i], NULL, routine_add, NULL);
		}else{
			pthread_create (&tid[i], NULL, routine_del, NULL);
			//pthread_create (&tid[i], NULL, routine_add, NULL);
		}
	}
	for (i=0; i<NUM; i++)
	{
		pthread_join (tid[i], NULL);
	}
#endif
	


	//vhashtable_lock (ht);
	{
		
		printf ("-----------------------------------\n");
		
		vhashtable_traversal(ht, printHT);
		
		printf ("-----------------------------------\n");
		printf ("size=%d\n", vhashtable_getsize(ht));
		printf ("-----------------------------------\n");

#if 0
		uidAdd ("uid1");
		uidAdd ("uid2");
		uidAdd ("uid3");
		uidAdd ("uid4");
		uidDel ("uid4");
		uidDel ("uid4");
		uidDel ("uid4");
		uidFind ("uid1");
		uidFind ("uid2");
		uidFind ("uid1");
		uidFind ("uid2");
		uidFind ("uid3");
		uidFind ("uid3");
		uidFind ("uid2");
#endif

		printf ("-----------------------------------\n");
		
		int Bound = 500;
		vhashtable_filterANDdelete (ht, filter, &Bound);

		printf ("MAX=%d\n", Bound);
		printf ("-----------------------------------\n");
		printf ("size=%d\n", vhashtable_getsize(ht));
		printf ("-----------------------------------\n");
		
		vhashtable_traversal(ht, printHT);
	
	
		printf ("-----------------------------------\n");
	}
	//vhashtable_unlock (ht);

	vhashtable_destroy (ht);

	vc_printMemUsage ();


	return 0;
}



