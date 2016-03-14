/*
 * PostingList.h
 *
 */

#ifndef POSTINGLIST_H_
#define POSTINGLIST_H_
#define IN_MEM

extern unsigned int t1;
extern unsigned int t2;
extern unsigned int mm;
extern unsigned int total_clock;

#include <limits>
#ifndef IN_MEM
#include "LeitorLista.h"
#endif
#include "../../libs-iw/indexer/include/IndiceInvertido.h"
#include "../../libs-iw/indexer/include/prefixinmem.h"

typedef struct doc {
	/*
	 unsigned int tf_url;
	 unsigned int tf_title;
	 unsigned int tf_anchor;
	 unsigned int tf_text;
	 */
	unsigned int tf;
	unsigned int doc_id;

	doc() {
	}

	doc(unsigned int _doc_id, unsigned int _tf) :
			tf(_tf), doc_id(_doc_id) {
	}
	/*
	 doc(unsigned int doc, unsigned int _tf_url, unsigned int _tf_title, unsigned int _tf_anchor, unsigned int _tf_text) :
	 tf_url(_tf_url),
	 tf_title(_tf_title),
	 tf_anchor(_tf_anchor),
	 tf_text(_tf_text),
	 doc_id(doc)
	 { tf = _tf_url + _tf_title + _tf_anchor + _tf_text;}
	 */
} doc;

class PostingList {
public:

#ifndef IN_MEM
	PostingList(LeitorLista *_leitor_lista);
#else
	PostingList();
#endif

	virtual ~PostingList();
	char freqDecoded;
	unsigned int bufDocs[513];
	unsigned int *bufFreqs;
	//	doc bufBloco[513];
	unsigned tBloco;
	int term_id;
	//float idf = 0.0f;
	int pBloco;
	int checkblock;
	unsigned int *plista;
	_tipo_buffer *out, *ini;
	unsigned char btat;
	unsigned int currPost;
	int currFreq;
	unsigned int *buffer_skip_docid;
	long *buffer_skip_offset;
	unsigned char *buffer_skip_bitat;
	float *buffer_skip_max_score;
	int pos_skip;
	int tam_skip;
	long tam_lista;
	//	unsigned int lastDoc;
	int pos_lista;
	long int total_descomprimido;
	long int total_bloco_lido;
	unsigned int Eob;
	float max_score;
	float min_score;
	float mil_score;
	unsigned int ft;
	float pre;
	unsigned int cont;
	int media_salto;
	unsigned int docs_processed;

	void init(); //TODO: necessario?
	void close();

	float getBlockMaxScore() {

		if (tam_skip == 0 || pos_skip == tam_skip)
			return max_score;
		return buffer_skip_max_score[pos_skip];
	}

	float getBlockMinScore() {
		return min_score;
	}

	///Retorna o proximo BlockMaxSxore da skiplist
	float nextShallow_todos() {
		while (pos_skip < tam_skip) {
			pos_skip++;
			break;
		} //nexttodos
		return this->getBlockMaxScore();
	}

	/*
	 Move o ponteiro da skiplist para o bloco com o docid>=doc_skip sem descomprimir nada
	 */

	inline void nextShallow(unsigned int doc_skip) {
		/*Calcula entre qual posicao da skiplist a lista invertida esta*/
//		(pos_lista > 0) ? (pos_skip = (pos_lista-1)/salto_skip) : (pos_skip = 0);
		/*Salta pela skip ateh encontrar uma entrada maior que o doc_skip*/
		while (pos_skip < tam_skip && buffer_skip_docid[pos_skip] <= doc_skip)
			pos_skip++;
	}

	inline bool isEmptyPostingList() {
		return tam_skip == 0;
	}

	unsigned int getDocBlockBoundary() {
		if (tam_skip == 0)
			return currPost + 1;

		if (pos_skip >= tam_skip && buffer_skip_docid[tam_skip - 1] < currPost)
			return currPost + 1;

		if (pos_skip >= tam_skip)
			return buffer_skip_docid[tam_skip - 1];

		return buffer_skip_docid[pos_skip];
	}

	/*
	 inline bool isEmptyPostingList() {
	 return this->leitorLista == NULL;
	 }
	 */

	static inline int END_OF_LIST_DOC_ID() {
		return std::numeric_limits<unsigned int>::max();
	}

