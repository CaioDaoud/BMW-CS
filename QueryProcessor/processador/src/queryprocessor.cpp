#include <iostream>
#include <algorithm> 
#include <vector>
#include <sys/time.h>
#include <time.h> 
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <exception>
#include "queryprocessor.h"
 


/*Guambiara para sumir com as msg de debug. TODO: temos que comecar a usar o log4cpp*/
#define SHOW_TIME 1
#define SHOW_RESULTS 1


#define UTI_SCORE 1
#define BM25_SCORE 2
#define VETORIAL_SCORE 3

#define WAND_MODE 1
#define BMW_MODE 2
#define BMWCSP_MODE 3
#define BMWFLASH_MODE 4
#define BMWCAMDAS_MODE 5
#define WAVE_MODE 6
#define HIBRIDO_MODE 7
#define thirWAVES_MODE 8
#define thirBMW_MODE 9
#define BMWCS_MODE 10
#define NO_MODE 0

#define SHOW_MSG 1 

#define tipoData "%a, %d/%m/%Y %H:%M:%S"
#define nMesAno  "%m_%Y"
#define SERVER 	 "RCOMP 2007/2008"
#define pagina 	 "<html><body></body></html>"

using namespace std;
PostingList **plistsGlob = NULL;



/**
 * Auxilary functions:
 */

inline double DIV(float a, float b)
{
	if(b==0.0)	return 0;
	return (double)a/(double)b;
}
inline double SUM(float a, float b)
{
	return (double)a+(double)b;
}
inline double MUL(float a, float b)
{
	return (double)a*(double)b;
}

bool resultComparator(const result& x, const result& y) 
{
	/*Ordena primeiro por quem tem todas as palavras*/
	if(fabs(x.score-y.score) > 0.00001)  return x.score > y.score;

	/*Em caso de empate, ordena pelo menor docid*/    
	return x.doc_id < y.doc_id;
}

bool resultComparator_(const result& x, const result& y) 
{
	return x.doc_id < y.doc_id;
}

bool resultComparator_scr(const result& x, const result& y) 
{
	return x.score > y.score;
}

bool finalResultScoreComparator(const finalResult* x, const finalResult* y) 
{
	return x->finalScore > y->finalScore;
}

bool finalResultDocIdComparator(const finalResult& x, const finalResult &y) 
{
	return x.doc_id < y.doc_id;
}

bool resultDocIdComparator(const result& x, const result &y) 
{
	return x.doc_id < y.doc_id;
}

void queryprocessor::processQuery(const string query) 
{
	int number_of_terms;
	vector<Term> *terms;

	/*Quebra a consulta em um vetor de termos presentes no vocabulario*/
	terms = preProcessQuery(query, number_of_terms);
		
	int text_n_results;
	float ideal_doc_score_text = 0.0f;

	ideal_doc_score_text = 0;
	
	vector<result> *tResults = NULL;
	
	
	cout<<"Number_of_terms "<<number_of_terms<<" ";
	
	switch (this->process_mode){
		
		case WAND_MODE :
			///Wand [2003]
			tResults = this->processQuery_WAND(this->entireIndex, number_of_terms, *terms, text_n_results, ideal_doc_score_text);
			break;
		
		case BMW_MODE:
			///BMW [2011]
			tResults = this->processQuery_BMW(this->entireIndex, number_of_terms, *terms, text_n_results, ideal_doc_score_text);
			break;
		
		case BMWCS_MODE :
			///BMW_CS.
			tResults = this->processQueryBMW_CS(this->smallIndex, this->SecondSmallIndex, number_of_terms, *terms, text_n_results, ideal_doc_score_text, plistsGlob);
			break;
		
		default:
			printf("------------MODO DE PROCESSAMENTO INEXISTENTE <WAND, BMW, CSP>-----------------[%d]\n",this->process_mode);
			exit(0);
	}

	sort(tResults->begin(), tResults->end(), &resultComparator);
	mostraResults(tResults);
	
	this->id_consulta++;
	if(tResults != NULL)
	delete tResults;
	delete terms;
}



