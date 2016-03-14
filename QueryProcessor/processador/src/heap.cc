#include "limits.h"
#include "heap.h"
#include "math.h"
#include <stdio.h>


/*------------------------------------------------------------------
  Função: AllocateHeap_
  **Objetivo:* Alocar memória para o _heap_
  **Variáveis globais: NumberOfRuns: 
       o número R de _runs_ gerados durante o processo de indexação e que 
       devem agora ser intercalados de forma a se obter um arquivo invertido 
       ordenado. R corresponde ao número de elementos do _heap.
  Valor de retorno: Um apontador para o bloco de memória alocado
-----------------------------------------------------  */
void AllocateHeap(CHeap *p, unsigned numEl){
    /* Alocam-se $R + 1$ elementos (pois a posição 0 funciona como um sentinela: */
    if (NULL == (p->body = new result[numEl+1])) {
        printf("Error allocating memory for heap\n");
		exit(1);
    }
} /* _AllocateHeap_ */

 /* -------------------------------------------------------------
    Função: _DeallocateHeap_
    Objetivo: Liberar a memória alocada pelo _heap_
    Parâmetros: p apontador para o _heap_
 ------------------------------------------------------------- */

void DeallocateHeap(CHeap *p){
    delete p->body;
} /* _DeallocateHeap_ */

/* -------------------------------------------------------------
   Função: _InitHeap_
   Objetivo: Inicializar o _heap_ para as operações de _merge_
   Parâmetros:* @p@: apontador para o _heap_
   --------------------------------------------------------------*/

void initHeap(CHeap *p, unsigned numEl){
  p->maxSize = numEl;
  AllocateHeap(p,numEl);
  p->size = 0;       /* O seu tamanho é indicado como 0 elementos */
  p->body[0].score = -1.0; /* Na posição 0 colocamos o sentinela: $t = 0$ */
  //p->body[0].D.Frequencia = 0xFFFF; /* Na posição 0 colocamos o sentinela: $f = 0xFFFF$ */
  //p->body[0].D.Bloco = 0;  /* Também temos $d = 0$ para o sentinela */

} /* _InitHeap_ */



/* -------------------------------------------------------------
   Função: _InsertHeap_
   Objetivo: Inserir uma nova tupla $\tpl{t, d, f, r}$ no _heap_ e 
   rearranjá-lo para que mantenha sua propriedade interna de ordenação
   Parâmetros: p: apontador para o _heap_, t, d, f, r: os componentes
   $t, d, f, r$ da tupla, respectivamente
  --------------------------------------------------------------*/


void insertVazioHeap(CHeap *p, int d, float _score, int nt){/**  tirei o * do t1*/
    p->size++;
   // cout << "3.1" << endl;                 /* Tamanho do _heap_ é atualizado */
    p->body[p->size] = result(d, _score, nt);    /* Os elementos são inseridos */
//cout << "3.2 - docId, " << d << "score :  " << _score << endl;

    UpHeap(p, p->size);        /* A propriedade de ordenação é reestabelecida */
   
} /* _InsertHeap_ */



void insertVazioHeap(CHeap *p, int d, float _score, float _upper_score, int nt){/**  tirei o * do t1*/
    p->size++;
    //cout << "3.1" << endl;                 /* Tamanho do _heap_ é atualizado */
    p->body[p->size] = result(d, _score, _upper_score, nt);    /* Os elementos são inseridos */
//cout << "3.2" << endl;
    UpHeap(p, p->size);        /* A propriedade de ordenação é reestabelecida */
 //   cout << "3.3" << endl;
} /* _InsertHeap_ */



/* -------------------------------------------------------------
   Função: _InsertHeap_
   Objetivo: Inserir uma nova tupla $\tpl{t, d, f, r}$ no _heap_ e 
   rearranjá-lo para que mantenha sua propriedade interna de ordenação
   Parâmetros: p: apontador para o _heap_, t, d, f, r: os componentes
   $t, d, f, r$ da tupla, respectivamente
  --------------------------------------------------------------*/

