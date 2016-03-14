/*
 * kwquery.cpp
 *
 *  Created on: 04/08/2010
 *      Author: felipe
 */

#include "kwquery.h"

kwquery::kwquery(vector<string*> *t, vector<int> *p, vector<int> *tf, int s) {
	this->tfs = tf;
	this->tokens = t;
	this->positions = p;
	this->querySize = s;
}

kwquery::~kwquery() {
	if (this->positions) { //acho que e desnecessario checar se e nulo
		delete positions;
	}
	if (this->tokens) {
		for (unsigned int i = 0; i < this->tokens->size(); ++i) {
			delete tokens->at(i);
		}
		delete this->tokens;
	}
	delete this->tfs;
}
