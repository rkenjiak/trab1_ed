#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<string.h>
#include"../include/libfacom.h"

float dist2(tarv *parv, void *t1, void *t2){
    float dlat = parv->cmpy(t1, t2);
    float dlong = parv->cmpx(t1, t2);
    return dlat*dlat + dlong*dlong;
}

void abb_constroi(tarv *parv, float (*cmpx)(void *, void *), float (*cmpy)(void *, void *)){
    parv->raiz = NULL;
    parv->cmpx  = cmpx;
    parv->cmpy  = cmpy;
}

int abb_insere_node(tarv *parv,tnode **ppnode,void *reg, int depth){
    if (*ppnode == NULL){
         *ppnode = malloc(sizeof(tnode));
         (*ppnode)->reg = reg;
         (*ppnode)->esq = NULL;
         (*ppnode)->dir = NULL;
    }
    if(depth % 2 == 0){ //comparar latitude
        if (parv->cmpy((*ppnode)->reg,reg) > 0){ /* esquerda*/
            abb_insere_node(parv,&((*ppnode)->esq),reg,depth+1);
        }else{ /*direita*/
            abb_insere_node(parv,&((*ppnode)->dir),reg,depth+1);
        }
    }else{ //comparar longitude
        if (parv->cmpx((*ppnode)->reg,reg) > 0){ /* esquerda*/
            abb_insere_node(parv,&((*ppnode)->esq),reg,depth+1);
        }else{ /*direita*/
            abb_insere_node(parv,&((*ppnode)->dir),reg,depth+1);
        }
    }
    return EXIT_SUCCESS;
}

int abb_insere(tarv *parv,  void *reg, int depth){
    return abb_insere_node(parv,&parv->raiz,reg, depth);
}


void * nearNeighbor_node(tarv *parv,tnode *pnode,void *reg, int depth){

}

void * nearNeighbor(tarv *parv,  void *reg, int depth){
    return nearNeighbor_node(parv, parv->raiz, reg, depth);
}


///////////////////////////////////////////////////////

void troca(theap *a, theap *b) {
	theap aux = *a;
	*a = *b;
	*b = aux;
}

int pai(int n) {
	return (n - 1) / 2;
}

int filho_esq(int n) {
	return (n * 2) + 1;
}

int filho_dir(int n) {
	return (n * 2) + 2;
}

void desce(theap v[], int tam, int n) {
	int maior = n;
	int esq = filho_esq(n);
	int dir = filho_dir(n);

	if (esq < tam && v[esq].distancia > v[maior].distancia) 
		maior = esq;
	if (dir < tam && v[dir].distancia > v[maior].distancia)
		maior = dir;

	if (maior != n) {
		troca(&v[n], &v[maior]);
		desce(v, tam, maior);
	}
}

void constroi_heap(theap v[], int tam) {
	for (int i = pai(tam); i >= 0; i--) {
		desce(v, tam, i);
	}
}

void sobe(theap v[], int n) {
	int p = pai(n);

	if (v[p].distancia < v[n].distancia) {
		troca(&v[n], &v[p]);
		sobe(v, p);
	}
}

theap extrai_raiz(theap v[], int *tam) {
	theap max = v[0];
	*tam -= 1;
	troca(&v[0], &v[*tam]);
	desce(v, *tam, 0);
	return max;
}

void altera_prioridade(theap *v, int tam, int n, theap new) { // ?
	theap anterior = v[n];
	v[n] = new;

	if (new.distancia > anterior.distancia)
		sobe(v, n);
	if (new.distancia < anterior.distancia)
		desce(v, tam, n);
}

////////////////////////////////////////////////////////

void abb_apaga(tnode *node){
    if (node == NULL) return;
    abb_apaga(node->esq);
    abb_apaga(node->dir);
    free(node);
}