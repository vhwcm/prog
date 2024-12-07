// Cria uma fila vazia com a capacidade informada e a retorna;
// Retorna NULL em caso de erro
#include <stdio.h>
#include <stdlib.h>
#include "fila.h"


struct fila_t *fila_cria (int capacidade){
  struct fila_t *novaFila = malloc(sizeof(struct fila_t));
  novaFila->nItens = 0;
  novaFila->capacidade = capacidade;
  novaFila->prim =  NULL;
  novaFila->ult = NULL;
  return novaFila;
}

// Remove todos os elementos da fila, libera memória e retorna NULL


// Insere o elemento no final da fila (politica FIFO);
// Retorna o número de elementos na fila após a operação
// ou -1 em caso de erro
int fila_insere (struct fila_t *f, int elem){
    if(f->capacidade <= f->nItens){
        return -1;
    }

    struct fila_nodo *novoNodo = malloc(sizeof(struct fila_nodo));
    if (novoNodo == NULL) {
        return -1;
    }
    novoNodo->data = elem;
    novoNodo->p_nodo = NULL;

    if (f->prim == NULL) {
        f->prim = novoNodo;
    } else {
        f->ult->p_nodo = novoNodo;
    }
    f->ult = novoNodo;
    (f->nItens)++;

    return f->nItens;
}

// Retira o elemento do inicio da fila (politica FIFO) e o devolve;
// Retorna o número de elementos na fila após a operação
// ou -1 em caso de erro
int fila_retira(struct fila_t *f, int *elem) {
    if (f->nItens <= 0) {
        return -1;
    }

    struct fila_nodo *nodoRemovido = f->prim;
    if (elem != NULL) {
        *elem = nodoRemovido->data;
    }
    f->prim = nodoRemovido->p_nodo;

    free(nodoRemovido);
    (f->nItens)--;

    // Se a fila ficou vazia, atualize o ponteiro para o último nodo
    if (f->nItens == 0) {
        f->ult = NULL;
    }

    return f->nItens;
}

// Devolve o primeiro da fila, sem removê-lo
// Retorna o número de elementos na fila ou -1 em caso de erro
int fila_primeiro (struct fila_t *f, int *elem){
    if(f->nItens <= 0){
        return -1;
    }

    *elem = f->prim->data;
    return f->nItens;
}

struct fila_t *fila_destroi(struct fila_t *f) {
    if (f == NULL) {
        return NULL; // Verifica se a fila é nula
    }

    struct fila_nodo *nodo = f->prim;
    while (nodo != NULL) {
        struct fila_nodo *prox = nodo->p_nodo;
        free(nodo); // Libera a memória do nodo atual
        nodo = prox; // Avança para o próximo nodo
    }

    // Verifica se o ponteiro f é válido antes de liberar a memória
    
    // free(f); // Libera a memória da estrutura da fila

    return NULL; // Retorna NULL para indicar que a fila foi destruída
}

// Retorna o tamanho da fila (número de elementos presentes)
int fila_tamanho (struct fila_t *f){
    return f->nItens;
}

// Retorna a capacidade da fila (número máximo de elementos)
int fila_capacidade (struct fila_t *f){
    return f->capacidade;
}

// Imprime o conteúdo da fila do início ao fim, no formato "item item ...",
// com um espaço entre itens, sem espaços antes/depois e sem \n no fim
void fila_imprime (struct fila_t *f){
    if(f->nItens == 0) return;
    struct fila_nodo *fl = f->prim;
    while (fl != f->ult)
    {
        printf("%d ",fl->data);
        fl = fl->p_nodo;
    }
    printf("%d",fl->data);
    return;
}


