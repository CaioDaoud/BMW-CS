#Format of the documents collection..
	
	2121 # 	 palavra1 palavra2 
	<docName><space><hashtag><tab><word1><space><word2>...
	->See examples in Try/GovTxtTeste/00
	
#Indexer
	Build:
		1. cd Indexer
		2. mkdir build
		3. cd build
		4. cmake ..
		5. make
	 After that you will have the following runnable files:
		./geraParcial
		./mergeParcial
		./geraIndiceFrequencia
		./geraNorma
		./geraSkipListBlockMaxFix
		./GeraDistribuicao (To check the threshold value related to a percentage.)
		./SplitIndex  (To split the index)
	
#QueryProcessor
	Build:
		1. cd processador
		2. mkdir build
		3. cd build
		4. cmake ..
		5. make
	After that you will have the following runnable files:
		./queryProcessor
	
### Try the Query Processor ###

1. Build the Indexer and the QueryProcessor.
2. Copy all runnable files to ./Try/
 
3. To index the documents and to split the index.

	a. $cd Try/
	b. python geraIndice.py <pathColecao> <pathBase> <pathIndice> <Nome do Indice>
	  $:python geraIndice.py GovTxtTeste/ govTxtBase/ govInd/ govTxt.
	  //whith this the documents are indexed in one tier, govInd/govTxt.
	  
	c. python Split2.py <pathIndice> <Nome do Indice.> <% (1 - 99)>
	  $:python Split2.py govInd/ govTxt. 25
	  //whith this  the index is splited in two tiers (govInd/fir_25. govInd/sec_25.) , with 25% in the first tier.
	  
4. To processe queries.
./QueryProcessor <arq.query> <path.word> <file.result> <wand/bmw/cs> <VET/UTI/BM25> <0> <Top-k> <Entire.index> <First.index> <Second.index> <0>
a. [Wand]  $:./QueryProcessor 1000query GovIndice/govTxt. rankingWand wand BM25 0 10  GovIndice/govTxt. GovIndice/fir_25. 
GovIndice/sec_25. 0
b. [BMW]   $:./QueryProcessor 1000query GovIndice/govTxt. rankingBMW  BMW  BM25 0 10  GovIndice/govTxt. GovIndice/fir_25. GovIndice/sec_25. 0
c. [BMW-CS]$:./QueryProcessor 1000query GovIndice/govTxt. rankingCSP  CS   BM25 0 10  GovIndice/govTxt. GovIndice/fir_25. GovIndice/sec_25. 0
