#include <stdio.h>
#include <stdlib.h>
#include "trie.h"
#include <string.h>
#include "invindex.h"


#define MAX_ARGS 10


int main(int argc , char **argv){

    if(argc!=5) {
      printf("Please execute the program as ./minisearch -i docfile -k K\n");
      return -1;
    }

    FILE *file;
    int textLength  , i,totalWords=0 , longest_word = 0;
    char c;
    int K ;
    textInfo **textArray = NULL;
    char *line = NULL;
    char *words = NULL;
    size_t  buf ;
    Trie trie;
    Trie_Init(&trie);
    /*  opening file  */
    if(strcmp(argv[1],"-i") == 0){
        file = fopen(argv[2],"r");
        if(strcmp(argv[3],"-k") == 0){
           K = atoi(argv[4]);
        }else{
          printf("Please execute the program as ./minisearch -i docfile -k K\n");
          return -1;
        }
    }else if(strcmp(argv[3],"-i") == 0){
      file = fopen(argv[4],"r");
      if(strcmp(argv[1],"-k") == 0){
         K = atoi(argv[2]);
      }else{
        printf("Please execute the program as ./minisearch -i docfile -k K\n");
        return -1;
      }
    }else{
      printf("Please execute the program as ./minisearch -i docfile -k K\n");
      return -1;
    }
    if(file == NULL){
      printf("Error opening the file %s\n",argv[2]);
      return -1;
    }

    /* count lines of file */
    while((c = getc(file))!=EOF){
      if(c=='\n')  trie.noFiles++;
    }
    rewind(file);   //go at the start of the file

    /* allocating array */
    textArray = malloc(sizeof(textInfo *)*trie.noFiles);
    if(textArray == NULL){
      printf("Error while allocating memory");
      return -1;
    }
    for(i=0;i<trie.noFiles;i++){
      textArray[i] = malloc(sizeof(textInfo));
      if(textArray[i] == NULL){
        printf("Error while allocating memory");
        return -1;
      }
    }

    /*reading file line by line and storing to the array*/
    for(i=0;i<trie.noFiles;i++){
      if((textLength = getline(&line,&buf,file))!=-1){
        textArray[i]->text = malloc(sizeof(char)*(textLength-sizeof(char)*2));
        /*seperating string to id and text excluding character \n*/
        sscanf(line,"%d %[^\n]s",&textArray[i]->id,textArray[i]->text);
        if(textArray[i]->id != i ){
          printf("%d -- %d\n",i , textArray[i]->id );
          printf("Input file has wrong format . Id's not valid\n");
          return -1;
        }
        textArray[i]->D = 0;  /*zero the counter of text words*/
      }else{
        printf("Error during reading file\n");
        return -1;
      }
    }
    fclose(file);
    free(line);
    line = NULL;
    for(i=0;i<trie.noFiles;i++){
      line = malloc(strlen(textArray[i]->text)+1);
      strcpy(line,textArray[i]->text);
      words = strtok(line," ");
      while(words!=NULL){
        textArray[i]->D++;
        if(strlen(words)>longest_word){
          longest_word = strlen(words);
        }
        Trie_Insert(&(trie.root),words,textArray[i]->id);
        words = strtok(NULL , " ");

      }
      totalWords+= textArray[i]->D;
      free(line);
      line = NULL;
    }


    char **query;
    char *input=NULL;
    printf("Data has been stored in memory.\n");
    int length,args;
    line = NULL;
    /*read input and store it to a second string variable */
    while(1){
      printf("\nType a command:\n");
      printf("\t1. /search q1,q2,..,q10  : to search for q1,q2,.. in the texts.For more than 10 words the first 10 will be searched\n");
      printf("\t2. /df   : to print the frequency of each word in all documents\n" );
      printf("\t3. /df  q : to print the frequency of word q in all documents\n" );
      printf("\t4. /tf id q : to print the frequency of word q in id document\n" );
      printf("\t5. /exit : to exit the application \n" );
      printf("\tWhere q,q1,q2,..,q10 are words\n");
      printf("Waiting for your command: " );
      getline(&input,&buf,stdin);
      if(input[strlen(input)-1]=='\n') input[strlen(input)-1]='\0'; //excluding \n character
      line = malloc(sizeof(char)*(strlen(input)+1));
      strcpy(line,input);

      words=strtok(input," ");
      if(strcmp("/search",words)== 0){
        args=-1; /*number of arguments for the query , starts from -1 because search will be counted*/
        while(words!=NULL){
          args++;
          words = strtok(NULL , " ");
        }
        /*splitting input tokens again to store them in the array*/
        if(args>MAX_ARGS) args = MAX_ARGS; //max words for search be 10;
        query = malloc(sizeof(char *)*args);
        words=strtok(line," ");
        words=strtok(NULL," "); //ignore first word
        i=0;
        /*allocate and fill the array with the search words*/
        while(i<args && words!=NULL){
          query[i]=NULL;
          query[i]=malloc(sizeof(char)*strlen(words)+1);
          strcpy(query[i],words);
          words=strtok(NULL," ");
          i++;
        }

        Trie_SearchResults(trie,query,args,totalWords/(float)trie.noFiles,K,textArray);
        for(i=0;i<args;i++){
          free(query[i]);
        }
        free(query);
      }else if(strcmp("/tf",words)== 0){
        int id;
        char *term = strtok(NULL," ");     //term = word to be searched
        if(words == NULL){
          printf("Wrong command.Try again\n");
          continue;
        }
        words = strtok(NULL," ");   //id = id
        if(words == NULL){
          printf("Wrong command.Try again\n");
          continue;
        }
        id = atoi(words);
        TrieTermFrequency(trie,term,id);
      }else if(strcmp("/df",words)== 0){
        char word[longest_word];
        words = strtok(NULL, " ");
        if(words==NULL){
          printTree(trie.root,longest_word,1,word);
        }else{
          InvertedIndex *invindex = Trie_Search(trie.root , words);
          if(invindex!=NULL)
            printf("%s  %d\n",words,invindex->df );
          else
            printf("There is no word %s\n",words );
        }
      }else if(strcmp("/exit",words)== 0){
        for(i=0;i<trie.noFiles;i++){
          free(textArray[i]->text);
          free(textArray[i]);
        }
        free(textArray);
        TrieFree(trie.root );
        printf("Bye\n");
        exit(1);
        //return -1;
      }else{
        printf("There is not such command command. Type one of the follownigs: \n");
      }
    }





}
