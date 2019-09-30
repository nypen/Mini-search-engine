#ifndef INVINDEX_H
#define INVINDEX_H

/*struct for posting List*/
struct pList{
  int id;      /*text id*/
  int fr;      /*frequency of word in id text*/
  struct pList *next;
};

typedef struct pList pList;

struct InvertedIndex{
  int df;              /*frequency of word in all texts*/
  int nqi;             /*number of texts containing the word*/
  pList *last;
  pList *postingList;   /*pointer to pList*/
};

typedef struct InvertedIndex InvertedIndex;

void invIndex_Update(InvertedIndex **,int);
void invIndex_print(InvertedIndex *);
float IDF(int ,int );
#endif
