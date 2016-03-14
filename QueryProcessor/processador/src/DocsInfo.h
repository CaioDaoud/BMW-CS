/*
 * DocsInfo.h
 *
 *  Created on: 22/09/2010
 *      Author: felipe
 */

#ifndef DOCSINFO_H_
#define DOCSINFO_H_

#include <string>
#include "DescritorInfoweb.h"

using namespace std;

class DocsInfo 
{

public:
	DocsInfo(const char *base_path);
	virtual ~DocsInfo();

	void getDocContent(int id, char* doc, int &size);
	void getLinkContent(int id, char* doc, int &size);
	void getTitle(int id, char* doc, int &size);
	void getURL(int id, char* doc, int &size);
	void getPageRank(int id, double &pagerank);
	void getLenUrl(int id, int &lenUrl);
	void getNumSlash(int id, int &numSlash);

private:
	DescritorInfoweb *descritor;
	void loadPageRankFile();
	double *vetPageRank;
	int *vetLenUrl;
	int *vetNumSlash;

	int num_docs;

    FILE *pageRankFile;
};

#endif /* DOCSINFO_H_ */
