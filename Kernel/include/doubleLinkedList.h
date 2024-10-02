#ifndef DOUBLELINKEDLIST_H
#define DOUBLELINKEDLIST_H

typedef struct Node Node;

typedef struct doubleLinkedListCDT *doubleLinkedListADT;

doubleLinkedListADT createDoubleLinkedListADT();
Node *addElement(doubleLinkedListADT list, void *data);
Node *addNode(doubleLinkedListADT list, Node *node);
Node *addNodeInFirst(doubleLinkedListADT list, Node *node);
void *removeNode(doubleLinkedListADT list, Node *node);
void *getFirstData(doubleLinkedListADT list);
int isEmpty(doubleLinkedListADT list);
void freeLinkedListADT(doubleLinkedListADT list);
int getSize(doubleLinkedListADT list);

void toBegin(doubleLinkedListADT list);
int hasNext(doubleLinkedListADT list);
void *nextInList(doubleLinkedListADT list);

#endif