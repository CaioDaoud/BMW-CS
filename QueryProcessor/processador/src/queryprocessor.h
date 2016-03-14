/*
 * QueryProcessor.h
 *
 *  Created on: 03/08/2010
 *      Author: felipe
 */

#ifndef QUERYPROCESSOR_H_
#define QUERYPROCESSOR_H_

#include <iostream>
#include <fstream>
#include "result.h"

using namespace std;
#ifndef __APPLE__
using namespace std::tr1;
#endif

typedef struct Term {
  int term_id;
  //  int lastDoc;
  int tf;
  float idf;
  float min_score;
  float max_score;
  float mil_score;
} Term;



typedef struct feature {
	double idfWholedoc;
	double idfText;
	double idfTitle;
	double idfAnchor;
	double idfUrl;

	double tfWholedoc;
	double tfText;
	double tfTitle;
	double tfAnchor;
	double tfUrl;

	double lenWholedoc;
	double lenText;
	double lenTitle;
	double lenAnchor;
	double lenUrl;

	double bm25Wholedoc;
	double bm25Text;
	double bm25Title;
	double bm25Anchor;
	double bm25Url;

	double vetWholedoc;
	double vetText;
	double vetTitle;
	double vetAnchor;
	double vetUrl;

	double pagerank;
	double len_url;
	double num_slash;
}Feature;



typedef struct finalResult {
	unsigned int doc_id;
	int num_termos;
	float wholedocScore;
	float anchorScore;
	float textScore;
	float titleScore;
	float urlScore;
	float finalScore;
	double pagerank;
	int lenUrl;
	int numSlash;
	finalResult(unsigned int id, float _wholedocScore, float _anchorScore, float _textScore, float _titleScore, float _urlScore, double _pagerank, int _lenUrl, int _numSlash) :
	    doc_id(id),
	    wholedocScore(_wholedocScore), 
	    anchorScore(_anchorScore), 
	    textScore(_textScore), 
	    titleScore(_titleScore),
	    urlScore(_urlScore),
	    finalScore(0),
	    pagerank(_pagerank),
	    lenUrl(_lenUrl),
	    numSlash(_numSlash)
	    {
	}

} finalResult ;



class queryprocessor {
public:
	queryprocessor(int limiar, Index *_entireIndex, Index *_smallIndex, Index *_wholedocIndex, Index *_textIndex, Index *_titleIndex, Index *_anchorIndex, Index *_urlIndex, DocsInfo *_docsInfo, Vocabulario *_voc, char *file_gabarito, int _process_mode, int _index_type) 
//	queryprocessor(Index *_textIndex, Index *_titleIndex, Index *_anchorIndex, Index *_urlIndex, DocsInfo *_docsInfo, Vocabulario *_voc, char *file_gabarito, int _process_mode, int _index_type) 
	{
		this->limiarType = limiar;
		//upper=0;
		this->entireIndex = _entireIndex;
		this->wholedocIndex = _wholedocIndex;
		this->smallIndex = _smallIndex;
		this->textIndex = _textIndex;
		this->anchorIndex = _anchorIndex;
		this->titleIndex = _titleIndex;
		this->urlIndex = _urlIndex;
		this->docsInfo = _docsInfo;
		
		this->formula = 0;
		this->voc = _voc;
		this->C = 4;
		this->F = 14;
		this->RESULT_HEAP_SIZE = 20;
		this->query_log.open("query_log.txt", ios::app | ios::out);
		this->query_log << "-------------------------" << endl;
		this->id_consulta = 0;
		this->fpGabarito = fopen(file_gabarito, "w");
		this->mrr_total = 0.0;

		
		this->process_mode = _process_mode;
		this->index_type = _index_type; 
		total_avalidado = 0;
	}

	~queryprocessor() {
		this->query_log.close();
		
	//	printf("AVG Pivots :         %lu \n", (TotalPivo  +(1000000*tp))/ this->id_consulta);
	//	printf("AVG Descomprimidos : %lu\n",  (TotalSaltos+(1000000*ts))/this->id_consulta);
		
		fclose(this->fpGabarito);
	}


