#Document Base.
	exemplo linha do arquivo
	2121 # 	 palavra1 palavra2 
	<docName><space><hashtag><tab><word1><space><word2>...

	see example in Try/GovTxtTeste/00
	
#Indexer
	./geraParcial
	./mergeParcial
	./geraIndiceFrequencia
	./geraNorma
	./geraSkipListBlockMaxFix
	
	Build:
		1. cd Indexer
		2. mkdir build
		3. cd build
		4. cmake ..
		5. make
	now you have all executables for indexing de Document Base.

#SplitIndex
	./GeraDistribuicao (to check the limiar for a %)
	./SplitIndex

#QueryProcessor
	Build:
		1. cd processador
		2. mkdir build
		3. cd build
		4. cmake ..
		5. make
	now you have the executable ./queryProcessor.
	
### Exemple to Run ###


1. Build the Indexer and the QueryProcessor.
2. Copy all executables to ./Try/
 
#To index the documents and to split the index.
3. Do it:
	1.$cd Try/
	2.python geraIndice.py <pathColecao> <pathBase> <pathIndice> <Nome do Indice>
      $:python geraIndice.py GovTxtTeste/ govTxtBase/ govInd/ govTxt.
	  //whith this the documents are indexed in one tier, govInd/govTxt.
	  
	3.python Split2.py <pathIndice> <Nome do Indice.> <% (1 - 99)>
	  $:python Split2.py govInd/ govTxt. 25
	  //whith this  the index is splited in two tiers (govInd/fir_25. govInd/sec_25.) , with 25% in the first tier.
	  
#To processe queries.
             ./QueryProcessor <arq.query> <path.word> <file.result> <wand/bmw/cs> <VET/UTI/BM25> <0> <Top-k> <Entire.index> <First.index> <Second.index> <0>
4. [Wand]  $:./QueryProcessor 1000query GovIndice/govTxt. rankingWand wand BM25 0 10  GovIndice/govTxt. GovIndice/fir_25. GovIndice/sec_25. 0
4. [BMW]   $:./QueryProcessor 1000query GovIndice/govTxt. rankingBMW  BMW  BM25 0 10  GovIndice/govTxt. GovIndice/fir_25. GovIndice/sec_25. 0
4. [BMW-CS]$:./QueryProcessor 1000query GovIndice/govTxt. rankingCSP  CS   BM25 0 10  GovIndice/govTxt. GovIndice/fir_25. GovIndice/sec_25. 0
