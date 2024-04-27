#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../include/libfacom.h"

void troca(void *a, void *b) {
	void *aux = *a;
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

void desce(void *v, int tam, int n) {
	int maior = n;
	int esq = filho_esq(n);
	int dir = filho_dir(n);

	if (esq < tam && v[esq] > v[maior]) 
		maior = esq;
	if (dir < tam && v[dir] > v[maior])
		maior = dir;

	if (maior != n) {
		troca(&v[n], &v[maior]);
		desce(v, tam, maior);
	}
}

void constroi_heap(int v[], int tam) {
	for (int i = pai(tam); i >= 0; i--) {
		desce(v, tam, i);
	}
}

void sobe(int v[], int n) {
	int p = pai(n);

	if (v[p] < v[n]) {
		troca(&v[n], &v[p]);
		sobe(v, p);
	}
}

int extrai_raiz(int v[], int *tam) {
	int max = v[0];
	*tam -= 1;
	troca(&v[0], &v[*tam]);
	desce(v, *tam, 0);
	return max;
}

void altera_prioridade(void *v, int tam, int n, int valor) { // ?
	int anterior = v[n];
	v[n] = valor;

	if (valor > anterior)
		sobe(v, n);
	if (valor < anterior)
		desce(v, tam, n);
}

void heap_sort(int v[], int tam) {
	for (int i = tam - 1; i > 0; i--) {
		troca(&v[0], &v[i]);
		desce(v, i, 0);
	}
}

int insere_elemento(int v[], int *tam, int max, int valor) {
	if (*tam == max)
		return EXIT_FAILURE;

	v[*tam] = valor;
	sobe(v, *tam);
	*tam += 1;

	return EXIT_SUCCESS;
}

