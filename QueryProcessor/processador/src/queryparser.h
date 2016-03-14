/*
 * QueryParser.h
 *
 *  Created on: 03/08/2010
 *      Author: felipe
 */

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "kwquery.h"

using namespace std;


#ifndef QUERYPARSER_H_
#define QUERYPARSER_H_

/**
 * queryparser class é uma classe stateless. Não possui atributos, portanto não guarda estado, pode ser usada por várias processadores ao mesmo tempo.
 * Não tira termos repetidos
 */
class queryparser {
public:
	queryparser();
	virtual ~queryparser();
	kwquery* parseQuery(string query);
	string tolower_and_remove_acents(string word);
private:
	std::vector<std::string>* split(const std::string &s, char delim, std::vector<std::string> *elems) {
	    std::stringstream ss(s);
	    std::string item;
	    while(std::getline(ss, item, delim)) {
	        if (!item.empty()) {
	        		elems->push_back(item);
	        }
	    }
	    return elems;
	}

	void strip (string &str) {
		for (unsigned int i = 0; i < str.size(); ++i) {
			if (str[i] == ' ' || str[i] == '"') {
				str.erase(i, 1);
			}
		}
	}

	std::vector<std::string>* split(const std::string &s, char delim) {
	    std::vector<std::string> *elems = new vector<string>;
	    return split(s, delim, elems);
	}

};


#endif /* QUERYPARSER_H_ */
