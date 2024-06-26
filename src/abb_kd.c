#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<string.h>
#include"../include/libfacom.h"
#include<math.h>

void abb_constroi(tarv *parv, float (*cmpx)(void *, void *), float (*cmpy)(void *, void *), float(*distancia2)(void*,void*), char*(*get_key)(void *)){
    parv->raiz = NULL;
    parv->cmpx  = cmpx;
    parv->cmpy  = cmpy;
	parv->distancia2 = distancia2;
	parv->get_key = get_key;
}

tnode* newNode(void* reg){
    tnode* temp = (tnode*)malloc(sizeof(tnode));
    temp->reg = reg;
    temp->esq = temp->dir = NULL;
    return temp;
}

/* percorre alternando as comparacoes ate chegar em um NULL */
int abb_insere_node(tarv *parv,tnode **ppnode,void *reg, int depth){
	int dim = depth % 2;
    if (*ppnode == NULL){
        *ppnode = newNode(reg);	
		if(*ppnode == NULL) return EXIT_FAILURE;	 
    }else if(dim == 0){ //comparar latitude
        if (parv->cmpy((*ppnode)->reg,reg) > 0){ /* esquerda*/
            abb_insere_node(parv,&((*ppnode)->esq),reg,depth+1);
        }else{ /*direita*/
            abb_insere_node(parv,&((*ppnode)->dir),reg,depth+1);
        }
    }else if (dim == 1){ //comparar longitude
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

/* percorre até a folha, calcula a distancia e compara com a max heap, se for menor insere
   antes de subir verifica se a subarvore que não foi percorrida tem chance de ser vizinho proximo*/
void nearNeighbor_node(tarv *parv,tnode *pnode, theap *heap,void *reg, int depth, int tam){
	if(pnode == NULL) return;
	int dim = depth % 2;
		
	if(dim == 0){
		if (parv->cmpy((pnode)->reg,reg) > 0) nearNeighbor_node(parv,((pnode)->esq),heap,reg,depth+1,tam);/* esquerda*/
        else nearNeighbor_node(parv,((pnode)->dir),heap,reg,depth+1,tam); /*direita*/         
	}else if (dim == 1){ //comparar longitude
        if (parv->cmpx((pnode)->reg,reg) > 0) nearNeighbor_node(parv,((pnode)->esq),heap,reg,depth+1,tam); /* esquerda*/
        else nearNeighbor_node(parv,((pnode)->dir),heap,reg,depth+1,tam); /*direita*/         
	}	

	float currentDist = parv->distancia2(pnode->reg,reg);
	float bestDist = heap[0].distancia2;
	if(currentDist<bestDist){
		theap new;
		new.distancia2 = currentDist;
		strcpy(new.key,parv->get_key(pnode->reg));
		altera_prioridade(heap,tam,0,new);
	}

	if(dim == 1 && bestDist>=pow(parv->cmpx(pnode->reg,reg),2)){
		if (parv->cmpx((pnode)->reg,reg) > 0) nearNeighbor_node(parv,((pnode)->dir),heap,reg,depth+1,tam); /* esquerda, oposto */
        else nearNeighbor_node(parv,((pnode)->esq),heap,reg,depth+1,tam); /* direita, oposto */        
	}else if(dim == 0 && bestDist>=pow(parv->cmpy(pnode->reg,reg),2)){
		if (parv->cmpy((pnode)->reg,reg) > 0) nearNeighbor_node(parv,((pnode)->dir),heap,reg,depth+1,tam);/* esquerda, oposto */
        else nearNeighbor_node(parv,((pnode)->esq),heap,reg,depth+1,tam); /* direita, oposto*/    
	}	
}

void nearNeighbor(tarv *parv, theap *heap,  void *reg, int depth, int tam){
    return nearNeighbor_node(parv, parv->raiz, heap, reg, depth, tam);
}

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

	if (esq < tam && v[esq].distancia2 > v[maior].distancia2) 
		maior = esq;
	if (dir < tam && v[dir].distancia2 > v[maior].distancia2)
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

	if (v[p].distancia2 < v[n].distancia2) {
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

void altera_prioridade(theap *v, int tam, int n, theap new) {
	theap anterior = v[n];
	v[n] = new;

	if (new.distancia2 > anterior.distancia2)
		sobe(v, n);
	if (new.distancia2 < anterior.distancia2)
		desce(v, tam, n);
}

void heap_sort(theap v[], int tam) {
	for (int i = tam - 1; i > 0; i--) {
		troca(&v[0], &v[i]);
		desce(v, i, 0);
	}
}

void abb_apaga(tnode *node){
    if (node == NULL) return;
    abb_apaga(node->esq);
    abb_apaga(node->dir);
    free(node);
}