/*
 * PostingList.cpp
 *
 *  Created on: 21/09/2010
 *      Author: felipe
 */

#include "PostingList.h"

#ifdef IN_MEM
PostingList::PostingList() 
#else
// Refazer para disco: PostingList::PostingList(LeitorLista *_leitor_lista) 


#endif
{
  //	this->leitorLista = _leitor_lista;
  // this->lastDoc = -1;
	this->currPost = 0;
	this->tam_skip = 0;
	this->tam_lista = 0;
	this->pos_skip = 0;
	this->cont = 0;
	
	this->media_salto = 0;
	this->docs_processed = 0;
	this->total_descomprimido = 0;
	this->total_bloco_lido = 0;



}


PostingList::~PostingList() {
  //	if(this->leitorLista!=NULL)	delete this->leitorLista;
}

void PostingList::init() {
	//this->leitorLista = _leitor_lista;
        
	this->cont = 0;
	this->media_salto = 0;
	this->docs_processed = 0;
	//TODO: necessario?
}
void PostingList::close() {

}


/*void PostingList::first( int *docid, int *tf) {
	if (this->leitorLista == NULL) {

	  *docid= END_OF_LIST_DOC_ID();
	  *tf = 0;
	}
	this->leitorLista->getDoc3(docid, tf);
}

void PostingList::sequentialNext( int *docid, int *tf) {
	this->leitorLista->getDoc3(docid, tf);
	}*/