void insertCheioHeap(CHeap *p,  int d, float _score, int nt){/**  tirei o * do t1*/
    p->body[1] = result(d, _score, nt);    /* Os elementos são inseridos */
    DownHeap(p,1);        /* A propriedade de ordenação é reestabelecida */

} /* _InsertHeap_ */

void insertCheioHeap(CHeap *p,  int d, float _score, float _upper_score, int nt){/**  tirei o * do t1*/
    p->body[1] = result(d, _score,  _upper_score, nt);    /* Os elementos são inseridos */
    DownHeap(p,1);        /* A propriedade de ordenação é reestabelecida */

} /* _InsertHeap_ */


/* -------------------------------------------------------------
   Função:* _UpHeap_
   Objetivo: Reestabelecer a propriedade de ordenação do _heap_ fazendo o elemento
   inserido à posição k ``descer'' até uma posição que não contrarie a propriedade
   mencionada
   Parâmetros: p: apontador para o _heap_, k: a posição contendo o elemento a
   ser descendido
   ------------------------------------------------------------- */

void UpHeap(CHeap *p, unsigned  k){
    result v;    /* Auxiliar durante as trocas de posições */

    v = p->body[k];       /* v armazena o elemento inserido a subir o _heap_ */

    /* Enquanto o pai do elemento inserido (na posição $k/2$) for maior que ele, o
       pai desce para a posição $k$ e o filho sobre para $k/2$:
    */
// cout << "3.2.1" << endl;
    while (OrdHeap(p->body[k >>1], v) > 0){
      p->body[k] = p->body[k >>1];
      k = k >>1;
    }
  //   cout << "3.2.2" << endl;
    p->body[k] = v;  /* No valor de $k$ final colocamos o elemento inserido */

} /* _UpHeap_ */

/* -------------------------------------------------------------
   Função: _DownHeap_
   Objetivo: Reestabelecer a propriedade de ordenação do _heap_ fazendo 
   o elemento inserido à posição $k$ ``subir'' até uma posição que não 
   contrarie a propriedade mencionada
   Parâmetros:p apontador para o _heap_, k: a posição contendo o elemento 
   a ser ascendido
   -------------------------------------------------------------*/

void DownHeap(CHeap *p, unsigned  k){
    result v;    /* Auxiliar durante as trocas de posições */
    unsigned  j;       /* Auxiliar nas iterações */
    unsigned tmpSize = p->size >>1;
    v = p->body[k];       /* @v@ armazena o elemento inserido a descer no _heap_ */

    /* Enquanto $k$ for menor que $R/2$ (isto é, $k$ tem filhos): */

    while (k <= tmpSize )
    {
      j =  k << 1;        /* $j$ contém o filho da esquerda de $k$ */

        /* Se $j < R$ e maior que seu irmão da direita, passamos a analisar esse irmão,
         * de forma que, se trocarmos $k$ por algum de seus filhos, certamente o menor
         * deles subirá para o lubar de $k$:
         */

        if ((j < p->size) &&  ( OrdHeap(p->body[j], p->body[j + 1]) > 0 ) )
        {
            j++;
        }

        /* Se o elemento inserido for menor que o menor de seus filhos, a propriedade
         * do _heap_ está mantida e podemos terminar as iterações:
         */
        if (OrdHeap(v,  p->body[j]) < 0 ) {
			break;
        }

        /* Caso contrário, o filho $j$ sobe para a posição $k$, que passa a apontar 
         * para a posição $j$:
         */
        p->body[k] = p->body[j];
        k = j;
    }
    p->body[k] = v;   /* No valor de $k$ final colocamos o elemento inserido */

} /* _DownHeap_ */


void insertElementoVazioHeap(CHeap *p) {
    DownHeap(p,1);        /* A propriedade de ordenação é reestabelecida */
} /* _InsertHeap_ */

unsigned char haElementosHeap(CHeap *p) {
	return (p->size >0);
}

void mostraHeap(CHeap *p) {
  
  unsigned x;
  
  for(x=1;x<= p->size; x++) {
    printf("(%.2f %d) ",p->body[x].score,p->body[x].doc_id);
  }
  printf("\n");
}
