#include <stdio.h>
#include <stdlib.h>
#include "alloc3.h"


/*Define union header here to get sizeof(Header) */
typedef double ALIGN;

union header {
  struct h {
    union header *ptr;
    int size;
  } s;
  ALIGN x;
};

typedef union header HEADER;

extern int realbytes;

extern HEADER *ap;

/*I think this has overridden error method */
void error(void)
{
  printf("fail\n");
  exit(1);
}

int alloc1000x10000(void *table[])
{
  int i;
  for (i = 0; i < 1000; i++) {
    table[i] = alloc3(10000);
    if (table[i] == NULL) {
      return -1;
    }
  }
  return 0;
}

void free1000x10000(void *table[])
{
  int i;

  for (i = 0; i < 1000; i++) {
    afree3(table[1000 - i - 1]);
  }
}

/* Free memory FIFO (i.e. no LIFO) */
void free_FIFO(void *table[])
{
  int i;
  for (i = 0; i < 1000; i++){
    afree3(table[i]);
  }
}

int main(void)
{
  int i, j;
  void *table[1000];
  void *q;
  HEADER *p;
  int nunits = 1 + (10000 + sizeof(HEADER) -1) / sizeof(HEADER); /* Number of blocks when 10000 bytes applied */
  int x = nunits * (int)sizeof(HEADER); 			/* Memory size of nunits blocks */

  /* テスト3： 割り付けしたメモリ領域全体に値を書き込んでもエラーにならない．*/
  printf("test3: "); fflush(stdout);
  q = alloc3(1000);
  if(q == NULL){
    error();
  }
  p = ap;   /* Pointer to the header of allocted memeory block */
 for(i = 1; i < p ->s.size; i++){   /* Write from header to tail */
    (p+i)->x = 10;
  }
  afree3(q);
  printf("passed. \n");

 /* テスト1：LIFO順でない割り付け・解放を多数回繰り返しても失敗しない.  */
  printf("test1: "); fflush(stdout);
  for (i = 0; i < 1000; i++ ) {
    if (alloc1000x10000(table) < 0) {
      error();
    }
    free_FIFO(table);
  }
  printf("passed.\n");

 /* テスト2：合計で数MB程度の領域を割り付けてもエラーにならない． */
   printf("test2: "); fflush(stdout);
   if((q = alloc3(10 * 1024 * 1024)) == NULL){    /* Allocate 10MB */
        error();
   }
   afree3(q);
   printf("passed. \n");




  /* テスト4: 割り付けを受けて，まだ解放されていない領域は，互いに重なっていない． */
  printf("test4: "); fflush(stdout);
  if (alloc1000x10000(table) < 0) {
    error();
  }
  for (i = 0; i < 1000; i++) {
    for (j = 0; j < 1000; j++) {
      if (i != j) {
	int distance = table[i] - table[j];
	if (distance < 0) {
	  distance = -distance;
	}
	if (distance < x) {
	  printf("distance(%d, %d) = %d\n", i, j, distance);
	  error();
	}
      }
    }
  }
  free1000x10000(table);
  printf("passed.\n");

  return 0;
}
