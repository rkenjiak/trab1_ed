#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "../include/mylib.h"

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

void carregaHash();

int main(){
    thash h;
    int nbuckets = 10000;
    tmunicipio *municipio;
    //hash_constroi(&h, nbuckets, get_key);

    //char* strchr( const char* fonte, int caractere) primeira, null se nao achar
    //char* strrchr( const char* fonte, int caractere) ultima
    char linha[60];
    char *start;
    char *end;

    char cod_ibge[10];
    char nome[40];
    float latitude;
    float longitude;
    int capital;
    int codigo_uf;
    int siafi_id;
    int ddd;
    char fuso_horario[40];

    int maxColisoes = 0;
    int contColisoes;
    int totColisoes = 0;
    int i=0;

    FILE *arquivo = fopen("./data/municipios.json", "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return EXIT_FAILURE;
    }
    while(1){
        if(fgets (linha, 60, arquivo)!=NULL) {
            start = strchr(linha,'\"');
            if(start == NULL) {
                continue;
            } else{
                ////// cod_ibge
                start = strchr(linha,':') + 1;
                end = strrchr(linha,',');
                *end = '\n';
                sscanf(start," %s", cod_ibge);

                ////// nome
                fgets (linha, 60, arquivo);
                start = strchr(linha,':') + 3;
                end = strrchr(linha,'\"');
                *end = '\n';
                sscanf(start," %[^\n]", nome);

                ////// latitude
                fgets (linha, 60, arquivo);
                start = strchr(linha,':') + 1;
                end = strrchr(linha,',');
                *end = '\n';
                sscanf(start,"%f", &latitude);

                ////// longitude
                fgets (linha, 60, arquivo);
                start = strchr(linha,':') + 1;
                end = strrchr(linha,',');
                *end = '\n';
                sscanf(start,"%f", &longitude);

                ////// capital
                fgets (linha, 60, arquivo);
                start = strchr(linha,':') + 1;
                end = strrchr(linha,',');
                *end = '\n';
                sscanf(start,"%d", &capital);

                ////// codigo_uf
                fgets (linha, 60, arquivo);
                start = strchr(linha,':') + 1;
                end = strrchr(linha,',');
                *end = '\n';
                sscanf(start,"%d", &codigo_uf);

                ////// siafi_id
                fgets (linha, 60, arquivo);
                start = strchr(linha,':') + 1;
                end = strrchr(linha,',');
                *end = '\n';
                sscanf(start,"%d", &siafi_id);

                ////// ddd
                fgets (linha, 60, arquivo);
                start = strchr(linha,':') + 1;
                end = strrchr(linha,',');
                *end = '\n';
                sscanf(start,"%d", &ddd);

                ////// fudo_horario
                fgets (linha, 60, arquivo);
                start = strchr(linha,':') + 3;
                end = strrchr(linha,'\"');
                *end = '\n';
                sscanf(start," %[^\n]",fuso_horario);
                i++;
            }
        } else{
            break;
        }
        if(i>5569){
        printf("1 %s\n", cod_ibge);
        printf("2 %s\n", nome);
        printf("3 %f\n", latitude);
        printf("4 %f\n", longitude);
        printf("5 %d\n", capital);
        printf("6 %d\n", codigo_uf);
        printf("7 %d\n", siafi_id);
        printf("8 %d\n", ddd);        
        printf("9 %s\n", fuso_horario); 
        }
        
        printf("%d\n", i);  
        
    }
    

   

        /*
        sprintf(municipio->codigo_ibge, "%d", cod_ibge);
        hash_insere(&h, aloca_municipio(municipio->codigo_ibge, municipio->nome, municipio->latitude, municipio->longitude, municipio->capital, municipio->codigo_uf, municipio->siafi_id, municipio->ddd, municipio->fuso_horario), &contColisoes);
        if(contColisoes>maxColisoes) maxColisoes=contColisoes;
        totColisoes+=contColisoes;  
        */


    //printf("Houve no maximo %d colisoes em uma insercao.\nHouve %d totais colisoes.\n", maxColisoes, totColisoes);


    fclose(arquivo);

    //hash_apaga(&h);
    return EXIT_SUCCESS;
}