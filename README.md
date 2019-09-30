# Mini-search-engine
A mini search engine which uses inverted index structure to store a set of documents, answers queries with keywords and returns the top-k documents which are more relevant to the query.

Compilation : make
Execution : ./minisearch -i input_file -k K

Requests:
 /search q1 q2 q3 q4 … q10  
 User requests documents which contain those words. A score which describes the appearance frequency of these words is calculated and printed on the screen.
  

eg.
-/search brown lazy  
1.( 1)[0.0341] Quick brown foxes leaped over lazy dogs for fun
                       ^^^^^                   ^^^^
2.( 0)[0.0320] The quick brown fox leaped over the lazy lazy dog
                         ^^^^^                     ^^^^ ^^^^ 
                         
                         
-/df 
Prints the document frequency vector , every word that appears inside the documents with its frequency
eg.
/df 
the 1 
quick 1 
over 2 
leaped 2 
lazy 2 
fun 1 
foxes 1 
fox 1 
for 1 
dogs 1 
dog 1 
brown 2 
The 1 
eg2:
/df leaped 
leaped 2 

 
- /tf id word 
Prints word's term frequency in id document
eg:
/tf 0 lazy 
0 lazy 2 

- /exit  
exit of app 

