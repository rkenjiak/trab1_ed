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

char *get_key(void *reg) {
    return ((tmunicipio *)reg)->codigo_ibge;
}

void *aloca_municipio(char *codigo_ibge, char *nome, float latitude, float longitude, int capital, int codigo_uf, int siafi_id, int ddd, char *fuso_horario);
void carregaHash(thash *h, FILE *arq, int *maxcolisoes, int *totcolisoes, int *cont);
void busca_municipio(thash *h);

int main(){
    thash h;
    int nbuckets = 10000;
    int maxColisoes = 0;
    int totColisoes = 0;
    int sucess=0;
    int escolha=-1;
    
    hash_constroi(&h, nbuckets, get_key);

    FILE *arquivo = fopen("./data/municipios.json", "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return EXIT_FAILURE;
    }    

    carregaHash(&h, arquivo, &maxColisoes, &totColisoes, &sucess);      
    printf("Houve %d insercoes.\n", sucess);
    printf("Houve no maximo %d colisoes em uma insercao.\nHouve %d totais colisoes.\n", maxColisoes, totColisoes);

    while(1){
        printf("\nMenu:\n0. SAIDA\n1. BUSCA\nDigite sua escolha: ");
        scanf("%d", &escolha);

        if(escolha == 0){
            break;
        }else if(escolha == 1){
            busca_municipio(&h);
        }else{
            printf("Escolha invalida, digite novamente.\n");
        }

    }

    fclose(arquivo);
    hash_apaga(&h);
    return EXIT_SUCCESS;
}


void carregaHash(thash *h, FILE *arq, int *maxcolisoes, int *totcolisoes, int *cont){
    char linha[60];
    char *start;
    char *end;
    int choice;
    int colisoes;
    tmunicipio temp;

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

                colisoes = 0;    
                if(hash_insere(h, aloca_municipio(temp.codigo_ibge, temp.nome, temp.latitude, temp.longitude, 
                    temp.capital, temp.codigo_uf, temp.siafi_id, temp.ddd, temp.fuso_horario), &colisoes) == EXIT_SUCCESS) *cont +=1;
                *totcolisoes += colisoes;
                if(colisoes>*maxcolisoes) *maxcolisoes = colisoes;
                                
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

void busca_municipio(thash *h){
    char leitura[10];
    tmunicipio *buscado;
    printf("Digite o Codigo IBGE a ser buscado: ");
    scanf(" %s", leitura);

    buscado = hash_busca(h, leitura);

    if(buscado == NULL){
        printf("Municipio nao encontrado no hash.\n");
    } else{
        printf("Informacoes relacionados ao codigo fornecido:\n");
        printf("codigo_ibge: %s,\n", buscado->codigo_ibge);
        printf("nome: %s,\n", buscado->nome);
        printf("latitude: %f,\n", buscado->latitude);
        printf("longitude: %f,\n", buscado->longitude);
        printf("capital: %d,\n", buscado->capital);
        printf("codigo_uf: %d,\n", buscado->codigo_uf);
        printf("siafi_id: %d,\n", buscado->siafi_id);
        printf("ddd: %d,\n", buscado->ddd);
        printf("fuso_horario: %s\n", buscado->fuso_horario);
    }
}