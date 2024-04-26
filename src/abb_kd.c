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

}

int abb_insere(tarv *parv,  void *reg, int depth){
    return abb_insere_node(parv,&parv->raiz,reg, depth);
}

void * nearNeighbo_node(tarv *parv,tnode *pnode,void *reg, int depth){

}

void * nearNeighbor(tarv *parv,  void *reg, int depth){
    return nearNeighbo_node(parv, parv->raiz, reg, depth);
}


////////////////////////////////////////////////////////

void * abb_busca_node(tarv *parv,tnode *pnode,void *reg){
    tnode * ret;
    if (pnode == NULL){
        ret = NULL;
    }
    if (parv->cmp(pnode->reg,reg) > 0){ /* esquerda*/
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

/////////////////////////////////////////////////////////

void abb_apaga(tnode *node){
    if (node == NULL) return;
    abb_apaga(node->esq);
    abb_apaga(node->dir);
    free(node);
}