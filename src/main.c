#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "../include/libfacom.h"
#include <math.h>

typedef struct{
    char codigo_ibge[10];
    char nome[40];
    float latitude;
    float longitude;
    int capital;
    int codigo_uf;
    int siafi_id;
    int ddd;
    char fuso_horario[40];
}tmunicipio;

char *get_key_ibge(void *reg) {
    return ((tmunicipio *)reg)->codigo_ibge;
}
char *get_key_nome(void *reg) {
    return ((tmunicipio *)reg)->nome;
}

float cmpy(void *t1, void *t2){
    return ((tmunicipio *) t1)->latitude - ((tmunicipio *) t2)->latitude;
}
float cmpx(void *t1, void *t2){
    return ((tmunicipio *) t1)->longitude - ((tmunicipio *) t2)->longitude;
}
float distancia(void *t1, void *t2){
    float dlat = cmpy(t1,t2);
    float dlong = cmpx(t1,t2);
    return dlat*dlat + dlong*dlong; // retorna dist^2
}


void *aloca_municipio(char *codigo_ibge, char *nome, float latitude, float longitude, int capital, int codigo_uf, int siafi_id, int ddd, char *fuso_horario);
void carregaDados(thash *h_ibge,tarv *arv, thash *h_nome, FILE *arq, int *max1, int *tot1, int *c1, int *max2, int *tot2, int *c2, int *totabb);
void imprime_municipio(tmunicipio *municipio);
void buscaVizinhos(tarv *arv, thash *h, theap *heap, tmunicipio *reg, int n);
void imprimeVetor(theap heap[], int tam);

int main(){
    thash h_ibge, h_nome;
    tarv arv;
    theap *heap = NULL;
    tmunicipio *buscando;

    int nbuckets = 10007;
    int max1 = 0, tot1 = 0, sucess1 = 0, max2 = 0, tot2 = 0, sucess2 = 0, totabb=0;
    int escolha=-1, n, escolha2;
    char leitura[10]; 
    char caracter;

    FILE *arquivo = fopen("./data/municipios.json", "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");        
        return EXIT_FAILURE;
    }    

    hash_constroi(&h_ibge, nbuckets, get_key_ibge);
    printf("Hash_ibge construido.\n");
    hash_constroi(&h_nome, nbuckets, get_key_nome);
    printf("Hash_nome construido.\n");
    abb_constroi(&arv, cmpx, cmpy, distancia, get_key_ibge);
    printf("Arvore construida.\n");
    carregaDados(&h_ibge, &arv, &h_nome, arquivo, &max1, &tot1, &sucess1, &max2, &tot2, &sucess2,&totabb);
    printf("Carregamento de dados concluido.\n\n");  
        
    printf("Houve %d insercoes ao utilizar codigo_ibge.\n", sucess1);
    printf("Houve no maximo %d colisoes em uma insercao.\nHouve %d totais colisoes.\n\n", max1, tot1);
    printf("Houve %d insercoes ao utilizar nome.\n", sucess2);
    printf("Houve no maximo %d colisoes em uma insercao.\nHouve %d totais colisoes.\n\n", max2, tot2);
    printf("Houve %d insercoes na kd-tree.\n", totabb);

    while(escolha != 0){
        printf("\nMENU:\n0. SAIDA\n1. BUSCA_IBGE (TAREFA 1)\n2. BUSCA VIZINHOS(TAREFA 2)\n3. DADOS VIZINHOS(TAREFA 3)\nDigite sua escolha: ");
        if(scanf(" %d", &escolha) != 1 ){
            printf("Escolha invalida, digite novamente.\n");
            while (getchar() != '\n'); //limpar buffer
            continue;
        }
        if(escolha == 1){
            printf("Digite o Codigo IBGE a ser buscado: ");
            scanf(" %s", leitura);
            buscando = hash_busca(&h_ibge, leitura);
            if(buscando == NULL) printf("Codigo nao encontrado na hash.\n");            
            else imprime_municipio(buscando);
        }else if(escolha == 2){
            if(heap == NULL){
                printf("Digite o codigo IBGE da cidade para procurar seus vizinhos: ");
                scanf(" %[^\n]", leitura);
                printf("Digite o numero n de vizinhos a serem buscados: ");            
                buscando = hash_busca(&h_ibge, leitura);
                if(scanf("%d", &n) != 1 || buscando == NULL) {
                    printf("Entrada invalida! Retornando ao MENU.\n");
                    continue;
                }
                heap = malloc((n+1)*sizeof(theap)); // n vizinhos + cidade buscada
                buscaVizinhos(&arv, &h_ibge, heap, buscando, n);
                imprimeVetor(heap, n+1);
            }else{
                printf("Memoria ocupada, deseja vizualizar(digite 1) ou limpar a memoria(digite 0)? ");
                scanf(" %d", &escolha2);
                if(escolha2==0){
                    free(heap);
                    heap = NULL;
                }else if(escolha2==1){
                    imprimeVetor(heap,n+1);
                }
            }
        }else if(escolha==3){
            if(heap == NULL){
                printf("Nao ha vizinhos salvos na memoria. Retornando ao MENU\n");
            }else{
                for(int k=1;k<n+1;k++){
                    imprime_municipio(hash_busca(&h_ibge, heap[k].key));
                    printf("| distancia    | %f\n\n", heap[k].distancia);
                }

            }
        }else if(escolha != 0){
            printf("Escolha invalida, digite novamente.\n");
        }
    }

    fclose(arquivo);
    abb_apaga(arv.raiz);
    printf("Arvore apagada.\n");
    hash_apaga(&h_ibge);
    printf("Hash_ibge apagado.\n");
    free(h_nome.table); 
    printf("Hash_nome apagado.\n");
    if(heap!=NULL) free(heap);
    return EXIT_SUCCESS;
}


