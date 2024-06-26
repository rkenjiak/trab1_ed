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
    float (*distancia2)(void*, void*);
    char *(*get_key)(void *);
}tarv;

 /* heap para n vizinhos na kdtree 2d */
typedef struct _heap{
    char key[20];
    float distancia2;
}theap;

/* funcoes para hash */
uint32_t hashf(const char* str, uint32_t h);
uint32_t hashf2(const char* data);
int hash_insere(thash *h, void *bucket, int *cont);
int hash_constroi(thash *h,int nbuckets, char *(*get_key)(void *));
void * hash_busca(thash *h, const char *key);
int qtd_ocorrencias_hash(thash  *h, const char * key);
void hash_busca_ate_0(thash  *h, void * lista, const char * key, int repeticoes, size_t tamanho_dados);
int hash_remove(thash *h, const char *key);
void hash_apaga(thash *h);

/* funcoes para abb/kdtree */
void abb_constroi(tarv *parv, float (*cmpx)(void *, void *), float (*cmpy)(void *, void *), float(*distancia2)(void*,void*), char *(*get_key)(void *));
tnode* newNode(void* reg);
int abb_insere_node(tarv *parv, tnode **ppnode, void *reg, int depth);
int abb_insere(tarv *parv, void *reg, int depth);
void nearNeighbor_node(tarv *parv,tnode *pnode, theap *heap,void *reg, int depth, int n);
void nearNeighbor(tarv *parv, theap *heap,  void *reg, int depth, int n);
void abb_apaga(tnode *node);

/* funcoes para heap que utliza a struct theap acima */
void troca(theap *a, theap *b);
int pai(int n);
int filho_esq(int n);
int filho_dir(int n);
void desce(theap v[], int tam, int n);
void constroi_heap(theap v[], int tam);
void sobe(theap v[], int n);
theap extrai_raiz(theap v[], int *tam);
void altera_prioridade(theap *v, int tam, int n, theap new);
void heap_sort(theap v[], int tam);

#endif