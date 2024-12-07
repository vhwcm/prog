// programa principal do projeto "The Boys - 2024/2"
// Autor: Victor Hugo Weigmann Chequer Maia, GRR 20245275

// seus #includes vão aqui
#include "theboys.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

void destruir_mundo(struct mundo *m) ;

void cria_mundo(struct mundo *m);

// programa principal
int main () {
  // iniciar o mundo
  srand(time(NULL));
  struct mundo *m = malloc(sizeof(struct mundo));
  if(!m){
    printf("Erro ao alocar memória para o mundo");
    return 1;
  }

  cria_mundo(m);
  int tempo;

  for(int i = 0; i < N_HEROIS; i++){
    int base = rand() % m->numBases;
    tempo = rand() % 4321;
    cria_evento(i , base, m->lef, CHEGA, tempo);
  }

  for(int i = 0; i < m->numMissoes; i++){
    tempo = rand() % (T_FIM_DO_MUNDO + 1);
    cria_evento(i, NADA, m->lef, MISSAO, tempo);
  }

  tempo = T_FIM_DO_MUNDO;
  cria_evento(NADA, NADA, m->lef, FIM, tempo);
  struct args *args;
  int evento;
  char fimExecucao = 0;

  /* Loop principal */
  while(!fimExecucao){
    args = fprio_retira(m->lef, &evento, &tempo);
    (m->qntEventos)++;
    switch (evento) {
        case ESPERA:
            espera(m, tempo, args->a, args->b);
            break;
        case DESISTE:
            desiste(m, tempo, args->a, args->b);
            break;
        case AVISA:
            avisa(m, tempo, args->a);
            break;
        case VIAJA:
            viaja(m, tempo, args->a, args->b);
            break;
        case ENTRA:
            entra(m, tempo, args->a, args->b);
            break;
        case SAI:
            sai(m, tempo, args->a, args->b);
            break;
        case MISSAO:
            missao(m, tempo, args->a);
            break;
        case MORRE:
            morre(m, tempo, args->a, args->b);
            break;
        case CHEGA:
            chega(m, tempo, args->a, args->b);
            break;
        case FIM:
            fimExecucao = fim(m, tempo);
            break;
        default:
            printf("Evento desconhecido: %d\n", evento);
            break;
    }
  }


  // destruir o mundo
  destruir_mundo(m);

  return 0;
}

void cria_mundo(struct mundo *m) {
    m->numHerois = N_HEROIS;
    m->numBases = N_BASES;
    m->numMissoes = N_MISSOES;
    m->numHabs = N_HABILIDADES;
    m->tamMundo = N_TAMANHO_MUNDO;
    m->lef = fprio_cria();
    m->qntEventos = 0;
    m->missoesCump = 0;
    m->mortos = 0;
    
    if (!m->lef) {
        printf("Erro ao criar a lista de eventos futuros\n");
        return;
    }

    // Inicializa os heróis
    for (int i = 0; i < m->numHerois; i++) {
        m->herois[i].status = 1;
        m->herois[i].id = i;
        m->herois[i].xp = 0;
        m->herois[i].pacie = rand() % 101;
        m->herois[i].vel = 50 + (rand() % 4951);
        char tAleat = 1 + rand() % 3;
        m->herois[i].habs = *cjto_aleat(tAleat,m->numHabs);
    }

    // Inicializa as bases
    for (int i = 0; i < m->numBases; i++) {
        m->bases[i].id = i;
        m->bases[i].local.x = rand() % m->tamMundo;
        m->bases[i].local.y = rand() % m->tamMundo;
        m->bases[i].cap = 3 + rand() % 8;
        m->bases[i].h_ids = *cjto_cria(m->numHerois);
        m->bases[i].fila = *fila_cria(m->numHerois);
        m->bases[i].numMaxFila = 0;
        m->bases[i].qntMissoes = 0;

    }

    // Inicializa as missões
    for (int i = 0; i < m->numMissoes; i++) {
        m->missoes[i].id = i;
        m->missoes[i].local.x = rand() % m->tamMundo;
        m->missoes[i].local.y = rand() % m->tamMundo;
        char tAleat = 6 + rand() % 5;
        m->missoes[i].habs = *cjto_aleat(tAleat, m->numHabs);
        m->missoes[i].perigo = rand() % 101;
        m->missoes[i].tentativas = 0;
        m->missoes[i].bases = malloc(m->numBases * sizeof(int));
        if (!m->missoes[i].bases) {
            printf("Erro ao alocar memória para bases da missão %u\n", i);
            return;
        }
        ordenaBasesPorDistancia(m, i);
    }
}

void destroi_cjto(struct cjto_t *cjto) {
    if (cjto) {
        if (cjto->flag) {
            free(cjto->flag); // Libera a memória alocada para o vetor de booleanos
            cjto->flag = NULL;
        }
        cjto->cap = 0;
        cjto->num = 0;
    }
}

void destruir_mundo(struct mundo *m) {
    if (!m) {
        return;
    }

    // Libera a memória alocada para a lista de eventos futuros
    if (m->lef) {
        fprio_destroi(m->lef);
    }

    // Libera a memória alocada para os conjuntos e filas das bases
    for (int i = 0; i < m->numBases; i++) {
        destroi_cjto(&m->bases[i].h_ids);
        fila_destroi(&m->bases[i].fila);
    }

    //Libera a memória alocada para os conjuntos de habilidades dos heróis
    for (int i = 0; i < m->numHerois; i++) {
        destroi_cjto(&m->herois[i].habs);
    }

    // Libera a memória alocada para os conjuntos de habilidades das missões
    for (int i = 0; i < m->numMissoes; i++) {
        destroi_cjto(&m->missoes[i].habs);
    }

    // Libera a memória alocada para o mundo
    free(m);
    m = NULL;
}