float queryprocessor::calcScore(const float &pre, const float &idf, const float &freq, const float &norma){
	
	if(index_type == BM25_SCORE){	///plists[i]->pre * (float)(f) / ((float)(f + 2 * ( 0.25 + 0.75 * (float)(index->getDocNorm(did))/(float)(130) ) ) );
		return ((pre * freq / ((float)(freq + 2 * ( 0.25 + 0.75 * (float)(norma)/(float)(130))))));
	}
	else if (index_type == VETORIAL_SCORE){	/// freq * vetIdf[term_id] / vetNorm[docid];
		return (freq*idf*idf)/norma;
	}else{
		cerr<<"ERRO : Index_type INEXISTENTE "<<endl;
		exit(0);
	}
}

vector<Term>* queryprocessor::preProcessQuery(const string &query, int &number_of_terms) 
{
	int *terms_ids;

	terms_ids = simple_query_parser.parse_query(query.c_str(), this->voc, &number_of_terms);
	if(terms_ids == NULL || number_of_terms==0) {
		if(terms_ids)	delete[] terms_ids;
		number_of_terms = 0;
		return NULL;
	}

	vector<Term> *query_terms = new vector<Term>();
	query_terms->reserve(number_of_terms); //reserva dentro do vector o numero de termos
	query_terms->resize(number_of_terms);  //reserva dentro do vector o numero de termos

	bool repeated;
	int tf = 1;
	int k = 0;

	for (int i = 0; i < number_of_terms; ++i) {
		repeated = false;
		tf = 1;
		if ((*query_terms)[i].tf != -1) { //se tf == -1 é um termo já repetido
			for (int j = i+1; j < number_of_terms; ++j) {
				if (i != j) {
					if (terms_ids[i] == terms_ids[j]) {
						repeated = true;
						(*query_terms)[j].tf = -1;
						tf ++;
					}
				}
			}
			(*query_terms)[k].term_id = terms_ids[i];
			(*query_terms)[k].tf = tf;
			k++;
		}
	}
	number_of_terms = k; // k is the new size of the query
	delete[] terms_ids;
	return query_terms;
}

PostingList** queryprocessor::getPostingLists(Index *index, vector<Term> &terms, int number_of_terms){
	PostingList **plists = new PostingList*[number_of_terms];

	for (int i = 0; i < number_of_terms; ++i) {
		
		/*Obtem a lista invertida do termo desejado*/
		plists[i] = index->getPostingList(terms[i].term_id); 
		
		/*Utilizando IDF para pegar os termos de menor lista na fase de pickTerm()*/
		terms[i].idf = index->getTermIdf(terms[i].term_id);
		terms[i].min_score = index->getTermMinScore(terms[i].term_id);
		terms[i].max_score = index->getTermMaxScore(terms[i].term_id);
		terms[i].mil_score = index->getTermMilScore(terms[i].term_id);
	} 

	return plists;
}

inline void insereNoRanking(const int SIZE_, int *pos, float *threshold, const result r_ , vector<result> *candidate_results ){
	if ( *pos < SIZE_ ) {  		  
		candidate_results->push_back( r_ ); 
		push_heap (candidate_results->begin(),candidate_results->end());
		*pos+=1;
		if( *pos==SIZE_){
			if( *threshold < (*candidate_results)[0].score){
				*threshold = (*candidate_results)[0].score;
			}
		}
		
	}else{
		
		pop_heap (candidate_results->begin(),candidate_results->end()); 
		candidate_results->pop_back();
		candidate_results->push_back(r_ ); 
		push_heap (candidate_results->begin(),candidate_results->end());
		
		//Atualiza o threshold com o menor score do heap para acelerar o processamento
		if( *threshold < (*candidate_results)[0].score){
			*threshold = (*candidate_results)[0].score;
		}
		
	}
}


inline void sortPostings(PostingList** mplists,  int mplistsSize) {
	/*Ordena os listas invertidas por docid*/
	int i, j;
	PostingList *plistTemp;
	
	for(i = 1; i < mplistsSize; i++)    {
		plistTemp= mplists[i];
		j = i-1;
		while(j>=0 && plistTemp->currPost < mplists[j]->currPost )
		{
			mplists[j+1] = mplists[j];
			j = j-1;
		}
		mplists[j+1] = plistTemp;
	}
}


inline void popdown(PostingList **plists, int i, int num){
	PostingList *temp = plists[i];
	int j = i + 1;
	while( j < num && temp->currPost > plists[j]->currPost)
	{
		/* switch */
		plists[j-1] = plists[j];
		j++;
	}
	plists[j - 1] = temp;
}

