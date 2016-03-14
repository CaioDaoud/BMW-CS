/*
 * QueryParser.cpp
 *
 *  Created on: 03/08/2010
 *      Author: felipe
 */

#include "queryparser.h"
#include <algorithm>

queryparser::queryparser() {

}

queryparser::~queryparser() {

}



kwquery* queryparser::parseQuery(string query) {
	std::transform(query.begin(), query.end(), query.begin(), ::tolower); 	//transforming to Lower! NAO TRANSFORMA COM ACENTO e CEDILHA

	unsigned int i;
	bool insidePhrase = false;
	char current;
	vector<string*> terms;
	vector<string*> *kwterms = new vector<string*>;
	vector<int> *tf = new vector<int>;

	vector<int> *positions = new vector<int>;
	int phraseNumber = 0;

	int querySize = 0;
	string *phrase_term = new string;
	string *term = new string;

	for (i = 0; i < query.size(); ++i) {
		current = query.at(i);

		if (current == '"') { //comecando frase
			if (insidePhrase) {
				insidePhrase = false;
				if (phrase_term->size() != 0) {
					positions->push_back(phraseNumber);
					kwterms->push_back(phrase_term);
					tf->push_back(1);
					phrase_term = new string();
				}
			}
			else {
				phraseNumber++;
				querySize++;
				insidePhrase = true;
			}
		}
		else {
			if (insidePhrase) {
				if (current == ' ') {
					if (phrase_term->size() != 0) {
						positions->push_back(phraseNumber);
						kwterms->push_back(phrase_term);
						tf->push_back(1);
						phrase_term = new string();
					}
				}
				else {
					if (ispunct(current) == 0) (*phrase_term) += current;
					else { //se for pontuação coloca espaço no lugar e reprocessa essa posicao da string
						query[i] = ' ';
						i--;
					}
				}
			}
			else {
				if (current == ' ') {
					if (!term->empty()) {
						querySize++;

						bool found = false;
						for (unsigned int i = 0; i < kwterms->size() && !found; ++i) {
							if ( positions->at(i) == 0 &&  kwterms->at(i)->compare(*term) == 0) { //se é termo sozinho (não frase)
								(*tf)[i] = tf->at(i) + 1;
								found = true;
							}
						}
						if (!found) {
							positions->push_back(0);
							kwterms->push_back(term);
							tf->push_back(1);
						}

						term = new string();
					}
				}
				else {
					if (ispunct(current) == 0) (*term) += current;
					else { //se for pontuação coloca espaço no lugar e reprocessa essa posicao da string
						query[i] = ' ';
						i--;
					}
				}

			}
		}
	}
	if (term->size() != 0) {
		querySize++;
		bool found = false;
		for (unsigned int i = 0; i < kwterms->size(); ++i) {
			if ( positions->at(i) == 0 &&  kwterms->at(i)->compare(*term) == 0 ) {
				(*tf)[i] = tf->at(i) + 1;
				found = true;
			}
		}
		if (!found) {
			positions->push_back(0);
			kwterms->push_back(term);
			tf->push_back(1);
		}

	}
	else {
		delete term;
	}
	delete phrase_term;
	kwquery *x = new kwquery(kwterms,positions, tf, querySize);

	return x;
}

string queryparser::tolower_and_remove_acents(string word)
{
  for( unsigned int i = 0; i < word.length(); ++i )
	{
    word[i] = tolower( word[i] );

    if(((word[i] >= -64) && (word[i] <= -59)) || ((word[i] >= -32) && (word[i] <= -27)))
      word[i] = 'a';
    else
    if(((word[i] >= -56) && (word[i] <= -53)) || ((word[i] >= -24) && (word[i] <= -21)))
      word[i] = 'e';
    else
    if(((word[i] >= -52) && (word[i] <= -49)) || ((word[i] >= -20) && (word[i] <= -17)))
      word[i] = 'i';
    else
    if(((word[i] >= -46) && (word[i] <= -42)) || ((word[i] >= -14) && (word[i] <= -10)))
      word[i] = 'o';
    else
    if(((word[i] >= -39) && (word[i] <= -36)) || ((word[i] >= -7) && (word[i] <= -4)))
      word[i] = 'u';
    else
    if((word[i] == -57) || (word[i] == -25))
      word[i] = 'c';
    else
    if((word[i] == -47) || (word[i] == -15))
      word[i] = 'n';

  }
  return word;
}


