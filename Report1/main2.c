#include <stdio.h>
#include <stdlib.h>
#include "alloc2.h"


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

/*I think this has overridden error method */
void error(void)
{
  printf("fail\n");
  exit(1);
}

int alloc1000x1000(void *table[])
{
  int i;
  for (i = 0; i < 1000; i++) {
    table[i] = alloc2(1000);
    if (table[i] == NULL) {
      return -1;
    }
  }
  return 0;
}

void free1000x1000(void *table[])
{
  int i;

  for (i = 0; i < 1000; i++) {
    afree2(table[1000 - i - 1]);
  }
}

/* Free memory FIFO (i.e. no LIFO) */
void free_FIFO(void *table[])
{
  int i;
  for (i = 0; i < 1000; i++){
    afree2(table[i]);
  }
}

int main(void)
{
  int i, j;
  void *p;
  void *table[1000];
  int nunits = 1 + (1000 + sizeof(HEADER) -1) / sizeof(HEADER); /* Number of blocks when 1000 bytes applied */
  int x = nunits * (int)sizeof(HEADER); /* Memory size of nunits blocks */

  /* テスト1: 上限を越えない範囲で割り付け・解放を多数回繰り返しても失敗しない． */
  printf("test1: "); fflush(stdout);
  for (i = 0; i < 1000; i++ ) {
    if (alloc1000x1000(table) < 0) {
      error();
    }
    free1000x1000(table);
  }
  printf("passed.\n");

 /*テスト２：LIFO順でない割り付け・解放を多数回繰り返しても失敗しない.  */
  printf("test2: "); fflush(stdout);
  for (i = 0; i < 1000; i++ ) {
    if (alloc1000x1000(table) < 0) {
      error();
    }
    free_FIFO(table);
  }
  printf("passed.\n");

  /* テスト3: 上限を越えた割り付けを行なうと，失敗する． */
  printf("test3: "); fflush(stdout);
  if (alloc1000x1000(table) < 0) {
    error();
  }
  p = alloc2(ALLOCSIZE / sizeof(HEADER));		/* should fail */
  if (p != NULL) {
    error();
  }
  free1000x1000(table);
  printf("passed.\n");

  /* テスト4: 割り付けを受けて，まだ解放されていない領域は，互いに重なっていない． */
  printf("test4: "); fflush(stdout);
  if (alloc1000x1000(table) < 0) {
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
  free1000x1000(table);
  printf("passed.\n");

  return 0;
}
