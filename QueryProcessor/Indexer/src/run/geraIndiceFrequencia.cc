#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../../../libs-iw/indexer/include/IndiceInvertido.h"

class LeitorLista
{
	public: 
	FILE *fp;
	_tipo_buffer *buffer;
	unsigned char btat;
	_tipo_buffer *out;
	long tam_lista;
	char fileatual[1000];

	LeitorLista(){	buffer = new _tipo_buffer[SIZE_BUFFER_LEITOR_LISTA+SIZE_FOLGA_LEITOR_LISTA];	fp = NULL;	fileatual[0] = 0;	}

	void Inicializa(long _tam_lista, char *arq, long inicio)
	{
        if(strcmp(fileatual, arq))
        {
                if(fp)  fclose(fp);
                strcpy(fileatual, arq);
                fp = fopen( arq, "rb");
        }

		fseek( fp, inicio, SEEK_SET);

		if(_tam_lista*2 > (SIZE_BUFFER_LEITOR_LISTA+SIZE_FOLGA_LEITOR_LISTA))
			fread(buffer, sizeof(_tipo_buffer), SIZE_BUFFER_LEITOR_LISTA+SIZE_FOLGA_LEITOR_LISTA, fp);
		else
			fread(buffer, sizeof(_tipo_buffer), _tam_lista*2, fp);

		out = buffer;
		btat = 0;
		tam_lista = _tam_lista;
	}

	inline	int getDoc(unsigned int *doc, unsigned int *info)
	{
		if(tam_lista == 0)	return 0;

		tam_lista --;

		/*Final do buffer. Deve-se ler mais*/
		if(out >= &buffer[SIZE_BUFFER_LEITOR_LISTA])
		{
			memcpy(buffer, &buffer[SIZE_BUFFER_LEITOR_LISTA], SIZE_FOLGA_LEITOR_LISTA*sizeof(int));
			fread(buffer+SIZE_FOLGA_LEITOR_LISTA, sizeof(_tipo_buffer), SIZE_BUFFER_LEITOR_LISTA, fp);
			out -= SIZE_BUFFER_LEITOR_LISTA;
		}

		*doc = RawDeltaDecode(out, btat) - 1;

		/*Final do buffer. Deve-se ler mais*/
		if(out >= &buffer[SIZE_BUFFER_LEITOR_LISTA])
		{
			memcpy(buffer, &buffer[SIZE_BUFFER_LEITOR_LISTA], SIZE_FOLGA_LEITOR_LISTA*sizeof(int));
			fread(buffer+SIZE_FOLGA_LEITOR_LISTA, sizeof(_tipo_buffer), SIZE_BUFFER_LEITOR_LISTA, fp);
			out -= SIZE_BUFFER_LEITOR_LISTA;
		}

		*info = RawDeltaDecode(out, btat) - 1;

		return 1;
	}
};

int main(int argc, char **argv){
	
	if(argc != 2)
	{
		printf("Execute:\n\t%s <path_index>\n", argv[0]);
		exit(1);
	}

	FILE *ffreq = NULL;
	FILE *fidx = NULL;
	IDX idx;
	int id_arq_ant = -1;
	int t = 0;
	_tipo_buffer *buffer_lista_freq = new _tipo_buffer[SIZE_BUFFER_LEITOR_LISTA+SIZE_FOLGA_LEITOR_LISTA];
	_tipo_buffer *out_freq;
	_tipo_buffer doc, pos;
	unsigned char btat_freq;

	LeitorLista leitor;

	char arq[100];
	int total;
	_tipo_buffer docid;
	int freq;
	unsigned int num_doc = 0;
	time_t start = time(NULL);
    int currDoc;
	char filename[1000];
	
	sprintf(filename, "%s%s", argv[1], NOME_IDX);
	fidx = fopen( filename, "r+" );
	printf("tam IDX %d\n",sizeof(IDX));
	
	while(fread( &idx, sizeof(IDX), 1, fidx ))
	{
		t++;
		if(idx.pos_tam_lista == 0)	
		{
			continue;
		}
		
		//cout<<"tamLista != 0"<<endl;
		/*Abre o arquivo de listas posicionais*/
		sprintf( arq, "%s%s%d", argv[1], NOME_POSICIONAL, idx.id_arq );
		leitor.Inicializa(idx.pos_tam_lista, arq, idx.pos_inicio_termo);
		
		if(id_arq_ant != idx.id_arq)
		{
		
			if(ffreq)	fclose(ffreq);
			/*Cria o arquivo de lista de frequencia*/
			sprintf( arq, "%s%s%d", argv[1], NOME_FREQUENCIA, idx.id_arq );
			cout<<"criando arquivo : "<<arq<<endl;
			ffreq = fopen( arq, "w+");
			id_arq_ant = idx.id_arq;
		}

		idx.freq_inicio_termo = ftell(ffreq);
		idx.freq_tam_lista = 0;

		total = 0;
		unsigned acm = 0;
		buffer_lista_freq[0] = 0;
		out_freq = buffer_lista_freq;
		btat_freq = 0;
		num_doc = 0;

		/*Le o primeiro documento*/
		leitor.getDoc(&docid, &pos);
		// Modificado abaixo para todo docid comecar com pelo menos 1
		// No indice de frequencias
		docid ++;
		currDoc = docid;
		freq = 1;
		
		while(leitor.getDoc(&doc, &pos))
		{	
			if(doc != 0)
			{
				num_doc ++;
				acm+=docid;
				RawDeltaEncode(docid, out_freq, btat_freq);
				RawDeltaEncode(freq, out_freq, btat_freq);
				
				if(out_freq >= &buffer_lista_freq[SIZE_BUFFER_LEITOR_LISTA])
				{
					fwrite(buffer_lista_freq, sizeof(_tipo_buffer), out_freq-buffer_lista_freq, ffreq);
					buffer_lista_freq[0] = *out_freq;
					out_freq = buffer_lista_freq;
				}
				freq = 1;
				docid = doc;
				currDoc += docid;
			}else
			{
				freq++;
			}
		}

		if(freq>0)
		{
			num_doc ++;
			acm+=docid;
			if(t<=2)	printf("%d %d %d\n",t-1,acm,freq);
			RawDeltaEncode(docid, out_freq, btat_freq);
			RawDeltaEncode(freq, out_freq, btat_freq);
		}
		if(out_freq != buffer_lista_freq)
			fwrite(buffer_lista_freq, sizeof(_tipo_buffer), out_freq-buffer_lista_freq, ffreq);
		if(btat_freq)
			fwrite(out_freq, sizeof(_tipo_buffer), 1, ffreq);

		idx.freq_tam_lista = num_doc;

		fseek(fidx, -sizeof(IDX), SEEK_CUR);
		fwrite(&idx, sizeof(IDX), 1, fidx);

	}

	printf("Indice de frequencia gerado em %.2lfmin\n", difftime(time(NULL), start)/60);

	return 0;
}
