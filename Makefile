main: trie.c  minisearch.c
	gcc -g3 -o ./minisearch trie.c minisearch.c invindex.c heap.c -lm -I.
