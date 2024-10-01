#include <doubleLinkedList.h>
#include <memoryManager.h>

typedef struct Node {
	void *data;
	struct Node *prev;
	struct Node *next;
} Node;

typedef struct doubleLinkedListCDT {
	Node *head;
	Node *tail;
	Node *current;
	int size;
} doubleLinkedListCDT;

doubleLinkedListADT createDoubleLinkedListADT() {
	doubleLinkedListADT list = (doubleLinkedListADT) malloc(sizeof(doubleLinkedListCDT));
	if (list) {
		list->head = NULL;
		list->tail = NULL;
		list->current = NULL;
		list->size = 0;
	}
	return list;
}

Node *addElement(doubleLinkedListADT list, void *data) {
	if (list == NULL)
		return NULL;
	Node *newNode = (Node *) malloc(sizeof(Node));
	if (newNode == NULL)
		return NULL;
	newNode->data = data;
	return addNode(list, newNode);
}

Node *addNode(doubleLinkedListADT list, Node *node) {
	if (list == NULL)
		return NULL;
	if (node == NULL)
		return NULL;
	node->next = NULL;
	if (list->size > 0) {
		list->tail->next = node;
	}
	else {
		list->head = node;
	}
	node->prev = list->tail;
	list->tail = node;
	list->size++;
	return node;
}

Node *addNodeInFirst(doubleLinkedListADT list, Node *node) {
	if (list == NULL)
		return NULL;
	if (node == NULL)
		return NULL;
	node->prev = NULL;
	if (list->size > 0) {
		list->head->prev = node;
	}
	else {
		list->tail = node;
	}
	node->next = list->head;
	list->head = node;
	list->size++;
	return node;
}

void *removeNode(doubleLinkedListADT list, Node *node) {
	if (node == NULL || list == NULL)
		return NULL;

	if (list->head == node) {
		list->head = node->next;
	}
	else {
		node->prev->next = node->next;
	}
	if (list->tail == node) {
		list->tail = node->prev;
	}
	else {
		node->next->prev = node->prev;
	}
	list->size--;
	void *data = node->data;
	node->next = NULL;
	node->prev = NULL;
	// free?
	return data;
}

Node *getFirst(doubleLinkedListADT list) {
	if (list == NULL)
		return NULL;
	return list->head;
}

void freeLinkedListADT(doubleLinkedListADT list) {
	Node *current = list->head;
	Node *next;
	while (current) {
		next = current->next;
		free(current);
		current = next;
	}
	free(list);
}

int getSize(doubleLinkedListADT list) {
	if (list == NULL)
		return -1;
	return list->size;
}

int isEmpty(doubleLinkedListADT list) {
	if (list == NULL)
		return -1;
	return list->size == 0;
}

void toBegin(doubleLinkedListADT list) {
	if (list == NULL)
		return;
	list->current = list->head;
}

int hasNext(doubleLinkedListADT list) {
	if (list == NULL)
		return -1;
	return list->current != NULL;
}

void *nextInList(doubleLinkedListADT list) {
	if (!hasNext(list))
		return NULL;
	void *data = list->current->data;
	list->current = list->current->next;
	return data;
}