/********************************************/
/* move the pointer to the right block      */
/********************************************/
inline unsigned int movepointer(PostingList *plist, unsigned int did){
	while (did > plist->buffer_skip_docid[plist->checkblock]) {
		plist->checkblock ++;
	}	
	return plist->buffer_skip_docid[plist->checkblock] + 1 ;
}


inline void popdownCS(PostingList **plists, PostingList **splists, int i, int num)
{
	PostingList *temp  = plists[i];
	PostingList *stemp = splists[i];
	int j = i + 1;
	while( j < num && temp->currPost > plists[j]->currPost)
	{
		/* switch */
		plists[j-1]  = plists[j];
		splists[j-1] = splists[j];
		j++;
	}
	plists[j - 1] = temp;
	splists[j - 1] = stemp;
}


inline void sortPostingsCS(PostingList** mplists,
		PostingList** mSplists,
		int mplistsSize) {
	/*Ordena as listas invertidas por docid*/
	int i, j;
	PostingList *plistTemp;
	PostingList *SplistTemp;

	for(i = 1; i < mplistsSize; i++)    {
		plistTemp= mplists[i];
		SplistTemp= mSplists[i];
		j = i-1;
		while(j>=0 && plistTemp->currPost < mplists[j]->currPost ){
			mplists[j+1] = mplists[j];
			mSplists[j+1] = mSplists[j];
			j = j-1;
		}
		mplists[j+1]  = plistTemp;
		mSplists[j+1] = SplistTemp;
	}
}



//  **************  INICIO DA WANDBM25 ***************************
vector<result>* queryprocessor::processQuery_WAND(Index *index, 
		int number_of_terms, 
		vector<Term> &terms, 
		int &number_of_results, 
		float &ideal_doc_score){

	unsigned int MAXD = std::numeric_limits<unsigned int>::max();
	unsigned int did;
	int k = this-> RESULT_HEAP_SIZE;
	int f;
	int least_ft, least_i;
	float scoret;
	float score;
	float threshold = 0.0;
	int pivot;
	int pos =0;
	

	vector<result> *candidate_results;
	candidate_results = new vector<result>();
	candidate_results->reserve(RESULT_HEAP_SIZE);

	
	
	if (number_of_terms == 0) return new vector<result>();
	PostingList **plists = getPostingLists(index, terms, number_of_terms);
	if(plists == NULL)	return new vector<result>();

	//Remove terms that the size of the list is 0
	for (int i = 0; i < number_of_terms; ++i) {
		int j;
		while((i<number_of_terms) && (plists[i]->tam_lista == 0)) {
			delete plists[i];
			for(j=i ; j<number_of_terms-1;j++) {
				plists[j] = plists[j+1];
			}
			number_of_terms--;
		}
	}
	//If not remain terms in the query, the process ends.
	if(number_of_terms == 0) {
		delete plists;
		return new vector<result>();
	}

	/*Computes a piece of BM25 score.
	 *Puts the pointer at the beginning of each list.
	 * */
	for (int i = 0; i < number_of_terms; ++i) {
		plists[i]->pre= (float)3 * (float)(log( 1 + (float)(index->getNDocs() - plists[i]->ft + 0.5)/(float)(plists[i]->ft + 0.5))/log(2));
		plists[i]->first();
	}
	
	sortPostings(plists, number_of_terms);
	
	//start WAND
	while( true )
	{
		// find the pivot term
		pivot = -1;
		scoret = 0.0f;
		
		for(int i = 0; i < number_of_terms; i ++){
			scoret += plists[i]->max_score;
			if( scoret > threshold)
			{
				while( i + 1 < number_of_terms && plists[i + 1]->currPost == plists[i]->currPost) i++;
				pivot = i;
				break;
			}
		}

		// pivot is -1 or MAXD, no item will make into topK 
		if(pivot == -1 || plists[pivot]->currPost == MAXD)
			break;
		
		// Evaluate this docID ('did') only when 'did' is equal to plists[0]->currPost
		if( plists[pivot]->currPost == plists[0]->currPost )
		{

			did = plists[ 0 ]->currPost;
			scoret = 0.0f;
			score = 0.0f;
			
			//Computing the score.
			for(int i = 0 ; i < pivot+1; i++)
			{
				f = plists[i]->getFreq();
				scoret = calcScore( plists[i]->pre, terms[i].idf , (float)(f),index->getDocNorm(did)); //plists[i]->pre * (float)(f) / ((float)(f + 2 * ( 0.25 + 0.75 * (float)(index->getDocNorm(did))/(float)(130) ) ) );
				score += scoret;	
			}
			
			//Inserts the document in the ranking and update the threshold.
			if(score>=threshold){
				insereNoRanking(k,  &pos, &threshold, result(did, score, pivot+1), candidate_results);
			}
			
			//Move the pointers and reorders the lists*
			for(int i = pivot ; i > -1; i--)
			{
				plists[i]->next(did + 1);
				popdown(plists, i, number_of_terms);
			}
		}
		else
		{
			// pick the term with the smallest ft, advance the pointer. 
			least_ft = plists[0]->ft;
			least_i = 0;

			int i =0;
			do{
				if(plists[i]->ft < least_ft){
					least_ft = plists[i]->ft;
					least_i = i;
				}				
				i++;
			}while(plists[i]->currPost != plists[pivot]->currPost);
			
			plists[least_i]->next(plists[pivot]->currPost);	
			popdown(plists, least_i, number_of_terms);
		}
	}

	
	for (int i = 0; i < number_of_terms; ++i) {	delete plists[i];}
	delete[] plists;

	return candidate_results;
}

