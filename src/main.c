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

/*
int cmp(void *t1, void *t2){
    return ((taluno *) t1)->rga - ((taluno *) t2)->rga ;
}
*/

void *aloca_municipio(char *codigo_ibge, char *nome, float latitude, float longitude, int capital, int codigo_uf, int siafi_id, int ddd, char *fuso_horario);
void carregaDados(thash *h_ibge,tarv *arv, thash *h_nome, FILE *arq, int *max1, int *tot1, int *c1, int *max2, int *tot2, int *c2);
void imprime_municipio(tmunicipio *municipio);

int main(){
    thash h_ibge, h_nome;
    tarv arv;

    int nbuckets = 10000;
    int max1 = 0, tot1 = 0, sucess1 = 0, max2 = 0, tot2 = 0, sucess2 = 0;
    int escolha=-1;
    char leitura[10]; 

    FILE *arquivo = fopen("./data/municipios.json", "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");        
        return EXIT_FAILURE;
    }    

    hash_constroi(&h_ibge, nbuckets, get_key_ibge);
    carregaDados(&h_ibge, &arv, &h_nome, arquivo, &max1, &tot1, &sucess1, &max2, &tot2, &sucess2);  
        
    printf("Houve %d insercoes.\n", sucess1);
    printf("Houve no maximo %d colisoes em uma insercao.\nHouve %d totais colisoes.\n", max1, tot1);

    while(1){
        printf("\nMenu:\n0. SAIDA\n1. BUSCA\nDigite sua escolha: ");
        scanf("%d", &escolha);

        if(escolha == 0){
            break;
        }else if(escolha == 1){
            printf("Digite o Codigo IBGE a ser buscado: ");
            scanf(" %s", leitura);
            imprime_municipio((hash_busca(&h_ibge, leitura)));
        }else {
            printf("Escolha invalida, digite novamente.\n");
        }
    }

    fclose(arquivo);
    hash_apaga(&h_ibge);
    return EXIT_SUCCESS;
}


void carregaDados(thash *h_ibge,tarv *arv, thash *h_nome, FILE *arq, int *max1,
                int *tot1, int *c1, int *max2, int *tot2, int *c2){
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

                /*hash_nome*/ 
                colisoes = 0;

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