	int getResultHeapSize() { return this->RESULT_HEAP_SIZE; }
	void setResultHeapSize(int _new_size) { this->RESULT_HEAP_SIZE = _new_size; }
	
	void setSecSIndex(Index *_SecSmallIndex){this->SecondSmallIndex = _SecSmallIndex;}
	
	void processQuery(const string query);
	void processQuery(const string query, const string sec_query); 
	
	inline float calcLimiar(vector<Term> &terms, PostingList **plists, int &number_of_terms);
	

	void setFormula(int _f) {  this->formula = _f;	}

	
private:
	//file to log all queries
	ofstream query_log;

	//C constant defined in the WAND paper
	int C;

	//F constant defined in the WAND paper
	float F;

	// number of results in the processing heap
	int RESULT_HEAP_SIZE;

	// processing mode 1->OR 2->AND 3->TWO_PHASE
	int process_mode;

	// index type 1->UTI 2->BM25
	int index_type;

	//Formula a ser utilizada. 0 -> score primario; (1-10) -> Formula da GP
	int formula;
	
	//Melhor possibilidade de scores ao verificar a segunda camada de indice invertido
	float contador_maxScore;
	
	//menor entre os max_score da segunda camada.
	float min_min_score;
	
	int id_consulta;
	
	Vocabulario *voc;
	queryparser query_parser;
	SimpleWBRQueryParser simple_query_parser;

	Index *SecondSmallIndex;
	
	Index *entireIndex;
	Index *wholedocIndex;
	Index *smallIndex;
	Index *textIndex;
	Index *titleIndex;
	Index *anchorIndex;
	Index *urlIndex;

	DocsInfo *docsInfo;
	float ideal_doc_score;

	int total_avalidado;
	int limiarType;
	
	vector<Term>*   preProcessQuery(const string &query, int &size);
	vector<result>* processQuery_WAND(Index *index, int number_of_terms, vector<Term> &terms, int &number_of_results, float &ideal_doc_score);


    vector<result>* processQuery_BMW(Index *index, 
						     int &number_of_terms, 
						     vector<Term> &terms, 
						     int &number_of_results, 
							 float &ideal_doc_score);

							
	vector<result> *  processQueryBMW_CSP(Index *index, 
							 Index *secIndex, 
						     int number_of_terms, 
						     vector<Term> &terms, 
						     int &number_of_results, 
						     float &ideal_doc_score,
						     PostingList** mplists);
						     
	vector<result> *  processQueryBMW_CSPLimiar(Index *index, 
							 Index *secIndex, 
						     int number_of_terms, 
						     vector<Term> &terms, 
						     int &number_of_results, 
						     float &ideal_doc_score,
						     PostingList** mplists);
   
	vector<result>*  processQueryWave(Index *index, 
							Index *secIndex, 
							int number_of_terms, 
							vector<Term> &terms, 
							int &number_of_results, 
							float &ideal_doc_score,
							PostingList** plists);
							
	vector<result>*  processQueryWave_AND(Index *index, 
							Index *secIndex, 
							int number_of_terms, 
							vector<Term> &terms, 
							int &number_of_results, 
							float &ideal_doc_score,
							PostingList** plists);
							
	vector<result> * processQueryTWave(Index *index, 
							Index *secIndex,
							Index *thirIndex,
							int number_of_terms, 
							vector<Term> &terms, 
							int &number_of_results, 
							float &ideal_doc_score,
							PostingList** plists);
							
	vector<result> * processQueryBMWWANDBM25_Camadas(Index *index,
							 Index *sIndex,
						     int &number_of_terms, 
						     vector<Term>  &terms, 
						     int &number_of_results, 
						     float &ideal_doc_score);
						     
	vector<result> * processQueryBMWWANDBM25_TCamadas(Index *index,
							Index *sIndex,
							Index *tIndex,
							int &number_of_terms, 
							vector<Term>  &terms, 
							int &number_of_results, 
							float &ideal_doc_score)	;	
	
