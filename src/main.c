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
float distancia2(void *t1, void *t2){
    float dlat = cmpy(t1,t2);
    float dlong = cmpx(t1,t2);
    return dlat*dlat + dlong*dlong; // retorna dist^2
}

void *aloca_municipio(char *codigo_ibge, char *nome, float latitude, float longitude, int capital, int codigo_uf, int siafi_id, int ddd, char *fuso_horario);
void carregaDados(thash *h_ibge,tarv *arv, thash *h_nome, FILE *arq);
void imprime_municipio(tmunicipio *municipio);
void buscaVizinhos(tarv *arv, thash *h, theap *heap, tmunicipio *reg, int n);
void imprimeVetor(theap heap[], int n);

int main(){
    thash h_ibge, h_nome;
    tarv arv;
    theap *heap = NULL;
    tmunicipio *buscando;
    tmunicipio *vetorMunicipio;

    int nbuckets = 15013;
    int escolha=-1, n, i, escolha2, qtd;
    char leitura[40]; 
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
    abb_constroi(&arv, cmpx, cmpy, distancia2, get_key_ibge);
    printf("Arvore construida.\n");
    carregaDados(&h_ibge, &arv, &h_nome, arquivo);    

    while(1){ // MENU
        printf("\nMENU:\n0 - SAIDA\n1 - BUSCA_IBGE (TAREFA 1)\n2 - BUSCA VIZINHOS (TAREFA 2)\n3 - DADOS VIZINHOS (TAREFA 3)\n\nDigite sua escolha: ");
        if(scanf("%d", &escolha) != 1 ){ //1 eh o valor esperado ao ler 1 numero
            printf("Escolha invalida, digite novamente.\n");
            while (getchar() != '\n'); //limpar buffer
            continue;
        }
        if(escolha == 1){ /* Busca e mostra os dados de uma cidade */
            printf("Digite o Codigo IBGE a ser buscado: ");
            scanf(" %s", leitura);
            buscando = hash_busca(&h_ibge, leitura);
            if(buscando == NULL) printf("Codigo nao encontrado na hash.\n");            
            else imprime_municipio(buscando);
        }else if(escolha == 2){ /* digita o cod_ibge e n, retornando um vetor com os n vizinhos mais proximos */            
            printf("Digite o codigo IBGE da cidade para procurar seus vizinhos: ");
            scanf(" %[^\n]", leitura);                
            buscando = hash_busca(&h_ibge, leitura);                    
            while(1){
                if(buscando == NULL){
                    printf("Codigo nao encontrado na hash.\n");
                    break;                
                }                    
                printf("Digite o numero n de vizinhos a serem buscados(0 para sair): ");            
                if(scanf("%d", &n) != 1 || n<0) {
                    printf("Entrada invalida! Digite novamente.\n");
                    while (getchar() != '\n');
                    continue;
                }
                if(n==0) break;
                heap = malloc((n+1)*sizeof(theap)); // n vizinhos + cidade buscada
                buscaVizinhos(&arv, &h_ibge, heap, buscando, n);
                imprimeVetor(heap, n);
                free(heap);  
            }               
        }else if(escolha==3){ /* se a lista heap estiver preenchida, busca os dados destas cidades */
            printf("Digite o nome a ser buscado: ");
            scanf(" %[^\n]", leitura);
            qtd = qtd_ocorrencias_hash(&h_nome, leitura);
            if(qtd<1){
                printf("Nome nao encontrado na hash. Retornando ao MENU.\n");
            }else if(qtd==1){ // nao ha nomes repetidos
                buscando = hash_busca(&h_nome, leitura);
                buscando = hash_busca(&h_ibge, buscando->codigo_ibge);
                while(1){
                    printf("Digite o numero n de vizinhos a serem buscados(0 para sair): ");                
                    if(scanf("%d", &n) != 1) {
                        printf("Entrada invalida! Digite novamente.\n");
                        while (getchar() != '\n');   
                        continue;                 
                    }
                    if(n==0) break;
                    heap = malloc((n+1)*sizeof(theap)); // n vizinhos + cidade buscada
                    buscaVizinhos(&arv, &h_ibge, heap, buscando, n);
                    for(int k=1;k<n+1;k++){// k==0 é a propria cidade  
                        imprime_municipio(hash_busca(&h_ibge, heap[k].key));
                        printf("| distancia    | %f\n\n", sqrtf(heap[k].distancia2));
                    }
                    free(heap);
                }
            }else if(qtd>1){
                vetorMunicipio = malloc(qtd*sizeof(tmunicipio));
                hash_busca_ate_0(&h_nome, vetorMunicipio, leitura, qtd, sizeof(tmunicipio));
                printf("Existe repeticao de %s %d vezes.\n", vetorMunicipio[0].nome, qtd);
                while(1){                        
                    printf("| opcao | cod_ibge | ddd | fuso_horario\n");
                    for(i=0;i<qtd;i++){                    
                        printf("|   %d   |  %s |  %d | %s\n", i+1, vetorMunicipio[i].codigo_ibge, vetorMunicipio[i].ddd, vetorMunicipio[i].fuso_horario);
                    }
                    printf("Escolha(0 para sair): ");
                    while(1){                        
                        if(scanf(" %d", &escolha2) != 1 || escolha2>qtd || escolha2<0) {
                            printf("Entrada invalida! Digite novamente: ");   
                            while (getchar() != '\n');     
                            continue;            
                        }
                        if(escolha2 == 0) break;
                        printf("Digite o numero n de vizinhos a serem buscados: ");                        
                        if(scanf(" %d", &n) != 1|| n<0) {
                            printf("Entrada invalida! ");  
                            while (getchar() != '\n');
                            break;
                        }
                        
                        heap = malloc((n+1)*sizeof(theap)); // n vizinhos + cidade buscada
                        buscaVizinhos(&arv, &h_ibge, heap, &vetorMunicipio[escolha2-1], n);
                        for(int k=1;k<n+1;k++){// k==0 é a propria cidade  
                            imprime_municipio(hash_busca(&h_ibge, heap[k].key));
                            printf("| distancia    | %f\n\n", sqrtf(heap[k].distancia2));
                        }
                        free(heap);                        
                        break;
                    }
                    if(escolha2==0) break;
                }
                free(vetorMunicipio);
            }                        
        }else if(escolha==0){
            break;
        }else {
            printf("Escolha invalida, digite novamente.\n");
        }
    }

    fclose(arquivo);
    abb_apaga(arv.raiz);
    printf("Arvore apagada.\n");
    hash_apaga(&h_ibge);
    printf("Hash_ibge apagado.\n");
    free(h_nome.table); // utilizar hash_apaga causava problema de double free()
    printf("Hash_nome apagado.\n");
    return EXIT_SUCCESS;
}


