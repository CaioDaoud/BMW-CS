/*
 * DocsInfo.cpp
 *
 *  Created on: 22/09/2010
 *      Author: felipe
 */

#include "DocsInfo.h"
#include <iostream>

DocsInfo::DocsInfo(const char *base_path) {
	descritor = new DescritorInfoweb((char*)base_path);

	int num_docs;
	char arq[10000];
	FILE *fp;

		//PageRank
	sprintf(arq, "%spagerank", base_path);
	fp = fopen(arq, "r");
	if(fp == NULL) {
	  printf("nao carreguei pagerank");
	}
	else {
	  fseek(fp, 0, SEEK_END);
	  this->num_docs = ftell(fp)/sizeof(double);
	  this->vetPageRank = new double[this->num_docs];
	  
	  fseek(fp, 0, SEEK_SET);
	  fread(this->vetPageRank, this->num_docs, sizeof(double), fp);
	  fclose(fp);
	}
	//Len Url
	sprintf(arq, "%slenurl", base_path);
	fp = fopen(arq, "r");
	if(fp == NULL) {
	  printf("Nao carreguei URL\n");
	}
	else {
	  this->vetLenUrl = new int[this->num_docs];

	  fread(this->vetLenUrl, this->num_docs, sizeof(int), fp);
	  fclose(fp);
	}
	//Slash Url
	sprintf(arq, "%sslashurl", base_path);
	fp = fopen(arq, "r");
	if(fp == NULL) {
	  printf("Nao carreguei slashurl\n");
	}
	else {
	  this->vetNumSlash = new int[this->num_docs];
	  
	  fread(this->vetNumSlash, this->num_docs, sizeof(int), fp);
	  
	  fclose(fp);
	}
}

DocsInfo::~DocsInfo() {
	delete descritor;
	delete[] vetPageRank;
}


void DocsInfo::getDocContent(int id, char* doc, int &size) {
	size = descritor->get_text((unsigned int)id, doc );
}

void DocsInfo::getLinkContent(int id, char* doc, int &size) {
	size = descritor->get_anchor((unsigned int)id, doc );
}

void DocsInfo::getTitle(int id, char* doc, int &size) {
	size = descritor->get_title((unsigned int)id, doc );
}

void DocsInfo::getURL(int id, char* doc, int &size) {
	size = descritor->get_url((unsigned int)id, doc );
}

void DocsInfo::getPageRank(int doc_id, double &pagerank) {
	if(doc_id >= this->num_docs)	pagerank = 0;
	else pagerank = this->vetPageRank[doc_id];
}

void DocsInfo::getLenUrl(int doc_id, int &lenUrl) {
	if(doc_id >= this->num_docs)	lenUrl = 0;
	else lenUrl = this->vetLenUrl[doc_id];
}

void DocsInfo::getNumSlash(int doc_id, int &numSlash) {
	if(doc_id >= this->num_docs)	numSlash = 0;
	else numSlash = this->vetNumSlash[doc_id];
}
