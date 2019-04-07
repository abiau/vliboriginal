#ifndef __TABLE_H__
#define __TABLE_H__
#ifdef __cplusplus
extern "C" {
#endif

#include "vutil.h"

/*******************  STRUCTURE  **********************/

typedef struct Node {
	struct Node* L; 
	struct Node* R; 
	void* arg;
} Node;

typedef struct List {
	int size;
	struct List* L;
	struct List* R;
	Node* mostL;
	Node* mostR;
	char key[32];
} List;

typedef struct Table {
	int size;
	List* mostL;
	List* mostR;
	List* cur;
	pthread_mutex_t mutex;
} Table;

/*******************  API  **********************/

Table* table_new();
void   table_del(Table* t);
void   table_put (Table* T, char* key, void* arg);
void   table_putBack (Table* T, char* key, void* arg);
void*  table_get (Table* T);
void   table_traversal (Table* T, todo_ft _pnt);
int    table_size (Table* T);
//static void table_lock (Table* T);
//static void table_unlock (Table* T);


#ifdef __cplusplus
}
#endif
#endif //__TABLE_H__


