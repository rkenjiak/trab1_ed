#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../include/libfacom.h"

#define SEED    0x12345678
#define FNV_PRIME 16777619
#define FNV_OFFSET_BASIS 2166136261U

uint32_t hashf(const char* str, uint32_t h){
    /* One-byte-at-a-time Murmur hash 
    Source: https://github.com/aappleby/smhasher/blob/master/src/Hashes.cpp */
    for (; *str; ++str) {
        h ^= *str;
        h *= 0x5bd1e995;
        h ^= h >> 15;
    }
    return h;
}

uint32_t hashf2(const char* data) { //fnv1a_hash
    uint32_t hash = FNV_OFFSET_BASIS;
    const unsigned char* ptr = (const unsigned char*)data;
    while (*ptr != '\0') {
        hash ^= *ptr;
        hash *= FNV_PRIME;
        ++ptr;
    }
    return hash;
}

int hash_insere(thash * h, void * bucket, int *cont){
    int j = 0;
    uint32_t hash = hashf(h->get_key(bucket),SEED);
    uint32_t hash2 = hashf2(h->get_key(bucket));
    int pos = hash %(h->max);
    /*se esta cheio*/
    if (h->max == (h->size+1)){
        free(bucket);
        return EXIT_FAILURE;
    }else{  /*fazer a insercao*/
        while(h->table[pos] != 0){
            if (h->table[pos] == h->deleted)
                break;
            j++;
            pos = (hash+j*hash2)%h->max;
        }
        h->table[pos] = (uintptr_t)bucket;
        h->size +=1;
    }
    *cont = j;
    return EXIT_SUCCESS;
}

int hash_constroi(thash * h,int nbuckets, char * (*get_key)(void *) ){
    h->table = calloc(sizeof(void *),nbuckets + 1);
    if (h->table == NULL){
        return EXIT_FAILURE;
    }
    h->max = nbuckets + 1;
    h->size = 0;
    h->deleted = (uintptr_t)&(h->size);
    h->get_key = get_key;
    return EXIT_SUCCESS;
}

/* retorna um *tmunicipio, se achar, NULL caso contrario */
void * hash_busca(thash  *h, const char * key){
    int j = 0;
    uint32_t hash = hashf(key,SEED);
    uint32_t hash2 = hashf2(key);
    int pos = hash %(h->max);
    void * ret = NULL;
    while(h->table[pos]!=0 && ret == NULL){
        void *bucket = (void *)h->table[pos];
        if (strcmp(h->get_key(bucket),key) == 0){
            ret = bucket;
        }else{
            j++;
            pos = (hash+j*hash2) % h->max;
        }
    }
    return ret;
}

/* descobre a qtd de repeticoes  */
int qtd_ocorrencias_hash(thash  *h, const char * key){
    int j = 0;
    uint32_t hash = hashf(key,SEED);
    uint32_t hash2 = hashf2(key);
    int pos = hash %(h->max);
    int ret = 0;
    while(h->table[pos]!=0){
        void *bucket = (void *)h->table[pos];
        if (strcmp(h->get_key(bucket),key) == 0){
            ret += 1;            
        }
        j++;
        pos = (hash+j*hash2) % h->max;
    }
    return ret;
}

/* insere tmunicipio na lista, sabendo a qtd */
void hash_busca_ate_0(thash  *h, void *lista, const char *key, int repeticoes, size_t tamanho_dados){ 
    int i = 0,j = 0;
    uint32_t hash = hashf(key,SEED);
    uint32_t hash2 = hashf2(key);
    int pos = hash %(h->max);    
    while(h->table[pos]!=0 && i<repeticoes){
        void *bucket = (void *)h->table[pos];        
        if (strcmp(h->get_key(bucket),key) == 0){
            memcpy(lista+i*tamanho_dados, bucket, tamanho_dados);
            i++;
        }        
        j++;
        pos = (hash+j*hash2) % h->max;        
    }
}

int hash_remove(thash * h, const char * key){
    int j = 0;
    uint32_t hash = hashf(key,SEED);
    uint32_t hash2 = hashf2(key);
    int pos = hash % (h->max);
    while(h->table[pos]!=0){
        if (strcmp(h->get_key((void*)h->table[pos]),key) == 0){ /* se achei remove*/
            free((void *)h->table[pos]);
            h->table[pos] = h->deleted;
            h->size -= 1;
            return EXIT_SUCCESS;
        }else{
            j++;
            pos = (hash+j*hash2) % h->max;
        }
    }
    return EXIT_FAILURE;
}

void hash_apaga(thash *h){
    int pos;
    for (pos =0; pos < h->max;pos++){
        if (h->table[pos] != 0){
            if (h->table[pos]!=h->deleted){
                free((void*) h->table[pos]);
            }
        }
    }
    free(h->table);
}