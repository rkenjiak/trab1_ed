#ifndef __LIBFACOM__
#define __LIBFACOM__
#include <stdint.h>

/* hash */
typedef struct {
        uintptr_t *table;
        int size;
        int max;
        uintptr_t deleted;
        char *(*get_key)(void *);
}thash;

uint32_t hashf(const char* str, uint32_t h);
uint32_t hashf2(const char* data);
int hash_insere(thash *h, void *bucket, int *cont);
int hash_constroi(thash *h,int nbuckets, char *(*get_key)(void *));
void * hash_busca(thash *h, const char *key);
int hash_remove(thash *h, const char *key);
void hash_apaga(thash *h);

/* ABB */
typedef struct _tnode{
    void *reg;
    struct _tnode *esq;
    struct _tnode *dir;
}tnode;

typedef struct _abb{ // 2 dimensoes
    tnode *raiz;
    float (*cmpx)(void* , void*);
    float (*cmpy)(void* , void*);
}tarv;

float dist2(tarv *parv, void *t1, void *t2);
void abb_constroi(tarv *parv, float (*cmpx)(void *, void *), float (*cmpy)(void *, void *));
int abb_insere_node(tarv *parv, tnode **ppnode, void *reg, int depth);
int abb_insere(tarv *parv, void *reg, int depth);
void * abb_busca_node(tarv *parv, tnode *pnode, void *reg);
void * abb_busca(tarv *parv, void *reg);

#endif