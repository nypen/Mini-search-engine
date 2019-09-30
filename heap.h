#ifndef HEAP_H
#define HEAP_H

struct heapNode{
  float score;
  int id;
};
typedef struct heapNode heapNode;

struct Headlist{
  heapNode **indexes;     /*pointer to first node*/
  int capacity;   /*max capacity of the list*/
  int n;          /*number of elements in the list*/
};

typedef struct Headlist Headlist;

void HeapInit(Headlist *,int );
void HeapInsert(Headlist *, int , float ,int **);
void printHeap(Headlist *);
void UpdateHeap(Headlist * , int,float ,int **);
void ReplaceHeapRoot(Headlist *, int ,float ,int **);
void HeapFree(Headlist *);

#endif