//****************************** FIM DA WAND ***************************




//  **************  INICIO DA BMW ***************************/
vector<result>* queryprocessor::processQuery_BMW(Index *index, 
		int &number_of_terms, 
		vector<Term> &terms, 
		int &number_of_results, 
		float &ideal_doc_score){




	float score, scoret,maxposs;
	unsigned int did,minpivot,tempd;
	int f=0;
	int least_ft, least_i;
	int k = this-> RESULT_HEAP_SIZE;
	float threshold = 0.0;
	int pivot;
	int pos = 0;

	if (number_of_terms == 0) return new vector<result>();
	
	vector<result> *candidate_results;
	candidate_results = new vector<result>();
	candidate_results->reserve(RESULT_HEAP_SIZE);

	unsigned int MAXD = std::numeric_limits<unsigned int>::max();

	PostingList **plists = getPostingLists(index, terms, number_of_terms);
	if(plists == NULL)	return new vector<result>();


	//Remove terms that the size of the list is 0	
	for (int i = 0; i < number_of_terms; ++i) {
		int j;
		while((plists[i]->tam_lista == 0) && (i<number_of_terms)) {
			delete plists[i];
			for(j=i ; j<number_of_terms-1;j++) {
				plists[j] = plists[j+1];
			}
			number_of_terms--;
		}
	}
	//If not remain terms in the query, the process ends.
	if(number_of_terms == 0) {
		delete plists;
		return new vector<result>();
	}
	
	/*Computes a piece of BM25 score.
	 *Puts the pointer at the beginning of each list.
	 * */
	for (int i = 0; i < number_of_terms; ++i) {
		plists[i]->pre= (float)3 * (float)(log( 1 + (float)(index->getNDocs() - plists[i]->ft + 0.5)/(float)(plists[i]->ft + 0.5))/log(2));
		plists[i]->first();   // move todas as listas para a 1a posicao
	}


	sortPostings(plists,number_of_terms);
	
	//Start BMW
	while( true ){
		// find the pivot term 
		pivot = -1;
		score = 0.0f;
		for(int i = 0; i < number_of_terms; i ++)
		{
			score += plists[i]->max_score;
			if( score > threshold)
			{
				while( i + 1 < number_of_terms && plists[i + 1]->currPost == plists[i]->currPost)	 i++;

				pivot = i;
				break;
			}
		}

		// pivot is -1 or MAXD, no item will make into topK 
		if(pivot == -1 || plists[pivot]->currPost == MAXD)    break;
		
		/* Use shallow pointers to comput BlockMaxScore to check if the pivot did can make into the top */
		//minpivot is used to skip entries if blockmaxScore not exceed the threshold.
		maxposs = plists[pivot]->buffer_skip_max_score[ plists[pivot]->pos_skip];	
		did = plists[pivot]->currPost ;
		minpivot = plists[pivot]->buffer_skip_docid[ plists[pivot]->pos_skip ] > MAXD ? MAXD : plists[pivot]->buffer_skip_docid[ plists[pivot]->pos_skip ]+1;
		for(int i = 0; i < pivot; i++){
			tempd = movepointer(plists[i], did);			
			if( minpivot >  tempd)
				minpivot = tempd;

			if(plists[i]->checkblock < plists[i]->tam_skip){
				maxposs += plists[i]->buffer_skip_max_score[ plists[i]->checkblock];
			}
		}
		
		//maxposs is the BlockMaxScore
		if( maxposs >= threshold )
		{
			// evaluate this did only when did is equal to plists[0]->currPost 
			if( did == plists[0]->currPost )
			{
				score = 0.0;
				//Computing score.
				for(int i = 0 ; i < pivot + 1; i++) {
					f = plists[i]->getFreq();
					scoret = calcScore( plists[i]->pre, terms[i].idf , (float)(f),index->getDocNorm(did));  //scoret = plists[i]->pre * (float)(f) / ((float)(f + 2 * ( 0.25 + 0.75 * (float)(index->getDocNorm(did))/(float)(130) ) ) );
					score+=scoret; 
					maxposs -= ( plists[i]->buffer_skip_max_score[ plists[i]->pos_skip ] - scoret );

					if( !(maxposs > threshold) )
						break;

				}				
				
				//Add the document in ranking and update the threshold.
				if(score>=threshold){
					insereNoRanking(k,  &pos, &threshold, result(did, score, pivot+1), candidate_results);
				}
				
				//Move the pointers and reorders the lists
				for(int i = pivot ; i > -1; i--){
					plists[i]->next(did + 1);
					popdown(plists, i, number_of_terms);
				}
			}
			else 	// pick the term with the smallest ft, advance the pointer to pivot docId.
			{
				least_ft = plists[0]->ft;
				least_i = 0;
			
				int i =0;
				do{
					if(plists[i]->ft < least_ft){
						least_ft = plists[i]->ft;
						least_i = i;
					}				
					i++;
				}while(plists[i]->currPost != did);
				
						
				plists[least_i]->next(did );
				popdown(plists,least_i, number_of_terms);	
			}
		}
		else 	// pick the term with the smallest ft, advance the pointer like in BMW paper.
		{	
			
			if( pivot < number_of_terms - 1)
				did = minpivot > plists[ pivot + 1 ]->currPost ? plists[ pivot + 1 ]->currPost : minpivot; 
			else
				did = minpivot;

			if( did <= plists[pivot]->currPost)
				did = plists[pivot]->currPost + 1;
				
			if(did > MAXD)
				break;
			
			// pick the term with the smallest ft
			least_ft = plists[0]->ft;
			least_i = 0;
			for(int i = 1; i < pivot + 1; i++)
			{
				if( plists[i]->ft < least_ft)
				{
					least_ft = plists[i]->ft;
					least_i = i;
				}
			}
		    //advance the pointer
			plists[least_i]->next(did );	
			popdown(plists, least_i, number_of_terms);	
		
		}
		
	}
	

	for (int i = 0; i < number_of_terms; ++i){	delete plists[i];}
	delete[] plists;

	return candidate_results;
}

