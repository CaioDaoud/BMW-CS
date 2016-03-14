#include "snippets.h"

/*Ordena os Termos segundo a aparição no Documento*/
/*De outro modo, é uma ordenação por insercao simples*/
void wordMatchSort(wPattern * Terms) 
{
    int i, j;
    wType elem;
    wType *p;
    
    p = Terms->Pattern; i = 1;
    while(i < (int)Terms->nTerms) {
        elem = p[i];
        j = i - 1;

        while((j >= 0) && (elem.locate < p[j].locate)) {
            p[j+1] = p[j];
            j--;         
        }
        
        p[j+1] = elem;
        i++;         
    }
    
    return ;
}

/*Executa a extração dos tokens, parser, do Texto*/
char * parserText(char *word, unsigned int *length, char * pattern)
{
    unsigned int k, controle;
    unsigned char c;
    char charBase;
    char *start;
    
    k = 0; word[k] = '\0'; start = NULL;
    while( *pattern ) {
        controle = 0;
 
        if( *pattern < 0 ) {
            if( *pattern == -61) {
                controle = 1;
                pattern++;
            }        
        }
        
        if( controle ) {
            c = ConverteBufferUTF8( *pattern );
            controle = 0;
        }
        else {
            charBase = *pattern;
            c =  _ConvertSemAcentos[(unsigned char) charBase];
        }
        
        if(isalnum(c)) {
            if(k < MAXTAMPALAVRA) {
                word[k] = tolower(c);//acho que o _ConvertSemAcentos ja deixa 
                                     //minusculo. tem que testar
				k++;
			}
            if(!start) {
                start = pattern;           
            }        
        }
        
        else if(k>0) {
            word[k] = 0;
            *length = k;
            return start;
        }
		
        pattern++;
    
    }
    
    if(k>0) {
	    word[k] = 0;
		*length = k;
		return start;
	}
    
    return NULL;
}

/*Localiza os termos da consulta no documento*/
void locatePattern(wPattern * Terms, char * document)
{
    wType *p;
    unsigned int shift, i, out, length;
    char *c, *start, word[MAXTAMPALAVRA+1];
    
    p = Terms->Pattern;
    for(i = 0; i < Terms->nTerms; i++) {
        shift = 0; out = 0;
        memset(word, '\0', MAXTAMPALAVRA+1);
        
        while(!out) {
            start = (document + shift);//define onde ira começar o parser do
                                       //texto se shift = 0, parser a partir do
                                       //inicio
            c = parserText(word, &length, start);
            if(c) {
                if((p[i].length == length) && (memcmp(p[i].word, word, length) == 0)) {
                    p[i].locate = c - document; //se c != NULL e palavra
                                      //encontrada, calcula localizacao da 
                                      //palavra no documento
                    out = 1; 
                }
                else shift = length + (c - document); //se palavra nao
                                 //encontrada, calcula desvioe continua a busca    
            }
            else out = 1; //se nao houver mais palavras no documento e o termo 
                          //nao foi encontrado, termina o loop            
        }
    }
     
    return;     
}
     
/*Executa o parseamento da consulta*/
void parserPattern(wPattern * Terms, char * pattern)
{
    wType *p;
    unsigned int shift, i;
    char *c, *start;
     
    shift = 0; p = Terms->Pattern;
    for(i = 0; i < MAXNUMPALAVRA; i++) {
        //Inicializa os dados da estrutura word
        p[i].length = 0; //necessario caso p[i] nao defina uma palavra da query
        p[i].locate = -1;
        memset(p[i].word, '\0', MAXTAMPALAVRA+1);
         
        start = (pattern + shift);//define onde ira começar o parser do texto
                                  //se shift = 0, parser a partir do inicio
        c = parserText(p[i].word, &p[i].length, start);
        
        if(c) shift = p[i].length + (c - pattern);//caso c!=NULL calcula o 
                                                  //desvio
        else break;
         
        Terms->nTerms++; //se c != NULL incrementa número de termos da query
    }
     
    return ;
}

/*Executa remoção de '\n', '\t', '\r', e excesso ' '*/
unsigned int cleanText(char * text, unsigned int size_text)
{
    unsigned int i, j;
    char *buffer;
    
    size_text = strlen(text);
    buffer = (char *) malloc(sizeof(char)*size_text + sizeof(char));
    memset(buffer, '\0', size_text+1);
    memcpy(buffer, text, size_text);
    
    i = 0; j = 0;
    while(buffer[j] != '\0') {
        switch (buffer[j]) {
            case '\n' : text[i] = ' ';
                i++; j++;
                //while((buffer[j] != '\0') && (buffer[j] == '\n')) j++;
                break;
            case '\t' : text[i] = ' ';
                i++; j++;
                //while((buffer[j] != '\0') && (buffer[j] == '\t')) j++;
                break;
            case ' ' : text[i] = ' ';
                i++; j++;
                //while((buffer[j] != '\0') && (buffer[j] == ' ')) j++;
                break;
            case '\r' : j++;
                //while((buffer[j] != '\0') && (buffer[j] == '\r')) j++;
                break;
            default : if(buffer[j]!='\0') {text[i] = buffer[j];
                i++; j++;}
                break;
        }
    }
    
    text[i] = '\0';
    free(buffer);
    return (i - 2);
}

