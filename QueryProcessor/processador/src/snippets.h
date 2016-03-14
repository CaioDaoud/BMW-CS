#ifndef SNIPPET_H
#define SNIPPET_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "valstringuniao_utf8.h"

#define MAXTAMPALAVRA 30
#define MAXNUMPALAVRA 10
#define MAXTAMTRECHO 300

typedef struct {
    char word[MAXTAMPALAVRA+1];
    unsigned int length;
    int locate;
} wType;

typedef struct {
    wType Pattern[MAXNUMPALAVRA];
    unsigned int nTerms;        
} wPattern;

/*Ordena os Termos segundo a aparição no Documento*/
/*De outro modo, é uma ordenação por insercao simples*/
void wordMatchSort(wPattern * Terms);

/*Executa a extração dos tokens, parser, do Texto*/
char * parserText(char *word, unsigned int *length, char * pattern);

/*Executa a extração dos tokens, parser, do Texto*/
char * parserText(char *word, unsigned int *length, char * pattern);

/*Localiza os termos da consulta no documento*/
void locatePattern(wPattern * Terms, char * document);

/*Executa o parseamento da consulta*/
void parserPattern(wPattern * Terms, char * pattern);

/*Executa remoção de '\n', '\t', '\r', e excesso ' '*/
unsigned int cleanText(char * text, unsigned int size_text);

/*Realiza a extração do texto*/
void extractSnippet(wPattern * Terms, char * document, unsigned int size_doc, char * snippet);

/*Extrai um resumo do texto baseado no padrão de consulta*/
char * getSnippet(char * document, char * pattern);

#endif
