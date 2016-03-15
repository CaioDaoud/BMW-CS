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
#
	[queryProcessor,
	geraParcial,
	mergeParcial,
	geraIndiceFrequencia,
	geraNorma,
	geraSkipListBlockMaxFix,
	GeraDistribuicao,
	SplitIndex]
	
 
3. To index the documents and to split the index.

	I.  $: cd Try/
	
	II. Indexing documents in one tier, govInd/govTxt.
	
	    [python GeraIndice.py <pathColecao> <pathBase> <pathIndice> <Nome do Indice>]
	    
	    $: python GeraIndice.py GovTxtTeste/ GovTxtBase/ GovInd/ GovTxt.
	 
	  
	III. Splitting the index in two tiers (govInd/fir_25. govInd/sec_25.) , with 25% in the first tier.. 
	     
	     [python Split.py <pathIndice> <Nome do Indice.> <% (1 - 99)>]
	 
	     $: python Split.py GovInd/ GovTxt. 25
	 
	   
	  
4. To process queries. (To view the generated ranking open the <file.result> )

	./QueryProcessor<arq.query><path.word><file.result><wand/bmw/cs><VET/UTI/BM25><x><Top-k><Entire.index><First.index><Second.index><x>

	I. [Wand] $: ./QueryProcessor 1000query GovInd/GovTxt. rankingWand wand BM25 0 10  GovInd/GovTxt. GovInd/fir_25. GovInd/sec_25. 0

	II. [BMW] $:    ./QueryProcessor 1000query GovInd/GovTxt. rankingBMW  BMW  BM25 0 10  GovInd/GovTxt. GovInd/fir_25. GovInd/sec_25. 0

	III. [BMW-CS] $: ./QueryProcessor 1000query GovInd/GovTxt. rankingCSP  CS   BM25 0 10  GovInd/GovTxt. GovInd/fir_25. GovInd/sec_25. 0
