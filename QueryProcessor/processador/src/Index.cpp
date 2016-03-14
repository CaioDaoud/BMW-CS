/*
 * Index.cpp
 *
 *  Created on: 21/09/2010
 *      Author: felipe
 */

#include "Index.h"
#include <string>

FILE *abre_arquivo(char *filename)
{
    FILE *fp = fopen(filename, "r");
    if(fp == NULL)
    {
        printf("Problemas na abertura do arquivo [%s]\n", filename);
        exit(0);
    }

    return fp;
}

Index::Index(const char *_index_base_path, IndexType _type, int topK )
{
    Index(_index_base_path, _type, topK, NULL);
}

Index::Index(const char *_index_base_path, IndexType _type, int topK, const char *_base_path )
{
    FILE *fp;
    long int size;
    long int num_term;
    this->type = _type;
    this->index_base_path = _index_base_path;
    this->base_path = _base_path;
    char arq[4096];

    /*Carrega o IDX*/
    //	sprintf( arq, "%sidx", this->index_base_path );
    sprintf( arq, "%sidx", this->index_base_path );

    this->idxFile = abre_arquivo(arq);

    fseek(this->idxFile, 0 , SEEK_END);
    size = ftell(this->idxFile)/sizeof(IDX);
    vetIdx = new IDX[size];

    cout << "Tamanho do indice: " << size << endl;

    fseek(this->idxFile, 0 , SEEK_SET);
    fread(vetIdx, size, sizeof(IDX), this->idxFile);

    this->number_of_terms_index = size; //pegando numero de termos no indice


    /*Carrega o LastDoc*/
    //	sprintf( arq, "%sidx.lastdoc", this->index_base_path );
    //	fp = abre_arquivo(arq);
    //fseek(fp, 0, SEEK_END);
    //size = ftell(fp)/sizeof(int);
    //
    //vetLastDoc = new int[size];
    //fseek(fp, 0, SEEK_SET);
    //fread(vetLastDoc, size, sizeof(float), fp);
    //fclose(fp);

    /*Carrega o idf*/
    sprintf( arq, "%sidf", this->index_base_path );
    fp = abre_arquivo(arq);
    fseek(fp, 0, SEEK_END);
    size = ftell(fp)/sizeof(float);
    vetFT = new unsigned int[size];
    vetIdf = new float[size];
    vetMaxScore = new float[size];
    vetMinScore = new float[size];
    vetMilScore = new float[size]; //quarda o milésimo maior score de cada termo

    memset(vetIdf, 0, sizeof(float) * size);
    memset(vetMinScore, 0, sizeof(float) * size);
    memset(vetMaxScore, 0, sizeof(float) * size);
    memset(vetMilScore, 0, sizeof(float) * size);

    fseek(fp, 0, SEEK_SET);
    fread(vetIdf, size, sizeof(float), fp);
    fclose(fp);


    /*Carrega o FT*/
    sprintf( arq, "%sFT", this->index_base_path );
    fp = abre_arquivo(arq);
    fread(vetFT, size, sizeof(unsigned int), fp);
    fclose(fp);


    /*Carrega os scores maximos de cada lista*/
    sprintf( arq, "%smax_score", this->index_base_path );
    fp = abre_arquivo(arq);
    fread(vetMaxScore, size, sizeof(float), fp);
    fclose(fp);

    /*Carrega os scores minimos de cada lista*/
    sprintf( arq, "%smin_score", this->index_base_path);
    fp = fopen(arq, "r");
    if(fp) {
        fread(vetMinScore, size, sizeof(float), fp);
        fclose(fp);
    }

    sprintf( arq, "%sdist_max_min_score", this->index_base_path );

    FILE *fm=fopen(arq, "w");
    for(int i=0; i<size; i++) {
        fprintf(fm, "%.4f %.4f\n", vetMaxScore[i], vetMinScore[i]);
    }
    fclose(fm);

    /*Carrega o milesimo maior score de cada lista*/
    //sprintf( arq, "%sMilMax_score", this->index_base_path );
    //sprintf( arq, "%sdecimo_maxScore", this->index_base_path );
    //sprintf( arq, "../../../comun.mil_max_score");
    /***/	
    sprintf( arq, "%s%dMax_score",this->base_path, topK);
    cout<<arq<<endl;
    fp = fopen(arq, "r");
    if(fp) {
        fread(vetMilScore, size, sizeof(float), fp);
        printf("Abrindo arquivo [%s]\n", arq); 
        fclose(fp);
    }else{
        printf("Problemas na abertura do arquivo [%s]\n", arq);
        //exit(0);
    }

    /*Carrega a norma*/
    // abaixo se for usar o indice com o fix
    sprintf( arq, "%snorma", this->index_base_path );
    //sprintf( arq, "%snorma", this->index_base_path );
    fp = abre_arquivo(arq);
    fseek(fp, 0, SEEK_END);
    size = ftell(fp)/sizeof(float);
    this->nDocs = size;
    vetNorm = new float[size];
    fseek(fp, 0, SEEK_SET);
    fread(vetNorm, size, sizeof(float), fp);
    fclose(fp);

#ifdef IN_MEM
    /*Mapeia o indice para a memoria*/	
    loadToMainMemory();
#endif
}	

