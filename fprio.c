#include "fprio.h"
#include <stdlib.h>
#include <stdio.h>

//cria uma fila de prioridade vazia
struct fprio_t *fprio_cria (){
    struct fprio_t* fp = malloc(sizeof(struct fprio_t));
    if(!fp)
        return NULL;
    fp->num = 0;
    fp->prim = NULL;
    return fp;
}

//libera toda a memória da fila e seus itens, depois retorna NULL
struct fprio_t *fprio_destroi (struct fprio_t *f){
    if(!f)
        return NULL;
    struct fpnodo_t* aux = f->prim;
    struct fpnodo_t* pont;
    //destruindo os nodos
    while(aux != NULL){
        pont = aux;
        aux = aux->prox;
        if(pont->item)
            free(pont->item);
        free(pont);
    }
    free(f);
    f = NULL;
    return f;
}


//verifica se da para inserir e insere na posição indicada
int fprio_insere(struct fprio_t *f, void *item, int tipo, int prio) {
    if (!f || item == NULL)
        return -1;
        

    // Verifica se o item já está na fila
    struct fpnodo_t* temp = f->prim;
    while (temp != NULL) {
        if (temp->item == item)
            return -1;
        temp = temp->prox;
    }

    // Cria um novo nó
    struct fpnodo_t* nodo = malloc(sizeof(struct fpnodo_t));
    if (!nodo)
        return -1;

    nodo->tipo = tipo;
    nodo->prio = prio;
    nodo->item = item;
    nodo->prox = NULL;

    // Insere o nó na posição correta
    if (f->num == 0 || prio < f->prim->prio) {
        // Insere no início
        nodo->prox = f->prim;
        f->prim = nodo;
    } else {
        struct fpnodo_t* aux = f->prim;
        while (aux->prox != NULL && aux->prox->prio <= prio) {
            aux = aux->prox;
        }
        nodo->prox = aux->prox;
        aux->prox = nodo;
    }
    (f->num)++;
    return f->num;
}


//retira o primeiro item da fila e devolve o seu ponteiro
void *fprio_retira (struct fprio_t *f, int *tipo, int *prio){
    //verifica se há erros
    if(!f || f->num == 0 || tipo == NULL || prio == NULL)
        return NULL;

    //armazena nas variaveis o tipo e a prioridade
    void *item = f->prim->item;
    *tipo =  f->prim->tipo;
    *prio =  f->prim->prio;

    //removendo e dando free
    struct fpnodo_t* aux = f->prim;
    f->prim = f->prim->prox;
    free(aux);
    (f->num)--;

    return item;
}

int fprio_tamanho (struct fprio_t *f){
    if(!f || f->num <0)
        return -1;
    return f->num;
}

//imprime o tipo e a prioridade de todos os itens da fila
void fprio_imprime (struct fprio_t *f){
    if(!f || f->num == 0)
        return;
    struct fpnodo_t* aux = f->prim;
    //percorre a fila
    while(aux->prox != NULL){
        printf("(%d %d) ",aux->tipo,aux->prio);
        aux = aux->prox;
    }
    printf("(%d %d)",aux->tipo,aux->prio);
    return;
}
