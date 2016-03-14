#ifndef SIMPLE_WBR_QUERY_PARSER_H_
#define SIMPLE_WBR_QUERY_PARSER_H_

//#ifndef _STEMMER
//#define _STEMMER
//#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Vocabulario.h"
#include "valstringuniao.h"
#include "semaphore.h"
#ifdef _STEMMER
#include "PorterStemmer.h"
#endif
#define TAM_PALAVRA 30

class SimpleWBRQueryParser 
{
public:
	int *parse_query(const char *str_query, Vocabulario *voc, int *num_termos)
	{
		unsigned int k;
		char pal[TAM_PALAVRA+1], charBase;
		int *termos_ids = new int [1000];
		unsigned char c;
		int ntermos_ids;
		int id;

#ifdef _STEMMER
        PorterStemmer pStemmer;
#endif
		memset(pal,0,TAM_PALAVRA+1);
		k=0;
		pal[k] = '\0';
		ntermos_ids = 0;
	//	printf("consulta: %s\n",str_query);
		while(*str_query)
		{
			
			//cout<<*str_query; //Imprime termo da consulta
			
			
			charBase = *str_query;
			c =  _ConvertSemAcentos[(unsigned char) charBase];
			if(isalnum(c))
			{
				if(k < TAM_PALAVRA)
				{
					pal[k] = tolower(c);
					k++;
				}
			}
			else if(k>0)
			{
				pal[k] = '\0';
				//	printf("palavra %s\n",pal);
#ifdef _STEMMER
                pStemmer.stem((char*)pal, k);
#endif
		//printf("palavra %s %d\n",pal,k);
				id = voc->getId((char*)pal, k, 0);
				//	printf("id %d\n",id);
//				if(id == -1)
//				{
//					delete[] termos_ids;
//					return NULL;
//				}
				if(id != -1) {
				  char tmp[50];
				  strncpy(tmp,(char*)pal,k);
				  tmp[k]=0;
				  //  printf("palavra %s %d\n",tmp,id);				
				  termos_ids[ntermos_ids++] = id;
				  //cout<<" - "<<id<<endl; //Imprime ID do termo 
				}
				
				k = 0;
			}
			str_query++;
		}
		if(k>0)
		{
			pal[k] = '\0';
#ifdef _STEMMER
            pStemmer.stem((char*)pal, k);
#endif
			id = voc->getId((char*)pal, k, 0);
//			if(id == -1)
//			{
//				delete[] termos_ids;
//				return NULL;
//			}
			if(id != -1) {
			  char tmp[50];
			  strncpy(tmp,(char*)pal,k);
			  tmp[k]=0;
			  //		  printf("palavra %s %d\n",tmp,id);
			   // cout<<" - "<<id<<endl;
				termos_ids[ntermos_ids++] = id;
			}
		}
		*num_termos = ntermos_ids;
		return termos_ids;
	}
	
	
};

#endif