Index::~Index()
{
    fclose(this->idxFile);

    delete[] vetNorm;
    delete[] vetIdf;
    delete[] vetIdx;
    delete[] vetMaxScore;
    delete[] vetMinScore;
    delete[] vetMilScore;
}

/*Carrega para memoria todo o indice invertido*/
void Index::loadToMainMemory()
{
    long total = 0;
    int number_of_arq = vetIdx[this->number_of_terms_index-1].id_arq+1;

    ptr_index = (unsigned char**)malloc(sizeof(unsigned char*) * number_of_arq);
    ptr_skip = (unsigned char**)malloc(sizeof(unsigned char*) * number_of_arq);

    for(int i = 0; i < number_of_arq; i++)
    {
        char arq[1000], arq_skip[1000];
        //sprintf( arq, "%sfrequencia%d", this->index_base_path, i );
        // abaixo para usar com indice corrigido do +1
        sprintf( arq, "%sfrequencia%d.newFormat", this->index_base_path, i );
        //sprintf( arq_skip, "%sfreq_skip%d", this->index_base_path, i );
        sprintf( arq_skip, "%sfreq_skip%d", this->index_base_path, i );

        FILE *fpIndex = fopen(arq, "r");
        if(fpIndex == NULL) {
            printf("erro de abertura de arquivo %s\n",arq);
            exit(1);
        }
        fseek(fpIndex, 0, SEEK_END);
        long len_index = ftell(fpIndex);
        ptr_index[i] = (unsigned char*)malloc(sizeof(char) * len_index);
        fseek(fpIndex, 0, SEEK_SET);
        fread(ptr_index[i], sizeof(char), len_index, fpIndex);
        fclose(fpIndex);

        FILE *fpSkip = fopen(arq_skip, "r");
        if(fpSkip == NULL) {
            printf("erro de abertura de arquivo %s\n",arq_skip);
            exit(1);
        }
        fseek(fpSkip, 0, SEEK_END);
        long len_skip = ftell(fpSkip);
        ptr_skip[i] = (unsigned char*)malloc(sizeof(char) * len_skip);
        fseek(fpSkip, 0, SEEK_SET);
        fread(ptr_skip[i], sizeof(char), len_skip, fpSkip);
        fclose(fpSkip);

        total += len_skip + len_index;
    }

    cout << "Carregados " << total << " dados de indice" << endl;
}

PostingList* Index::getPostingList(int term_id)
{
    if (term_id > this->number_of_terms_index) return new PostingList(); //TODO: checar boundaries disso aqui. Não seria number_of_terms_index - 1 ?
    IDX idx = vetIdx[term_id];

    if (idx.freq_tam_lista == 0)	return new PostingList();

#ifdef IN_MEM

    //Pega a lista invertida a partir de um ponteiro da memoria
    PostingList *y = new PostingList();

    //printf("---------------> idx.freq_inicio_skip %d\n", idx.freq_inicio_skip);
    y->InicializaSkip((unsigned int*)(ptr_index[idx.id_arq]), idx.freq_tam_lista, idx.freq_inicio_termo, (char*)(ptr_skip[idx.id_arq]), idx.freq_tam_skip, idx.freq_inicio_skip,term_id);


    //	y->size_list = idx.freq_tam_lista;		//Tamanho da lista
    y->ft = vetFT[term_id]; //pega o tamanho completo da lista (ft);	
    y->max_score = vetMaxScore[term_id];	//Maior Score da lista
    y->min_score = vetMinScore[term_id];    //Menor Score da lista
    y->mil_score = vetMilScore[term_id];	//Milesimo maior score da lista
    return y;
#else

    //Pega a lista invertida a partir de um offset de arquivo

    LeitorLista *x = new LeitorLista();

    char arq[1000], arq_skip[1000];
    //	sprintf( arq, "%sfrequencia%d", this->index_base_path, idx.id_arq );
    //sprintf( arq_skip, "%sfreq_skip%d", this->index_base_path, idx.id_arq );
    // Abaixo codigos para indice corrigido
    sprintf( arq, "%sfrequencia%d.newFormat", this->index_base_path, idx.id_arq );
    sprintf( arq_skip, "%sfreq_skip%d", this->index_base_path, idx.id_arq );

    x->InicializaSkip(arq, idx.freq_tam_lista, idx.freq_inicio_termo, arq_skip, idx.freq_tam_skip, idx.freq_inicio_skip);

    PostingList *y = new PostingList(x);

    y->size_list = idx.freq_tam_lista;
    y->max_score = vetMaxScore[term_id];	//Maior Score da lista
    y->min_score = vetMinScore[term_id];	//Menor Score da lista
    y->mil_score = vetMilScore[term_id];	//Milesimo maior score da lista
    return y;
#endif
}