void carregaDados(thash *h_ibge,tarv *arv, thash *h_nome, FILE *arq, int *max1,
                int *tot1, int *c1, int *max2, int *tot2, int *c2, int *totabb){
    char linha[60];
    char *start;
    char *end;
    int colisoes;
    tmunicipio temp;
    tmunicipio *temp2;

    while(1){
        if(fgets (linha, 60, arq)!=NULL) {
            start = strchr(linha,'\"');
            if(start == NULL) {
                continue;
            } else{                
                start = strchr(linha,':') + 2;
                end = strrchr(start,',');
                *end = '\n';
                sscanf(start," %s", temp.codigo_ibge);

                fgets (linha, 60, arq);            
                start = strchr(linha,':') + 3;
                end = strrchr(start,'\"');
                *end = '\n';
                sscanf(start," %[^\n]", temp.nome);

                fgets (linha, 60, arq);            
                start = strchr(linha,':') + 1;
                end = strrchr(start,',');
                *end = '\n';
                sscanf(start,"%f", &temp.latitude);    

                fgets (linha, 60, arq);            
                start = strchr(linha,':') + 1;
                end = strrchr(start,',');
                *end = '\n';
                sscanf(start,"%f", &temp.longitude);

                fgets (linha, 60, arq);            
                start = strchr(linha,':') + 1;
                end = strrchr(start,',');
                *end = '\n';
                sscanf(start,"%d", &temp.capital);

                fgets (linha, 60, arq);
                start = strchr(linha,':') + 1;
                end = strrchr(start,',');
                *end = '\n';
                sscanf(start,"%d", &temp.codigo_uf);

                fgets (linha, 60, arq);            
                start = strchr(linha,':') + 1;
                end = strrchr(start,',');
                *end = '\n';
                sscanf(start,"%d", &temp.siafi_id);

                fgets (linha, 60, arq);            
                start = strchr(linha,':') + 1;
                end = strrchr(start,',');
                *end = '\n';
                sscanf(start,"%d", &temp.ddd);

                fgets (linha, 60, arq);            
                start = strchr(linha,':') + 3;
                end = strrchr(start,'\"');
                *end = '\n';
                sscanf(start," %[^\n]", temp.fuso_horario);
                
                temp2 = aloca_municipio(temp.codigo_ibge, temp.nome, temp.latitude, temp.longitude, 
                    temp.capital, temp.codigo_uf, temp.siafi_id, temp.ddd, temp.fuso_horario);

                /*hash_ibge*/
                colisoes = 0;
                if(hash_insere(h_ibge, temp2, &colisoes) == EXIT_SUCCESS) *c1 +=1;
                *tot1 += colisoes;
                if(colisoes>*max1) *max1 = colisoes;
                /*kd-tree*/                
                colisoes = 0;
                abb_insere(arv, temp2, 0, &colisoes);
                *totabb+=colisoes;

                /*hash_nome*/ 
                colisoes = 0;
                if(hash_insere(h_nome, temp2, &colisoes) == EXIT_SUCCESS) *c2 +=1;
                *tot2 += colisoes;
                if(colisoes>*max2) *max2 = colisoes;

            }
        } else{
            break;
        }
    }
}

void *aloca_municipio(char *codigo_ibge, char *nome, float latitude, float longitude, int capital,
                      int codigo_uf, int siafi_id, int ddd, char *fuso_horario) {
    tmunicipio *municipio = malloc(sizeof(tmunicipio));
    strcpy(municipio->codigo_ibge, codigo_ibge);
    strcpy(municipio->nome, nome);
    municipio->latitude = latitude;
    municipio->longitude = longitude;
    municipio->capital = capital;
    municipio->codigo_uf = codigo_uf;
    municipio->siafi_id = siafi_id;
    municipio->ddd = ddd;
    strcpy(municipio->fuso_horario, fuso_horario);
    return municipio;
}

void imprime_municipio(tmunicipio *municipio){    
    printf("| codigo_ibge  | %s\n", municipio->codigo_ibge);
    printf("| nome         | %s\n", municipio->nome);
    printf("| latitude     | %.4f\n", municipio->latitude);
    printf("| longitude    | %.4f\n", municipio->longitude);
    printf("| capital      | %d\n", municipio->capital);
    printf("| codigo_uf    | %d\n", municipio->codigo_uf);
    printf("| siafi_id     | %d\n", municipio->siafi_id);
    printf("| ddd          | %d\n", municipio->ddd);
    printf("| fuso_horario | %s\n", municipio->fuso_horario);
    
}

void buscaVizinhos(tarv *arv, thash *h, theap *heap, tmunicipio *reg, int n){    
    for(int i=0;i<n+1 ;i++){
        heap[i].distancia = 3.40282347E+38;
        *(heap[i].key) = '\0';
    }
    nearNeighbor(arv,heap,reg->codigo_ibge,0,n+1);

    heap_sort(heap,n+1);      
}

void imprimeVetor(theap heap[], int tam){
    printf("Cidade Selecionada: %s.\n", heap[0].key);
    printf("|  | Distancia  | cod_ibge\n");
    for(int i = 1; i<tam; i++){
        printf("|%2.d| %f   | %s\n",i, heap[i].distancia, heap[i].key);
    }

}