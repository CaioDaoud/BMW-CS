/*
 * Index.h
 *
 BDRI

 */

#ifndef INDEX_H_
#define INDEX_H_

#include "PostingList.h"
#include <vector>
#include <semaphore.h>

/**
 * The query processor has one Index object for: textual, title, anchor text
 */
class Index {

public:
    enum IndexType {TEXT, ANCHOR, TITLE, URL, UTI};
	Index(const char *_index_base_path, IndexType _type,  int topK);
	Index(const char *_index_base_path, IndexType _type,  int topK, const char *_base_path );
	
    virtual ~Index();
	int   getLastDoc(int term_id) { return vetLastDoc[term_id]; }
	PostingList* getPostingList(int term_id);
	float getTermIdf(int term_id) {	return vetIdf[term_id];	}
	float getDocNorm(int doc_id) {	return vetNorm[doc_id];	}
	float getTermMinScore(int term_id) { return vetMinScore[term_id]; }
	float getTermMaxScore(int term_id) { return vetMaxScore[term_id]; }
	float getTermMilScore(int term_id) { return vetMilScore[term_id]; }
	unsigned int getNDocs() { return this->nDocs; }
 	IndexType type;

	void loadToMainMemory();

	inline string getIndexTypeString() {
	    switch(this->type) {
	        case TEXT: return "TEXT";
	        case ANCHOR: return "ANCHOR";
	        case TITLE: return "TITLE";
	        case URL: return "URL";
	        case UTI: return "UTI";
	        default: return "UTI";
	    }
	}

	unsigned char **ptr_index;
	unsigned char **ptr_skip;
	unsigned nDocs;
	float *vetNorm;
	float *vetIdf;
	unsigned int *vetFT;
	int *vetLastDoc;
private:

	const char *index_base_path;
	const char *base_path;
	
    FILE *idxFile;
	int number_of_terms_index;

	float *vetMaxScore;
	float *vetMinScore;
	float *vetMilScore;
	IDX   *vetIdx;
};

#endif /* INDEX_H_ */