	inline float getMaxScore() {
		return max_score;
	}

#ifndef IN_MEM
	LeitorLista *leitorLista;
#endif
	//	LeitorListaMem *leitorLista;
	//#endif

	void InicializaSkip(unsigned int *_plista, long _tam_lista, long _inicio_lista, char *pskip, long _tam_skip, long _inicio_skip, int _term_id) {
		Inicializa(_plista, _tam_lista, _inicio_lista);
		this->term_id = _term_id;
		//	  if(_lastDoc == -1) lastDoc = 0;
		/*Le o vetor de docid, offset e bitat*/
		buffer_skip_docid = (unsigned int*) (pskip + _inicio_skip);
		buffer_skip_offset = (long*) (pskip + _inicio_skip + sizeof(unsigned int) * _tam_skip);
		buffer_skip_bitat = (unsigned char*) (pskip + _inicio_skip + (sizeof(unsigned int) + sizeof(long)) * _tam_skip);
		buffer_skip_max_score = (float*) (pskip + _inicio_skip + (sizeof(unsigned int) + sizeof(long) + sizeof(unsigned char)) * _tam_skip);

		pos_skip = 0;
		tam_skip = _tam_skip;

	}
	
	void InicializaSkipTemp() {
		pos_skip = 0;
		out = ini;
		btat = 0;
		currPost = 0;
		pos_lista = 0;

	}

	void Inicializa(unsigned int *_plista, long _tam_lista, long _inicio_lista) {
		plista = (unsigned int*) _plista + (_inicio_lista / sizeof(unsigned int));
		out = (unsigned int*) plista;
		ini = out;
		btat = 0;
		tam_lista = _tam_lista;
		currPost = 0;
		pos_lista = 0;
	}

	inline void nextShallow(unsigned int doc_skip, int d) {
		nextShallow(doc_skip);
	}

	/*
	 Le uma lista invertida
	 */
	int getDoc3(unsigned int *doc, unsigned int *info) {
		//	currPost +=  RawDeltaDecode(out, btat);
		currPost = *(unsigned int*) out;
		out++;

		//	*info =  RawDeltaDecode(out, btat);
		*info = *(unsigned int*) out;
		out++;

		*doc = currPost;

		return 1;
	}

	inline void endList()
	{
		currPost = END_OF_LIST_DOC_ID();
	}

int simula;
	/*****************************
	 Le a lista invertida aplicando saltos com a skiplist. Busca o primeiro docid >= doc_skip
	 *******************************/
	inline void next(unsigned int doc_skip) {

		//doc_skip = Eob;///--------

		if (doc_skip > Eob) {

			if (pos_skip == tam_skip - 1) {
				currPost = END_OF_LIST_DOC_ID();
				return;
			}

			while (buffer_skip_docid[pos_skip] < doc_skip) {
				pos_skip++;
				if (pos_skip == tam_skip) {
					currPost = END_OF_LIST_DOC_ID();
					return;
				}
			}
			
			tBloco = 0;
			pBloco = -1;
			freqDecoded = 0;
			
			/*Salta com o cursor sobre a lista*/
			out = (unsigned int*) (&plista[buffer_skip_offset[pos_skip - 1]]);

			/*Decodifica o codigo do docId da skiplist*/
			bufDocs[0] = (*(unsigned int *) out);
			out++;
			
			//simula = 0;
			
			/**
			for (int i = 0; i < 100; i++) //SIMULA CUSTO DE DESCOMPRIMIR
			{
				simula = simula * 2;
				cout<<"";
			}
			/**/
			
			//total_bloco_lido++;
			total_descomprimido++;

			Eob = buffer_skip_docid[pos_skip];  //<---

			while (bufDocs[tBloco] < Eob) {
				tBloco++;
				bufDocs[tBloco] = (*(unsigned int *) out);
				out++;
				//	bufBloco[tBloco].tf = RawDeltaDecode(out, btat);
			}
			tBloco++;
			bufDocs[tBloco] = END_OF_LIST_DOC_ID();
		} //antigo if do mudou de bloco vem aqui

		///AQUI BUSCA NO BLOCO DESCOMPRIMIDO!
		/*Enquanto a lista nao termina, procura por um docId >= doc_skip*/
		
		pBloco++;  // passa para o proximo doc da lista
		
		/*		
		while(pBloco+30 < tBloco){
			if(bufDocs[pBloco+30] < doc_skip){
				pBloco=pBloco+30;
			}else{
				break;
			}
		}
		*/
		while (bufDocs[pBloco] < doc_skip){	
			pBloco++;
		}
		
		currPost = bufDocs[pBloco];
		return;
	}
	
