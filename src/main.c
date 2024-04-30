#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "../include/libfacom.h"

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


void *aloca_municipio(char *codigo_ibge, char *nome, float latitude, float longitude, int capital, int codigo_uf, int siafi_id, int ddd, char *fuso_horario);
void carregaDados(thash *h_ibge,tarv *arv, thash *h_nome, FILE *arq, int *max1, int *tot1, int *c1, int *max2, int *tot2, int *c2, int *totabb);
void imprime_municipio(tmunicipio *municipio);

int main(){
    thash h_ibge, h_nome;
    tarv arv;

    int nbuckets = 10000;
    int max1 = 0, tot1 = 0, sucess1 = 0, max2 = 0, tot2 = 0, sucess2 = 0, totabb=0;
    int escolha=-1;
    char leitura[10]; 

    FILE *arquivo = fopen("./data/municipios.json", "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");        
        return EXIT_FAILURE;
    }    

    hash_constroi(&h_ibge, nbuckets, get_key_ibge);
    printf("Hash_ibge construido.\n");
    hash_constroi(&h_nome, nbuckets, get_key_nome);
    printf("Hash_nome construido.\n");
    abb_constroi(&arv, cmpx, cmpy);
    printf("Arvore construida.\n");
    carregaDados(&h_ibge, &arv, &h_nome, arquivo, &max1, &tot1, &sucess1, &max2, &tot2, &sucess2,&totabb);
    printf("Carregamento de dados concluido.\n\n");  
        
    printf("Houve %d insercoes ao utilizar codigo_ibge.\n", sucess1);
    printf("Houve no maximo %d colisoes em uma insercao.\nHouve %d totais colisoes.\n\n", max1, tot1);
    printf("Houve %d insercoes ao utilizar nome.\n", sucess2);
    printf("Houve no maximo %d colisoes em uma insercao.\nHouve %d totais colisoes.\n\n", max2, tot2);
    printf("Houve %d insercoes na kd-tree.\n", totabb);

    while(escolha != 0){
        printf("\nMenu:\n0. SAIDA\n1. BUSCA_IBGE\n2. TESTE\n3. BUSCA_NOME\nDigite sua escolha: ");
        if(scanf(" %d", &escolha) != 1 ){
            printf("Escolha invalida, digite novamente.\n");
            while (getchar() != '\n'); //limpar buffer
            continue;
        }
        if(escolha == 1){
            printf("Digite o Codigo IBGE a ser buscado: ");
            scanf(" %s", leitura);
            imprime_municipio((hash_busca(&h_ibge, leitura)));
        }else if(escolha==3){
            printf("Digite um nome a ser buscado: ");
            scanf(" %[^\n]", leitura);
            imprime_municipio((hash_busca(&h_nome, leitura)));
        }else if(escolha == 2){
            imprime_municipio((arv.raiz)->reg);
            imprime_municipio(((arv.raiz)->esq)->reg);
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
    if(municipio == NULL){
        printf("Municipio nao encontrado na hash.\n");
    } else{
        printf("\nInformacoes relacionados ao codigo fornecido:\n");
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
}