	float getOptimalScoreBMW_CSP(Index *index,  
							int number_of_terms, 
							vector<Term> &terms, 
							int &number_of_results, 
							float &ideal_doc_score,
							PostingList** plists);
							
	vector<result> * processQueryBMW_CS(Index *index, 
							Index *secIndex, 
							int number_of_terms, 
							vector<Term> &terms, 
							int &number_of_results, 
							float &ideal_doc_score,
							PostingList** plists);

	void teste();
	float calcScore( const float &pre, const float &idf, const float &freq, const float &norma);
	inline float calculateDocumentScoreUTI(Index *index, const doc* postingArray, vector<Term> *terms, size_t &size);
	inline float calculateDocumentScoreBM25(Index *index, const doc* postingArray, vector<Term> *terms, size_t &size,float *pre);

	vector<result>* removeRepetidos(vector<result> *ordenado, vector<result> *temp);
	vector<result>* processQueryforIndexAND(Index *index, int number_of_terms, vector<Term> &terms, int &number_of_results, float &ideal_doc_score);
	vector<result>* processQueryforIndexAND(Index *index, const int number_of_terms, vector<Term> &terms, int &number_of_results, float &ideal_doc_score, vector<result> *candidate_results, float top_maxScore) ;
	vector<result>* distBLockMaxScore(Index *index, const int number_of_terms, vector<Term> &terms);
	
	//vector<result>* processQueryforIndexOR(Index *index, int number_of_terms, vector<Term> &terms, int &number_of_results, float &ideal_doc_score, vector<result> *pre_results, vector<result> *candidate_results);

	vector<result>* processQueryforIndexORSimplificado(Index *index, int number_of_terms, vector<Term> &terms, int &number_of_results, float &ideal_doc_score);

	vector<result>* processQueryforIndexOR(Index *index, int number_of_terms, vector<Term> &terms, int &number_of_results, float &ideal_doc_score, vector<result> *candidate_results, vector<result> *candidate);
	vector<result>* processQueryforIndexOR(Index *index, int number_of_terms, vector<Term> &terms, int &number_of_results, float &ideal_doc_score, vector<result> *candidate_results); 

	vector<result>* processQueryforIndexOR(Index *index, int number_of_terms, vector<Term> &terms, int &number_of_results, float &ideal_doc_score);
	bool verificaLista(int IDdoc, vector<result> *candidate);
	bool avalia(vector<result> *resA, vector<result> *resB);
	bool mrr(vector<result> *resA, vector<result> *resB);

	PostingList** getPostingLists(Index *index, vector<Term> &terms, int number_of_terms);
	PostingList** getPostingLists(Index *index, Index *sIndex, vector<Term> &terms, int number_of_terms);
	PostingList** secGetPostingLists(Index *index, vector<Term> &terms, int number_of_terms);
	PostingList** getPostingLists(Index *index, Index *sIndex, Index *tIndex, vector<Term> &terms, int number_of_terms);
	
	//	CHeap resultHeap_ ; 
	float mrr_total;
	
	//int  block_min, score_min, score_block, score_score;
	FILE *fpGabarito;
	
	inline void mostraResults(vector<result>* results_){
		int topK=0;
		fprintf(this->fpGabarito,"\n %d\n ",this->id_consulta);
		for(vector<result>::iterator it = results_->begin(); it!= results_->end(); it++) {
			fprintf(this->fpGabarito, "\t%d %d %d %f\n", topK, it->num_termos, it->doc_id, it->score);
			topK++;
		}
	}

	inline void mostraResultsTela(vector<result>* results_){
		int topK=0;
		for(vector<result>::iterator it = results_->begin(); it!= results_->end(); it++) {
			printf("\t%d %d %d %f\n", topK, it->num_termos, it->doc_id, it->score);
			topK++;
		}
		printf("total de %d resultados\n",topK);
	}

};

#endif /* QUERYPROCESSOR_H_ */

