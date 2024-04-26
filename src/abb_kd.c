#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<string.h>
#include"../include/libfacom.h"

void abb_constroi(tarv *parv, int (*cmp)(void *, void *)){
    parv->raiz = NULL;
    parv->cmp  = cmp;
}

int abb_insere_node(tarv *parv,tnode **ppnode,void *reg){
    if (*ppnode == NULL){
         *ppnode = malloc(sizeof(tnode));
         (*ppnode)->reg = reg;
         (*ppnode)->esq = NULL;
         (*ppnode)->dir = NULL;
    }else if (parv->cmp((*ppnode)->reg,reg) > 0){ /* esquerda*/
        abb_insere_node(parv,&((*ppnode)->esq),reg);
    }else{ /*direita*/
        abb_insere_node(parv,&((*ppnode)->dir),reg);
    }
}

int abb_insere(tarv *parv,  void *reg){
    return abb_insere_node(parv,&parv->raiz,reg);
}

void * abb_busca_node(tarv *parv,tnode *pnode,void *reg){
    tnode * ret;
    if (pnode == NULL){
        ret = NULL;
    }else if (parv->cmp(pnode->reg,reg) > 0){ /* esquerda*/
        ret = abb_busca_node(parv,pnode->esq,reg);
    }else if (parv->cmp(pnode->reg,reg) < 0) { /*direita*/
        ret = abb_busca_node(parv,pnode->dir,reg);
    }else{
        ret = pnode->reg;
    }
    return ret;
}

void * abb_busca(tarv *parv,  void *reg){
    return abb_busca_node(parv,parv->raiz,reg);
}


void abb_apaga(tnode *node){
    if (node == NULL) return;
    abb_apaga(node->esq);
    abb_apaga(node->dir);
    free(node);
}