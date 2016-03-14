/*
 * kwquery.h
 *
 *  Created on: 04/08/2010
 *      Author: felipe
 */

#include <vector>
#include <iostream>
#include <string>

using namespace std;

#ifndef KWQUERY_H_
#define KWQUERY_H_

typedef struct token {
	int size;
	int tf; //frequency in the query
	string *term;
	~token() {
		delete[] term;
	}
} token;

using namespace std;

class kwquery {
public:
	vector<string*> *tokens;
	vector<int> *positions;
	vector<int> *tfs;

	kwquery(vector<string*> *t, vector<int> *p, vector<int> *tf, int s);

	int size() { return querySize; }

	void beginIterator() {
		curToken = 0;
	}

	bool hasNext() {
		return curToken < positions->size();
	}
	token next() {
		token x;
//		int type = positions->at(n);
		int type = positions->at(curToken);
		if (type > 0 ) { //is phrase
			unsigned int last_pos = curToken;
			while (last_pos < positions->size() && positions->at(last_pos) == type) {
				last_pos++;
			}
			last_pos--;
			int phraseSize = last_pos - curToken + 1;
			x.size = phraseSize;
			x.term = new string[phraseSize];
			x.tf = 1;  //frase sempre tem TF = 1
			for (unsigned int j = curToken; j <= last_pos; ++j) {
				x.term[j-curToken] = *tokens->at(j);			//copiando TODO: pode melhorar isso
			}
			curToken += phraseSize;
		}
		else {						// is term
			x.term = new string[1];
			x.term[0] = *tokens->at(curToken);
			x.size = 1;
			x.tf = tfs->at(curToken);
			curToken++;
		}
		return x;
	}


	virtual ~kwquery();
	kwquery();
private:
	int querySize;
	unsigned int curToken;
};

#endif /* KWQUERY_H_ */
