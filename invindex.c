#include "invindex.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

void invIndex_Update(InvertedIndex **invIndex,int id){
  /******************************************************************
   *     input : double pointer to the head of the posting list,    *
   *          id of the text                                        *
   *     function : posting list of the word is updated             *
   *     if inverted index is empty -> initiate it                  *
   *   if text with id #id not in posting list add it at the end    *
   *          if it exists , update frequency                       *
   ******************************************************************/
  pList *current = NULL;
  pList *previous = NULL;
  if((*invIndex)->df == 0){    /*inverted index is empty , create inverted index*/
    (*invIndex)->df=1;
    (*invIndex)->nqi=1;
    (*invIndex)->postingList = malloc(sizeof(pList));
    (*invIndex)->postingList->id = id;
    (*invIndex)->postingList->fr = 1;
    (*invIndex)->postingList->next = NULL;
    (*invIndex)->last = (*invIndex)->postingList;
  }else{
    (*invIndex)->df++;    /*increase df*/
    current = (*invIndex)->last;    /*go to the last node of the posting list*/

    //while(current!=NULL && current->id!=id){ /*search for the node with id #id*/
    //  previous = current;
    //  current = current->next;
    //}
    //if(current == NULL){          /*if node with id #id doesnt exist , insert at end and initiate*/
    //  previous->next = malloc(sizeof(pList));
    //  previous->next->id = id;
    //  previous->next->fr = 1;
    //  previous->next->next = NULL;
    //  (*invIndex)->nqi++;         //new text , nqi increased
    if(current->id<id){     /*if id is bigger than last id make a next new node*/
                            /*and initiate it*/
      current->next = malloc(sizeof(pList));
      current->next->id = id;
      current->next->fr=1;
      current->next->next = NULL;
      (*invIndex)->last = current->next;
    }else{          /*if node exists , id found in posting list , frequency is increased*/
      current->fr++;
    }
  }
}

void invIndex_print(InvertedIndex *invindex){
  if(invindex==NULL) return;
  pList * postList = invindex->postingList;
  while(postList!=NULL){
    printf("[%d,%d],",postList->id,postList->fr );
    postList = postList->next;
  }
  printf("\n");

}

float IDF(int N,int nqi){
  return(log2((N-nqi-0.5)/(nqi+0.5)));
}