/*Realiza a extração do texto*/
void extractSnippet(wPattern * Terms, char * document, unsigned int * size_doc, 
                    char * snippet)
{
    wType *p;
    int found_t, next_found_t; //found_t variavel que proucra o primeiro termo 
                          //encontrado no documento
    int shift, remain;
    char *c, *start;
    
    //Caso o documento seja muito curto, retorna o conteudo dele integralmente
    if(*size_doc <= MAXTAMTRECHO) {
//	fprintf(stderr, "-----> 1\n");        
	memcpy(snippet, document, *size_doc);
	snippet[*size_doc - 1] = '\0';
        cleanText(snippet, MAXTAMTRECHO+100);
        //strcat(snippet, " ...");
        return ;
    }
   
    //procura o primeiro termo que foi encontrado no documento   
    found_t = 0; p = Terms->Pattern;
    while((found_t < (int)Terms->nTerms) && (p[found_t].locate == -1)) found_t++;
    
    //Verifica se o número de termos encontrado é no máximo 1 ou se os termos se encontram em uma distância menor que o trecho
    if(((Terms->nTerms - found_t) <= 1) || ((p[Terms->nTerms - 1].locate - p[found_t].locate) < (MAXTAMTRECHO - 30))) {
//	fprintf(stderr, "-----> 2\n");        
	if(found_t == (int)Terms->nTerms) --found_t;//verifica se não existe termo
        
        shift = 0;
        if(Terms->nTerms) shift = p[found_t].locate - 30; //calcula desvio
        
        if(shift < 0) start = document; //caso nao exista o termo ou se encontre
                 //no inicio do documento, retorna o snippet a partir do inicio
        else start = (char *) memchr((document + shift), ' ', *size_doc - shift) + 1; /*document + shift;*/
                 //do contrario calcula o inicio do snippet a partir do desvio
        
        remain = document + *size_doc - start; //calcula a distrancia do 
                           //comeco do snippet para o fim do documento
	//fprintf(stderr, "%d\n", remain);
        if((remain > 0) && (remain <= MAXTAMTRECHO)){ 
		memcpy(snippet, start, remain-1); //se for menor ou igual ao tamanho do trecho retorna tudo
		snippet[remain-1] = '\0';
	}
        else if(remain > 0) { 
            memcpy(snippet, start, MAXTAMTRECHO);
            c = strrchr(snippet, ' ');//elimina o último token, necessario pois
            snippet[c - snippet] = '\0';//pode ter copiado somente parte de uma
                                        //palavra
        }
        
	if(remain > 0) {
        	cleanText(snippet, MAXTAMTRECHO + 100);
       	 	strcat(snippet, " ...");
	}
        
        return;                      
    }
    else {
//	fprintf(stderr, "-----> 3\n");
	next_found_t = found_t + 1;
	while((p[next_found_t].locate - p[found_t].locate) <= 200) next_found_t++;

	shift = p[found_t].locate - 30; //calcula desvio
        
        if(shift < 0) start = document; //caso nao exista o termo ou se encontre
                 //no inicio do documento, retorna o snippet a partir do inicio
        else start = (char *) memchr((document + shift), ' ', *size_doc - shift) + 1; /*document + shift;*/
                 //do contrario calcula o inicio do snippet a partir do desvio
        
        remain = (document + *size_doc - 1) - start; //calcula a distrancia do 
                           //comeco do snippet para o fim do documento
        
        if((remain > 0) && (remain <= MAXTAMTRECHO/2)){ 
		memcpy(snippet, start, remain-1); //se for menor ou igual ao tamanho do trecho retorna tudo
		snippet[remain-1] = '\0';
	}
        else if (remain > 0){ 
            memcpy(snippet, start, MAXTAMTRECHO/2);
            c = strrchr(snippet, ' ');//elimina o último token, necessario pois
            snippet[c - snippet] = '\0';//pode ter copiado somente parte de uma
                                        //palavra
        }
        
	if(remain > 0) {
        	cleanText(snippet, MAXTAMTRECHO + 100);
        	strcat(snippet, " ...");
	}
        
        return;
        /*if((p[Terms->nTerms - 1].locate - p[found_t].locate) < (MAXTAMTRECHO - 30)) {
            shift = 0;
            if(Terms->nTerms) shift = p[found_t].locate - 30;
            
            if(shift < 0) start = document;
            else start = (char *) memchr((document + shift), ' ', size_doc) + 1;
            
            remain = (document + size_doc - 1) - start;
            
            if(remain <= MAXTAMTRECHO) memcpy(snippet, start, remain+1);
            
            else {
                memcpy(snippet, start, MAXTAMTRECHO);
                c = strrchr(snippet, ' ');
                snippet[c - snippet] = '\0';     
            }                    
        
            cleanText(snippet);
            strcat(snippet, " ...");
            
            return;
        }*/
    }
}

/*Extrai um resumo do texto baseado no padrão de consulta*/
char * getSnippet(char * document, char * pattern)
{
    unsigned int size_document;
    char *snippet;
    wPattern query_terms;
    
    //size_document = strlen(document);
    size_document = cleanText(document, size_document); //fprintf(stderr, "%s\n\n\n", document);//elimina /r, /t, /n do texto
    query_terms.nTerms = 0;
    
    snippet = (char*) malloc(sizeof(char)*(MAXTAMTRECHO+100));
    memset(snippet, '\0', MAXTAMTRECHO+5);
    
    //Inicia tabela de conversao de caracteres
    StartConvertSemAcentos();
    
    //Executa o Parser da Query
    parserPattern(&query_terms, pattern);
    
    //Executa a Localizacao dos Termos da Query no Documento        
    if(size_document) locatePattern(&query_terms, document);
    
    //Ordena os Termos por aparição no texto do Documento
    if(query_terms.nTerms) wordMatchSort(&query_terms);
    
    //Executa a extracao do texto que será o snippet
    if(size_document) extractSnippet(&query_terms, document, &size_document, 
                      snippet);
                           
    return snippet;     
}

