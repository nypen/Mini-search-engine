#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "trie.h"
#include "invindex.h"
#include "heap.h"
#include <sys/ioctl.h>

#define TRUE 1
#define FALSE 0

#define k1 1.2
#define b 0.75

void Trie_Init(Trie *trie){
  trie->noFiles = 0;
  trie->root = NULL;
}

void Trie_Insert(trieNode **root,char *word,int id){
  /***************************************************
   * input : root of trie and word to be inserted    *
   * function : insert each letter to the right list *
   ***************************************************/
  int i;
  trieNode *temp1 = NULL;
  trieNode *headnode = *root;
  temp1 = List_insert(root,word[0]); //insert the first letter at list of first letters
  for(i=1;i<strlen(word);i++){
    //temp1 = Node for letter word[i]
    temp1 = List_insert(&(temp1->rest),word[i]); //add to this list the i-th letter
  }
  if(temp1->invIndex == NULL ){
    temp1->invIndex = malloc(sizeof(InvertedIndex));
    temp1->invIndex->df=0;
    temp1->invIndex->postingList = NULL;
  }
  invIndex_Update(&(temp1->invIndex),id);
}

trieNode *List_insert(trieNode **head,char letter){
  /************************************************
  *      inserts a letter into the list           *
  *  1.if the letter exists already in the list , *
  *    a pointer to this node is returned         *
  * 2.else the new node is being inserted and     *
  *     returned                                  *
  ************************************************/
  trieNode *tempNode = NULL;
  trieNode *current = *head;
  trieNode *prev = NULL;

  if((*head)==NULL || letter<(*head)->c){
    //if its the first element of the list or
    //letter is of smaller value than head of the list,insert at start
    tempNode = *head;
    *head = malloc(sizeof(trieNode));
    (*head)->c = letter;
    (*head)->otherLetter = tempNode;
    (*head)->rest = NULL;
    (*head)->invIndex = NULL;
    return(*head);
  }else{
    /* else find the right place for the letter */
    while( current !=NULL && current->c<=letter){
      if(current->c == letter){   //if letter is in list , return the node
        return current;
      }
      prev = current;                 //else go to the next node
      current = current->otherLetter;
    }
    /*initialize the Node that will be inserted*/
    tempNode = malloc(sizeof(trieNode));
    tempNode->c = letter;
    tempNode->otherLetter = NULL;
    tempNode->rest = NULL;
    tempNode->invIndex = NULL;
    if(current != NULL){   //new node has to be inserted between previous and current node
      prev->otherLetter = tempNode;         //link prev->temp->curr
      tempNode->otherLetter = current;
    }else{            //insert at end
        prev->otherLetter = tempNode;
    }
    return(tempNode);
  }
}

InvertedIndex *Trie_Search(trieNode *head , char *word){
  int i;
  int not_found = TRUE;
  trieNode *temp = head;
  //gia kathe gramma to brisko sti lista kai proxorao sto paidi tis
  for(i=0;i<strlen(word);i++){
    temp  = inList(temp,word[i]);
    if(temp==NULL){
      return NULL;
    }
    if(i<strlen(word)-1){       //letter found in list
        temp = temp->rest;
    }

  }
  return(temp->invIndex);
}

trieNode *inList(trieNode *list,char letter){
  /****************************************************
   * input : 1st node of list , letter to be found    *
   * function : search the list until letter is found *
   * return : pointer to the node with letter         *
   *            if not found return NULL              *
   ****************************************************/

  trieNode *temp = list;
  while(temp!=NULL && temp->c!=letter){
    temp = temp->otherLetter;
  }
  return temp;
}

void printList(trieNode *list){
  while(list!=NULL){
    printf("%c->",list->c );
    list = list->otherLetter;
  }
  printf("NULL\n");
}
void printTree(trieNode *root,int longest_word,int level,char *word){
    int i ;

    if(root == NULL) return;
    if(root->invIndex!=NULL){     /*if this  node completes a word print inverted index*/
      word[level-1]=root->c;
      word[level]=0;
      printf("%s :",word );
      invIndex_print(root->invIndex);
    }

    if(root->rest!=NULL){
      word[level-1]=root->c;
      word[level]=0;
      printTree(root->rest,longest_word,level+1,word);

    }

    if(root->otherLetter!=NULL){
      //change the last letter to the next possible
      word[level-1]=0;
      printTree(root->otherLetter,longest_word,level,word);
    }
}