	int contador = 0;
	inline void mostraLista() {
		pos_skip++;
		
		out = ini-1;
		/*Salta com o cursor sobre a lista*/
		//out = (unsigned int*) (&plista[buffer_skip_offset[pos_skip - 1]]);
		
		cout<< "\n#Tam_lista : "<< tam_lista << " term_id : "<< term_id <<endl ;
		for (int i = 0; i < (128*2)*20; i++)
		{
			contador ++;
			cout<< " - " << (*(unsigned int *) out);
			if (i%128 == 0 && i>0){
				cout<<endl;
			}
			out++;
		}
		
		cout<< "\n------------------------\n" <<endl;
		cout<<"# Documentos : "<< contador/2 <<endl;
		
	}

	int getCurrentPost() {
		return currPost;
	}

	int getFreq() {

		/*   if((pBloco >= tBloco)|| (pBloco <0)) {
		 printf("erro na getfreq pBloco %d tBloco %d\n",pBloco,tBloco);
		 exit(1);
		 }*/
		if (freqDecoded) {
			return bufFreqs[pBloco];
		} else { // decodifica frequencias do bloco. Versao futura pode simplificar a compressao da frequencia para acelerar esse processo. Ex: tamanho fixo de 1 byte para a frequencia
			freqDecoded = 1;
			bufFreqs = (unsigned int *) out;
			/* int x;
			 for(x =0; x< tBloco; x++)       {
			 bufFreqs[x] = (*(unsigned int *) out); out++;  
			 } */
			return bufFreqs[pBloco];
		}
	}

	void first() {

		out = ini;
		btat = 0;
		checkblock = 0;
		if (tam_skip <= 0) {  // estava lastDoc!
			currPost = END_OF_LIST_DOC_ID();
			return;
		}

		tBloco = 0;
		freqDecoded = 0;
		// bufDocs[0] = RawDeltaDecode(out, btat);
		bufDocs[0] = (*(unsigned int*) out);
		
		out++;
		if (tam_skip == 0) {
			printf("nunca deve acontecer %d  First %d ft %d\n", tam_lista, bufDocs[0], ft);
			exit(1);
		} else {
			Eob = buffer_skip_docid[0];
		}
		
		while (bufDocs[tBloco] < Eob) {
			tBloco++;
			//  bufDocs[tBloco] = bufDocs[tBloco-1]+ RawDeltaDecode(out, btat);
			bufDocs[tBloco] = (*(unsigned int*) out);
			out++;
			if (tBloco > 127) {
				printf("erro na descomprime!\n");
				exit(1);
			}
		}
		//total_descomprimido++;
		tBloco++;
		bufDocs[tBloco] = END_OF_LIST_DOC_ID();

		pBloco = 0;
		currPost = bufDocs[0];

		pos_skip = 0;
	}

	void first_() {

		out = ini;
		btat = 0;
		checkblock = 0;
		if (tam_skip <= 0) {  // estava lastDoc!
			currPost = END_OF_LIST_DOC_ID();
			return;
		}

		tBloco = 0;
		freqDecoded = 0;
		// bufDocs[0] = RawDeltaDecode(out, btat);  
		cout << "term_id : " << term_id << endl;
		cout << "out : " << out << endl;
		bufDocs[0] = (*(unsigned int*) out);
		out++;
		
		if (tam_skip == 0) {
			printf("nunca deve acontecer %d  First %d ft %d\n", tam_lista, bufDocs[0], ft);
			exit(1);
		} else {
			Eob = buffer_skip_docid[0];
		}
		//            printf("lendo bloco");

		while (bufDocs[tBloco] < Eob) {
			tBloco++;
			//  bufDocs[tBloco] = bufDocs[tBloco-1]+ RawDeltaDecode(out, btat);  
			bufDocs[tBloco] = (*(unsigned int*) out);
			out++;

		}
		tBloco++;
		bufDocs[tBloco] = END_OF_LIST_DOC_ID();

		pBloco = 0;
		currPost = bufDocs[0];

		pos_skip = 0;
	}

};

#endif /* POSTINGLIST_H_ */
