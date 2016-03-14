#ifndef HEAP_H_
#define HEAP_H_

#include "limits.h"
#include "math.h"
#include <stdio.h>
#include <vector>
#include <iostream>
#include <stdlib.h>
//#include "queryprocessor.h"
#include "result.h"


using namespace std;

typedef struct 
{
 unsigned size;
 struct result *body;
 unsigned maxSize;
} CHeap;


void AllocateHeap(CHeap *p, unsigned NumEl);

void insertVazioHeap(CHeap *p,  int d, float _score, int nt);

void insertVazioHeap(CHeap *p, int d, float _score, float _upper_score, int nt);


void insertCheioHeap(CHeap *p,  int d, float _score, int nt);

void insertCheioHeap(CHeap *p,  int d, float _score, float _upper_score, int nt);

void DeallocateHeap(CHeap *p);

void initHeap(CHeap *p, unsigned NumEl);



void UpHeap(CHeap *p, unsigned k);

void DownHeap(CHeap *p, unsigned  k);
void insertElementoVazioHeap(CHeap *p); 

unsigned char haElementosHeap(CHeap *p);

void InsertElementoVazioHeap(CHeap *p);

void mostraHeap(CHeap *p);

inline int haEspacoHeap(CHeap *p) {
 return p->size < p->maxSize;
}



inline int OrdHeap(result t1, result t2) {
 if (t1.score < t2.score) return 1;
 return -1;
}

inline void limpaHeap(CHeap *h) {
	h->size=0;
}


void resetHeap(CHeap *p, unsigned NumEl);

#endif