//****************************** FIM DA BMW ***************************


//================================ BMW-CS =======================================
vector<result> * queryprocessor::processQueryBMW_CS(Index *index, 
		Index *secIndex, 
		int number_of_terms, 
		vector<Term> &terms, 
		int &number_of_results, 
		float &ideal_doc_score,
		PostingList** plists){


		unsigned int MAXD = std::numeric_limits<unsigned int>::max();
	int pivot =0;
	int did,minpivot,tempd, f, least_ft, least_i;
	int k = this-> RESULT_HEAP_SIZE;
	int pos =0;

	float score = 0.0f;
	float threshold =0.0f;
	float s, ms, ms_aux, scoret,maxposs, upper;
	s= ms= ms_aux= scoret= maxposs= upper=0;

	vector<result> *candidate_results;
	candidate_results = new vector<result>();
	candidate_results->reserve(RESULT_HEAP_SIZE);
	
	vector<result> *candidate = new vector<result>();
	candidate->reserve(RESULT_HEAP_SIZE*10);

	if (number_of_terms == 0) {return new vector<result>(); }
	
	PostingList **secPlists = getPostingLists(secIndex, terms, number_of_terms);
	plists = getPostingLists(index, terms, number_of_terms);
	
	if(plists == NULL){return new vector<result>();}

	//Remove terms that the size of the list is 0
	for (int i = 0; i < number_of_terms; ++i) {
		int j;
		while((plists[i]->tam_lista == 0) && (i<number_of_terms)) {
			delete plists[i];
			for(j=i ; j<number_of_terms-1;j++) {
				plists[j] = plists[j+1];
				secPlists[j] = secPlists[j+1];
			}
			number_of_terms--;
		}
	}
	//If not remain terms in the query, the process ends.
	if(number_of_terms == 0) {
		delete plists;
		delete secPlists;
		return new vector<result>();
	}

	/*Computes a piece of BM25 score.
	 *Puts the pointer at the beginning of each list.
	 *Saves the maximum weight of the second tier.
	 * */
	for (int i = 0; i < number_of_terms; ++i) {
		plists[i]->pre = secPlists[i]->pre = (float)3 * (float)(log( 1 + (float)(index->getNDocs() - plists[i]->ft + 0.5)/(float)(plists[i]->ft + 0.5))/log(2));
		plists[i]->first();   
		secPlists[i]->first();
		ms_aux += plists[i]->min_score;	
	}

	//Sort the lists to each other by the current document.
	sortPostingsCS(plists, secPlists, number_of_terms);

	///Phase one. Selects candidates documents
	while( true ){

		// find the pivot term 
		pivot = -1;
		s = 0.0f;
		ms = ms_aux;
		upper=0.0f;

		//Selects PIVO - the document that the accumulated upperbound exceeds the threshold
		for(int i = 0; i < number_of_terms; i ++){
			s  += plists[i]->max_score;
			ms -= plists[i]->min_score;
			upper = s + ms;

			if( upper >= threshold){
				while( i + 1 < number_of_terms && plists[i + 1]->currPost == plists[i]->currPost)	 {i++; ms -= plists[i]->min_score; }
				pivot = i;
				break;
			}					
		}

		// pivot is -1 or MAXD, no item will make into topK 
		if(pivot == -1 || plists[pivot]->currPost == MAXD){	break; 	}
		
		did = plists[pivot]->currPost ;
		
		
		//Use shallow pointers to check the upperScore of pivot  [proposed in the BMW paper.]
		//minpivot is used to skip entries if blockmaxScore not exceed the threshold.
		maxposs = plists[pivot]->buffer_skip_max_score[ plists[pivot]->pos_skip];	
		minpivot = plists[pivot]->buffer_skip_docid[ plists[pivot]->pos_skip ] > MAXD ? MAXD : plists[pivot]->buffer_skip_docid[ plists[pivot]->pos_skip ]+1;
		for(int i = 0; i < pivot; i++){
			tempd = movepointer(plists[i], did);			
			if( minpivot >  tempd)
				minpivot = tempd;

			if(plists[i]->checkblock < plists[i]->tam_skip)
				maxposs += plists[i]->buffer_skip_max_score[ plists[i]->checkblock];
		}
				
		//upper is the BlockMaxScore
		upper = maxposs + ms;

		if(upper >= threshold ){
			// Evaluate this docID ('did') only when 'did' is equal to plists[0]->currPost 
			if( did == plists[0]->currPost ){
				score = 0.0f;
				
				//Computing the score.
				for(int i = 0 ; i < pivot + 1; i++) {
					f = plists[i]->getFreq();
					scoret = calcScore( plists[i]->pre, terms[i].idf , (float)(f),index->getDocNorm(did));
					score+=scoret;
					upper -= ( plists[i]->buffer_skip_max_score[ plists[i]->pos_skip ] - scoret );

					if( !(upper >= threshold) )
						break;
				}
				if(score ==0)exit(1);
				
				//Inserts the document in the ranking to update the threshold.
				if(score>=threshold){
					insereNoRanking(k,  &pos, &threshold, result(did, score, pivot+1), candidate_results);
				}
				
				upper = score+ms;
				//Add to the list of candidates
				if (upper >= threshold){
					
					if(candidate->size() > RESULT_HEAP_SIZE && (*candidate)[0].upper_score <= threshold) {
						pop_heap (candidate->begin(),candidate->end()); 
						candidate->pop_back();
					}
					candidate->push_back( result(did, score, upper, pivot+1/*, st*/) );
					push_heap(candidate->begin(),candidate->end());
				}

				/*Move the pointers and reorders the lists*/
				for(int i = pivot ; i > -1; i--){
					plists[i]->next(did + 1);
					popdownCS(plists, secPlists, i, number_of_terms);
				}
			}
			else{ //Advances the pointer  of one of the list before the pivot list to pivo document.

				least_ft = plists[0]->ft;
				least_i = 0;
				
				int i =0;
				do{
					if(plists[i]->ft < least_ft){
						least_ft = plists[i]->ft;
						least_i = i;
					}				
					i++;
				}while(plists[i]->currPost != did);
				
				plists[least_i]->next(did);
				popdownCS(plists, secPlists, least_i, number_of_terms);					

			}
		}
		else{ //Advances the pointer of one of the lists before the pivot list.
			least_ft = plists[0]->ft;
			least_i = 0;
			for(int i = 1; i < pivot + 1; i++)
			{
				if( plists[i]->ft < least_ft)
				{
					least_ft = plists[i]->ft;
					least_i = i;
				}
			}

			if( pivot < number_of_terms - 1)
				did = minpivot > plists[ pivot + 1 ]->currPost ? plists[ pivot + 1 ]->currPost : minpivot; 
			else
				did = minpivot;

			if( did <= plists[pivot]->currPost)
				did = plists[pivot]->currPost + 1;
			if(did > MAXD)
				break;
				
			plists[least_i]->next(did );
			popdownCS(plists, secPlists, least_i, number_of_terms);	
		}
	}
	
	mrr_total+= candidate->size();
	vector<result> *s_candidate = new vector<result>();
	s_candidate->reserve(RESULT_HEAP_SIZE*10);
	
	//Remove from the list of candidates documents that do not exceed the current threshold
	for(vector<result>::iterator it = candidate->begin(); it!= candidate->end(); it++) {
		if (threshold <= it->upper_score ){ s_candidate->push_back(*it);}
	}
	delete(candidate);
	//Sort the list of candidates by the id of the document.
	sort(s_candidate->begin(), s_candidate->end(), &resultDocIdComparator);


	/**
	 * Phase two. Computes the full score  
	 * */
	pos = 0;
	score =0.0f;
	(*candidate_results).clear();

	int totalTerm = number_of_terms;
	
	////Remove terms that the size of the second list is 0
	for (int i = 0; i < number_of_terms; ++i) {
		int j;
		while((secPlists[i]->tam_lista == 0) && (i<number_of_terms)) {
			delete secPlists[i];
			for(j=i ; j<number_of_terms-1;j++) {
				secPlists[j] = secPlists[j+1];
			}
			number_of_terms--;
		}
	}

	for (int i = 0; i < number_of_terms; ++i) {
		secPlists[i]->first();
	}
	
	vector< result >::iterator it;
	//For each candidate For each candidate, calculate full score and updated ranking.vector< result >::iterator it;
	for(it = s_candidate->begin(); it!= s_candidate->end(); it++){
		
		//Documents in all terms already have full score, then try to insert in the final ranking
		if(it->num_termos == totalTerm)	{	
			if(it->score >= threshold)
				insereNoRanking(k,  &pos, &threshold, *it, candidate_results);
			continue;
		
		}else{
			upper = 0.0f;
			score = 0.0f;
			
			//Use shallow pointers in the second tier to check the upperScore of candidate
			for (int j = 0; j < number_of_terms; j++){
				movepointer(secPlists[j], it->doc_id);
				
				if(secPlists[j]->checkblock < secPlists[j]->tam_skip)
					upper+=secPlists[j]->buffer_skip_max_score[ secPlists[j]->checkblock];
			}
			
			//If the new upperScore exceed the threshold, the block is accessed to compute the full score
			if(upper+it->score >= threshold){ /**/

				for (int j = 0; j < number_of_terms; j++){

					if(secPlists[j]->currPost < it->doc_id) {
						secPlists[j]->next(it->doc_id);
					}
					
					if(secPlists[j]->currPost == it->doc_id){
						it->num_termos++;
						f = secPlists[j]->getFreq(); 
						scoret = calcScore( secPlists[j]->pre, terms[j].idf , (float)(f),index->getDocNorm(it->doc_id));
						score +=scoret;
					}
				}
				it->score += score;
				
				//Insert in the final ranking if exceed the threshold
				if(it->score >= threshold){
					insereNoRanking(k,  &pos, &threshold, *it, candidate_results);
				}
			}
		}
	}

	for (int i = 0; i < number_of_terms; ++i) { 	
		delete secPlists[i];	
	}
	delete[] secPlists;
	
	for (int i = 0; i < number_of_terms; ++i) {	
		delete plists[i];	
	}
	
	delete[] plists;
	delete(s_candidate);
	
	return candidate_results;
}
