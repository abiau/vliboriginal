#include "table.h"

Node* node_new(void* arg)
{
	Node* n = (Node*) vc_malloc(sizeof(Node));
	n->arg = arg;
	return n;
}

void node_del(Node* n)
{
	vc_free(n, sizeof(Node));
}

List* list_new(char* key)
{
	List* l = (List*) vc_malloc(sizeof(List));
	snprintf(l->key, sizeof(l->key), "%s", key);
	return l;
}

void list_del(List* l)
{
	vc_free(l, sizeof(List));
}

/***********************************************/
//  list;
/***********************************************/

void* list_pop(List* l, Node* n)
{
	if (!n)
	{
		return NULL;
	}
	else if (!n->L && !n->R)
	{
		l->mostL = NULL;
		l->mostR = NULL;
	}
	else if (n->L && !n->R)
	{
		Node* pivot = n->L;
		pivot->R = NULL;
		l->mostR = pivot;
	}
	else if (!n->L && n->R)
	{
		Node* pivot = n->R;
		pivot->L = NULL;
		l->mostL = pivot;
	}
	else if (n->L && n->R)
	{
		Node* pL = n->L;
		Node* pR = n->R;
		pL->R = pR;
		pR->L = pL;
	}

	n->L = NULL;
	n->R = NULL;
	void* arg = n->arg;
	node_del(n);
	(l->size)--;
	return arg;
}

void list_pushL(List* l, Node* pivot, Node* n)
{
	if (!pivot)
	{
		l->mostL = n;
		l->mostR = n;
	}
	else if (pivot && pivot->L)
	{
		Node* pL = pivot->L;
		Node* pR = pivot;

		n->L = pL;
		n->R = pR;
		pL->R = n;
		pR->L = n;
	}
	else if (pivot && !pivot->L)
	{
		Node* pL = pivot->L;
		Node* pR = pivot;
		
		n->L = pL;
		n->R = pR;
		pR->L = n;
		l->mostL = n;
	}
	(l->size)++;
}

void list_pushR(List* l, Node* pivot, Node* n)
{
	if (!pivot)
	{
		l->mostL = n;
		l->mostR = n;
	}
	else if (pivot && pivot->R)
	{
		Node* pL = pivot;
		Node* pR = pivot->R;

		n->L = pL;
		n->R = pR;
		pL->R = n;
		pR->L = n;
	}
	else if (pivot && !pivot->R)
	{
		Node* pL = pivot;
		Node* pR = pivot->R;
		
		n->L = pL;
		n->R = pR;
		pL->R = n;
		l->mostR = n;
	}
	(l->size)++;
}

/***********************************************/
//  table;
/***********************************************/
void* table_pop(Table* t, List* l)
{
	if (!l)
	{
		return NULL;
	}
	else if (!l->L && !l->R)
	{
		t->mostL = NULL;
		t->mostR = NULL;
	}
	else if (l->L && !l->R)
	{
		List* pivot = l->L;
		pivot->R = NULL;
		t->mostR = pivot;
	}
	else if (!l->L && l->R)
	{
		List* pivot = l->R;
		pivot->L = NULL;
		t->mostL = pivot;
	}
	else if (l->L && l->R)
	{
		List* pL = l->L;
		List* pR = l->R;
		pL->R = pR;
		pR->L = pL;
	}

	l->L = NULL;
	l->R = NULL;

	/******************************************/
	//TODO
	/******************************************/
	(t->size)--;
	list_del(l);
	return NULL;
}

void table_pushL(Table* t, List* pivot, List* l)
{
	if (!pivot)
	{
		t->mostL = l;
		t->mostR = l;
	}
	else if (pivot && pivot->L)
	{
		List* pL = pivot->L;
		List* pR = pivot;

		l->L = pL;
		l->R = pR;
		pL->R = l;
		pR->L = l;
	}
	else if (pivot && !pivot->L)
	{
		List* pL = pivot->L;
		List* pR = pivot;
		
		l->L = pL;
		l->R = pR;
		pR->L = l;
		t->mostL = l;
	}
	(t->size)++;
}

void table_pushR(Table* t, List* pivot, List* l)
{
	if (!pivot)
	{
		t->mostL = l;
		t->mostR = l;
	}
	else if (pivot && pivot->R)
	{
		List* pL = pivot;
		List* pR = pivot->R;

		l->L = pL;
		l->R = pR;
		pL->R = l;
		pR->L = l;
	}
	else if (pivot && !pivot->R)
	{
		List* pL = pivot;
		List* pR = pivot->R;
		
		l->L = pL;
		l->R = pR;
		pL->R = l;
		t->mostR = l;
	}
	(t->size)++;
}

List* table_findList_byKey(Table* t, char* key)
{
	List* l = t->mostL; 
	while (l)
	{
		if (strcmp(l->key, key)==0)
			break;
		l = l->R;
	}
	return l;
}

List* table_setCursor(Table* t, List* l)
{
	t->cur = l;
	return l;
}







/***********************************************/
//  TABLE DATA STRUCTURE API;
/***********************************************/

Table* table_new()
{
	Table* t = (Table*) vc_malloc(sizeof(Table));
	pthread_mutex_init (&t->mutex, NULL);
	return t;
}

void table_del(Table* t)
{
	vc_free(t, sizeof(Table));
}

int table_size (Table* T)
{
	int sum=0;
	List* L = T->mostL;
	while (L)
	{
		sum = sum + L->size;
		L = L->R;
	}
	return sum;
}

static void table_lock (Table* T)
{
	pthread_mutex_lock (&T->mutex);
}

static void table_unlock (Table* T)
{
	pthread_mutex_unlock (&T->mutex);
}

void table_put (Table* T, char* key, void* arg)
{
	table_lock(T);
	{
		List* L = table_findList_byKey(T, key);
		if (!L)
		{
			L = list_new(key);
			table_pushR(T, T->mostR, L);
		}

		Node* N = node_new(arg);
		list_pushR(L, L->mostR, N);
	}
	table_unlock(T);
}

void table_putBack (Table* T, char* key, void* arg)
{
	table_lock(T);
	{
		List* L = table_findList_byKey(T, key);
		if (!L)
		{
			L = list_new(key);
			table_pushR(T, T->mostR, L);
		}

		Node* N = node_new(arg);
		list_pushL(L, L->mostL, N);
	}
	table_unlock(T);
}

void* table_get (Table* T)
{
	void* arg = NULL;
	table_lock(T);
	{
		List* L = T->cur;
		do {
			if (!L)
			{
				table_setCursor(T, T->mostL);
				break;
			}

			arg = list_pop(L, L->mostL);
			table_setCursor(T, L->R);
			if (!L->mostL)
			{
				table_pop(T, L);
			}
		}while(0);
	}
	table_unlock(T);
	return arg;
}


void table_traversal (Table* T, todo_ft _pnt)
{
	table_lock(T);
	{
		List* L = T->mostL;
		while (L)
		{
			Node* N = L->mostL;
			printf ("[key:%s]=>  ", L->key);
			while (N)
			{
				_pnt(N->arg);
				N = N->R;
			}
			printf ("\n");
			L = L->R;
		}
	}
	table_unlock(T);
}