void Trie_SearchResults(Trie trie,char **query,int noQuery ,int avgdl ,int maxResults,textInfo** texts ){
  InvertedIndex *result_invindex = NULL;
  pList *postingList;
  int i;
  int *inHeap= malloc(sizeof(int)*trie.noFiles);     //array that indicates if text[id] is in heap
  float scores[trie.noFiles],score;
  /*initiate inHeap with -1*/
  for(i=0;i<trie.noFiles;i++){
    inHeap[i] = -1;
    scores[i] = 0;
  }
  Headlist maxheap;
  HeapInit(&maxheap,maxResults);
  int pos;

  for(i=0;i<noQuery;i++){                       //for every word in the query
    result_invindex = Trie_Search(trie.root , query[i]);    //find the posting list in trie
    if(result_invindex!=NULL){
      postingList = result_invindex->postingList;
      while(postingList!=NULL){       //add scores for this query to every id in postingList
        scores[postingList->id] += IDF(trie.noFiles,result_invindex->nqi) * postingList->fr*(k1+1)/(postingList->fr+k1*(1-b+b*texts[postingList->id]->D/avgdl)); //ypologismos score
        if(inHeap[postingList->id]>-1){   //if text id is inheap

            pos = inHeap[postingList->id];  //position of id in heap
            (maxheap.indexes[pos])->score = scores[postingList->id];  //update score in heap
            //and update the heap from this position and after
            UpdateHeap(&maxheap , pos , scores[postingList->id] , &inHeap  );

        }else{  //if text id not in heap
          if(maxheap.n < maxheap.capacity){     //if heap not full
            HeapInsert(&maxheap, postingList->id , scores[postingList->id] , &inHeap);
          }else if((maxheap.indexes[0])->score < scores[i] ){   //if heap is full and this score is bigger than the minimum
            ReplaceHeapRoot(&maxheap,postingList->id,score,&inHeap);
          }//else go to next
        }

        postingList = postingList->next;
      }
    }
  }
  if(maxheap.n==0) printf("No results for this search.Try something else\n");
  printResults(&maxheap,texts,query,noQuery);
  free(inHeap);
  HeapFree(&maxheap);
}

void printResults(Headlist *heap  , textInfo **array , char **query , int noQuery){
  int i , format_length = 25 , j;
  struct winsize w;
  int line_length ,stdline_length,marking = 0;;
  char *token = NULL;
  char *line = NULL;
  char *mark_line = NULL;
  char mark = '^';
  char space = ' ';
  ioctl(0,TIOCGWINSZ,&w);

  if( heap == NULL ) return;
  line_length = w.ws_col - format_length ;
  stdline_length = w.ws_col - format_length ;
  mark_line = malloc(sizeof(char)*(line_length+1));
  memset(mark_line,space,stdline_length);         //initialize a string with spaces
  for(i=heap->n-1;i>=0;i--){              //1st node of the heap has the minimum score , so print from end
      marking = 0;
      line = malloc(sizeof(char)*strlen(array[heap->indexes[i]->id]->text)+1);
      strcpy(line,array[heap->indexes[i]->id]->text);
      token = strtok(line , " ");
      //print info of the text
      printf("%03d. (%05d) [%09.6lf] ",heap->n-i,heap->indexes[i]->id,heap->indexes[i]->score );
      while(token!=NULL){
        if(strlen(token) < line_length){  //if remaing line is bigger than the word to be printed
            printf("%s ",token );         //print it
            if(query_word(token,query,noQuery)){  //if word is in query's words add marking
              for(j=stdline_length-line_length;j<stdline_length-line_length+strlen(token);j++){
                  mark_line[j]= mark;
              }
              marking = 1;
            }
            line_length -= strlen(token)+1;   //decrease the remaing line by the length of the word and the space
        }else{    //if remaing line is smaller than the word to be printed
          printf("\n");       //change line
          print_spaces(format_length);    //print spaces
          if(marking==1){                 //print the marking
            printf("%s\n",mark_line );
            print_spaces(format_length);
            marking = 0;
          }
          printf("%s ",token );
          line_length = stdline_length ;      //re-initialize the remaining line length to max
          memset(mark_line,space,stdline_length);
          if(query_word(token,query,noQuery)){  //if word is in query's words add marking
            marking = 1;
            for(j=stdline_length-line_length;j<stdline_length-line_length+strlen(token);j++){
              mark_line[j]= mark;
            }
          }
          line_length -= strlen(token)+1;   //decrese the remaining length by the length of word and space
        }

        token = strtok(NULL , " ");
      }
      line_length =  w.ws_col - format_length;
      printf("\n\n");
  }
  free(mark_line);
}

void TrieTermFrequency(Trie trie,char *term,int id){
  /**************************************************
   *   finds the word in the trie ,                 *
   *   prints the frequency of the term in id text  *
   *   if it doesn't exist it prints message        *
   **************************************************/

  InvertedIndex *invindex = Trie_Search(trie.root , term);
  pList *postinglist = NULL;
  if(invindex== NULL){
    printf("%s Not found in trie.\n",term );
  }else{
    postinglist = invindex->postingList;
    while(postinglist!=NULL && postinglist->id<id){
      postinglist = postinglist->next;
    }
    if(postinglist->id == id){
      printf("%d %s %d\n",id,term,postinglist->fr );
    }else{
      printf("'%s' doesn't exist in text with id %d \n",term,id );
    }
  }
}

int query_word(char *word,char **query,int noQuery){
  /* returns 1 if word is a word of the query
    else returns 0  */
  int i ;
  for(i=0;i<noQuery;i++){
    if(strcmp(word,query[i])==0){
      return 1;
    }
  }
  return 0;
}


void print_spaces(int n){
  for(int i=0;i<n;i++){
    printf(" ");
  }
}

void postingListFree(InvertedIndex *inv){
  if(inv == NULL) return;
  pList *current = inv->postingList;
  pList *next = current->next;
  while(current!=NULL){   /*nodes of list are being freed from the start , keeping the next node*/
    free(current);
    current = next;
    if(current!=NULL) next = next->next;
  }
}
void TrieFree(trieNode *node ){
  /*it goes recursively at the leaf nodes of the trie , and frees the nodes from bottom to top */
  if(node == NULL )return;
  if(node->invIndex!=NULL){ //if there is inverted index free it
    postingListFree(node->invIndex);
    free(node->invIndex);
  }

  TrieFree(node->rest);
  TrieFree(node->otherLetter);

  free(node);


}
