
#ifndef TRIE_H
#define TRIE_H
#include "invindex.h"
#include "heap.h"

typedef struct{
  int id;   /*text id*/
  char *text;
  int D;    /*words of text*/
}textInfo;


struct trieNode{
  char c;             /* Letter store in this node*/
  struct trieNode *otherLetter;   /* Pointer to other letters that have the same prefix*/
  struct trieNode *rest;         /* Pointer to words that have the letter c */
  InvertedIndex *invIndex;     /* Inverted index for word with final letter the letter c*/
};

typedef struct trieNode trieNode;

struct Trie{
  trieNode *root;
  int noFiles;
};

typedef struct Trie Trie;


void init_List(Headlist *,int );
void Trie_Init(Trie *);
trieNode *List_insert(trieNode **,char );
void printList(trieNode *);
void Trie_Insert(trieNode **,char *,int);
trieNode *inList(trieNode *,char);
InvertedIndex *Trie_Search(trieNode * , char *);
void printTree(trieNode *,int ,int,char *);
void Trie_SearchResults(Trie ,char **,int,int,int,textInfo**);
void printResults(Headlist *heap  , textInfo **array,char **,int);
void TrieTermFrequency(Trie trie,char *term,int id);
void print_spaces(int n);
char *fill_with_spaces(int n);
int query_word(char *word,char **query,int noQuery);
void TrieFree(trieNode *root );
#endif