void carregaDados(thash *h_ibge,tarv *arv, thash *h_nome, FILE *arq){
    char linha[60];
    char *start;
    char *end;
    int colisoes;
    int max1=0, tot1=0, max2=0, tot2=0; //contadores colisoes
    int totabb=0, c1=0 , c2=0; //insercoes realizadas
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
                if(hash_insere(h_ibge, temp2, &colisoes) == EXIT_SUCCESS) c1 += 1;
                tot1 += colisoes;
                if(colisoes>max1) max1 = colisoes;

                /*kd-tree*/
                if(abb_insere(arv, temp2, 0) == EXIT_SUCCESS) totabb += 1;

                /*hash_nome*/ 
                colisoes = 0;
                if(hash_insere(h_nome, temp2, &colisoes) == EXIT_SUCCESS) c2 += 1;
                tot2 += colisoes;
                if(colisoes>max2) max2 = colisoes;
            }
        } else{
            break;
        }
    }
    printf("Carregamento de dados concluido.\n\n");  
    printf("Houve %d insercoes ao utilizar codigo_ibge.\n", c1);
    printf("Houve no maximo %d colisoes em uma insercao.\nHouve %d totais colisoes.\n\n", max1, tot1);
    printf("Houve %d insercoes ao utilizar nome.\n", c2);
    printf("Houve no maximo %d colisoes em uma insercao.\nHouve %d totais colisoes.\n\n", max2, tot2);
    printf("Houve %d insercoes na kd-tree.\n", totabb);
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

/* busca os n vizinhos e guarda o resultado em um vetor de n+1 posicoes, em ordem crescente de distancia,
   a propria cidade deverá estar em heap[0] apos o heap_sort */
void buscaVizinhos(tarv *arv, thash *h, theap *heap, tmunicipio *reg, int n){    
    for(int i=0;i<n+1 ;i++){
        heap[i].distancia2 = 3.40282347E+38;
        *(heap[i].key) = '\0';
    }
    nearNeighbor(arv,heap,reg->codigo_ibge,0,n+1);
    heap_sort(heap,n+1);      
}

/* imprime as as distancias e cod_ibge dos n vizinhos mais proximos, nota-se que utilizou-se
   uma heap com n+1 posicoes */
void imprimeVetor(theap heap[], int n){
    printf("Cidade Selecionada: %s.\n", heap[0].key);
    printf("|   | Distancia  |  cod_ibge\n");
    for(int i = 1; i<n+1; i++){ //i==0 esta a propria cidade
        printf("|%2.d |  %f  | %s\n",i, sqrtf(heap[i].distancia2), heap[i].key);
    }
}