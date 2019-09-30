#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "trie.h"
#include "invindex.h"


void HeapInit(Headlist *heap,int capacity){
  heap->capacity = capacity;
  heap->n = 0;
  heap->indexes = malloc(sizeof(heapNode *)*capacity);
  int i;
  for(i=0;i<heap->capacity;i++){
    heap->indexes[i] = NULL;
  };
}

void HeapInsert(Headlist *heap , int id , float score , int **inheap ){
  /*inserts a new node to heap , and updates heap */
  int *inHeap = *inheap;
  int parent,child1,child2;
  heap->indexes[heap->n] = malloc(sizeof(heapNode));
  heap->indexes[heap->n]->id = id;
  heap->indexes[heap->n]->score = score;
  inHeap[id] = heap->n;
  heapNode *temp = NULL;
  heap->n++;
  int n = heap->n-1;
  int smallest_score;
  if(heap->n == 1){ return; }      //root has been inserted
  /*update x*/
  while(heap->indexes[n]->score < heap->indexes[n/2]->score && n>0){ /*while child fewer than parent => swap*/
    temp = heap->indexes[n];
    inHeap[heap->indexes[n]->id] = n/2;
    inHeap[heap->indexes[n/2]->id] = n;
    heap->indexes[n] = heap->indexes[n/2];
    heap->indexes[n/2] = temp;

    n = n/2;

  }
}

void UpdateHeap(Headlist *heap , int n,float score,int **inheap){
    /*updates the heap , when score at position n is updated , it can only get bigger
     so we have to scan this node and their children*/
     int *inHeap = *inheap;
     heapNode *temp = NULL;
     int parent = n;
     int child1 = n*2+1;
     int child2 = n*2 +2;
     int smallest_score = parent;
     heap->indexes[parent]->score = score;
     while(child1 < heap->n){     //while child node exists
       //if child is of smaller value , mark it as smallest
       if(heap->indexes[child1]->score < heap->indexes[parent]->score){
         smallest_score = child1;
       }
       //if(child2 >= heap->n) return;
       //if child2 is of smaller value ,mark it
       if(child2 < heap->n && heap->indexes[child2]->score < heap->indexes[smallest_score]->score){
         smallest_score = child2;
       }

       //if smallest has changed => swap smallest with parent
       if(smallest_score!=parent){
         /*update inheap*/
         inHeap[heap->indexes[parent]->id] = smallest_score;
         inHeap[heap->indexes[smallest_score]->id] = parent;
         /* swap positions*/
         temp = heap->indexes[smallest_score];
         heap->indexes[smallest_score] = heap->indexes[parent];
         heap->indexes[parent] = temp;

       }else{ //if no changes made
         return;
       }
       //change parent to smallest
       parent = smallest_score;
       child1 = parent*2+1;
       child2 = parent*2+2;
     }
}

void ReplaceHeapRoot(Headlist *heap , int id ,float score,int **inheap){
  /*replaces first element of heap , which is the minimum ,with a new one*/
  int *inHeap = *inheap;
  inHeap[heap->indexes[0]->id] = -1;  //this text is removed from heap
  heap->indexes[0]->id = id;
  inHeap[id]=0;
  heap->indexes[0]->score = score;
  /*update heap from node 0*/
  UpdateHeap(heap,0,score,inheap);
}

void printHeap(Headlist *heap){
  int i;
  if( heap == NULL ) return;
  for(i=0;i<heap->n;i++){
    if(heap->indexes[i]!=NULL)
      printf("%d - %f->",heap->indexes[i]->id,heap->indexes[i]->score );
  }
  printf("|\n");
}

void HeapFree(Headlist *heap){
  for(int i=0;i<heap->n;i++){
    free(heap->indexes[i]);
  }
  free(heap->indexes);
}
