#include <stdio.h>      /* printf */
#include <time.h>       /* clock_t, clock, CLOCKS_PER_SEC */
#include <math.h>       /* sqrt */
#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include "queryparser.h"
#include "queryprocessor.h"
#include "PostingList.h"
#include "utils/ConfigFile.h"
//#include "snippets.h"
#include <sys/time.h>
#include <ctime>
#include "queryparser.h"
#include "DocsInfo.h"
#include "parser/parse_query.h"
#include "Vocabulario.h"
#include "Index.h"


float totalTime;
struct timespec start_r, stop_r;
struct timespec star, sto;
                


int main(int argc, char **argv) {
	
	StartConvertSemAcentos(); //inicializar conversor de caracteres:

    if(argc < 11) {
        printf (" Execute\n\t%s <arq.query> <path.word> <file.result> <wand/bmw/cs> <VET/UTI/BM25> <0> <Top-k> <Entire.index> <First.index> <Second.index> <0>\n", argv[0]);
        
        printf ("tamanho = %d",argc);       
        
        exit(0);
    }
    
   
    //ConfigFile config(argv[2]);

    cout << "Starting Query Processor ...  " << endl;

	int id_formula = atoi(argv[6]);

    int process_mode;
    if(!strcmp(argv[4], "WAND") || !strcmp(argv[4], "wand")) {
        printf("Processing in mode WAND!\n");
        process_mode = 1;
    } else if(!strcmp(argv[4], "BMW") || !strcmp(argv[4], "bmw")) {
        printf("Processing in mode 	BMW!\n");
        process_mode = 2;
    } else  if(!strcmp(argv[4], "CSP") || !strcmp(argv[4], "csp")) {
        printf("Processing in mode CSP \n");
        process_mode = 3;
    } else  if(!strcmp(argv[4], "FLASH") || !strcmp(argv[4], "flash")) {
        printf("Processing in mode  BMW-FLASH \n");
        process_mode = 4;
    } else  if(!strcmp(argv[4], "BMWLAYER") || !strcmp(argv[4], "bmwlayer")) {
        printf("Processing in mode  BMW-Camdas de 2011 \n");
        process_mode = 5;
    }else  if(!strcmp(argv[4], "WAVE") || !strcmp(argv[4], "wave")) {
        printf("Processing in mode  WAVE \n");
        process_mode = 6;
    }else  if(!strcmp(argv[4], "HIBRIDO") || !strcmp(argv[4], "hibrido")) {
        printf("Processing in mode  HIBRIDO\n");
        process_mode = 7;
    }else  if(!strcmp(argv[4], "TWAVE") || !strcmp(argv[4], "twave")) {
        printf("Processing in mode  TWAVE\n");
        process_mode = 8;
    }else  if(!strcmp(argv[4], "TBMWLAYER") || !strcmp(argv[4], "tbmwlayer")) {
        printf("Processing in mode  TBMW\n");
        process_mode = 9;
    }else  if(!strcmp(argv[4], "CS") || !strcmp(argv[4], "cs")) {
        printf("Processing in mode  CS\n");
        process_mode = 10;
    }else{
		printf("modo não existe !\n");
        process_mode = 0;
	}

    int index_type;
    if(!strcmp(argv[5], "VET") || !strcmp(argv[5], "vet")) {
          printf("Using Vetorial score!\n");
          index_type = 3;
    }else if(!strcmp(argv[5], "UTI") || !strcmp(argv[5], "uti")) {
          printf("Using UTI score!\n");
          index_type = 1;
    } else {
          printf("Using BM25 score!\n");
          index_type = 2;
    }

	cout << "Carregando arquivo de query " << argv[1] << endl;


    
    Index *smallIndex=NULL;
    Index *SecondSmallIndex=NULL;
    Index *entireIndex=NULL;
    
    if(process_mode == 3 || process_mode == 4 || process_mode == 5 || process_mode == 6 || process_mode == 10)
    {
		cout << "Carregando arquivo com indice parcial " << argv[9] << endl;
		smallIndex = new Index(argv[9], Index::UTI, atoi(argv[7]), argv[2]  );

		cout << "Carregando arquivo com indice parcial 2º camada" << argv[10] << endl;
		SecondSmallIndex = new Index(argv[10], Index::UTI,atoi(argv[7]),argv[2]  );

    }else if(process_mode == 8 || process_mode == 9){

		cout << "Carregando arquivo com indice parcial " << argv[9] << endl;
		smallIndex = new Index(argv[9], Index::UTI, atoi(argv[7]),argv[2] ) ;

		cout << "Carregando arquivo com indice parcial 2º camada" << argv[10] << endl;
		SecondSmallIndex = new Index(argv[10], Index::UTI, atoi(argv[7]),argv[2]  );

		cout << "Carregando arquivo de uti " << argv[8] << endl;
		//Indice que sera usado pra fazer o processamento
		entireIndex = new Index(argv[8], Index::UTI, atoi(argv[7]),argv[2]  );

	}else{
	    cout << "Carregando arquivo de uti " << argv[8] << endl;
		//Indice que sera usado pra fazer o processamento
		entireIndex = new Index(argv[8], Index::UTI, atoi(argv[7]),argv[2]  );
	}
   

	Index *wholedocIndex = NULL;
	Index *textIndex = NULL;
	Index *anchorIndex = NULL;
	Index *titleIndex = NULL;
	Index *urlIndex = NULL;
/*
	if(id_formula >= 1 && id_formula <=10) {
		cout << "Carregando arquivo de wholedoc " << config.read<string>("wholedoc_index") << endl;
		wholedocIndex = new Index(config.read<string>("wholedoc_index").c_str(), Index::UTI, atoi(argv[7]));

		cout << "Carregando arquivo de text " << config.read<string>("text_index") << endl;
		textIndex = new Index(config.read<string>("text_index").c_str(), Index::TEXT, atoi(argv[7]));

		cout << "Carregando arquivo de anchor " << config.read<string>("anchor_index") << endl;
		anchorIndex = new Index(config.read<string>("anchor_index").c_str(), Index::ANCHOR, atoi(argv[7]));

		cout << "Carregando arquivo de title " << config.read<string>("title_index") << endl;
		titleIndex = new Index(config.read<string>("title_index").c_str(), Index::TITLE, atoi(argv[7]));

		cout << "Carregando arquivo de url " << config.read<string>("url_index") << endl;
		urlIndex = new Index(config.read<string>("url_index").c_str(), Index::URL, atoi(argv[7]));
	}
*/
    string vocabulario ( argv[2] );
    Vocabulario *voc = new Vocabulario(10000, 5000, (char*)vocabulario.c_str(), 2);
    
    string dataset ( argv[2] );
    DocsInfo *docsInfo = new DocsInfo(dataset.c_str());

    queryprocessor processor(atof(argv[11]), entireIndex, smallIndex, wholedocIndex, textIndex, titleIndex, anchorIndex, urlIndex, docsInfo, voc, argv[3], process_mode, index_type);

    processor.setSecSIndex(SecondSmallIndex);

    
    processor.setResultHeapSize(atoi(argv[7]) );

    processor.setFormula(id_formula);

    printf("Using formula: %d\n", atoi(argv[6]));
    printf("Heap size: %d\n", processor.getResultHeapSize());

    string interface = "cli";
	int cont_linha=0;
    if (interface.compare("cli") == 0) {
        string query;
        if (argc > 1 ) { //tem consulta
            if(strcmp(argv[1], "-file")) {
				
				ifstream in(argv[1], ifstream::in);
                getline(in, query);
                while(in.good())
                {
                    /**Testar uma consulta**/ 
					//if(cont_linha == 10)
					{
						printf("\nQID: %d ", cont_linha);
						processor.processQuery(query);
					}
                    cont_linha++;
                    getline(in, query);

                }
                in.close();
				
            }
        }
        else{
            char q[1024];
            while(true) {
                cout << "Enter query: " << endl;
                cin.getline(q, 1024);
                processor.processQuery(string(q));
            }
        }
   }
   else{
		cout<<"ThreadPoool removed"<<endl;
   }
   printf("\n\n-----\nTotal Time is: %f ms \nAVG_Time is  : %f ms \n-----\n",totalTime,totalTime/cont_linha);

   delete docsInfo;
   delete voc;
   return 0;
}

