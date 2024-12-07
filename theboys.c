#include "theboys.h"
#include "conjunto.h"
#include "fprio.h"
#include <stdio.h>
#include <stdlib.h>


void trocar(int vetor[], int a, int b) {
    int temp = vetor[a];
    vetor[a] = vetor[b];
    vetor[b] = temp;
}

void maxHeapify(int heap[], int dist[], int pos, int tam) {
    int e = pos * 2 + 1;
    int d = pos * 2 + 2;
    int maior = pos;

    if (e < tam && dist[heap[e]] > dist[heap[maior]])
        maior = e;
    if (d < tam && dist[heap[d]] > dist[heap[maior]])
        maior = d;

    if (maior != pos) {
        trocar(heap, pos, maior);
        maxHeapify(heap, dist, maior, tam);
    }
}

void criaMaxHeap(int heap[], int dist[], int tam) {
    for (int i = tam / 2 - 1; i >= 0; i--) {
        maxHeapify(heap, dist, i, tam);
    }
}

void heapSort(int heap[], int dist[], int tam) {
    criaMaxHeap(heap, dist, tam);
    for (int i = tam - 1; i > 0; i--) {
        trocar(heap, 0, i);
        maxHeapify(heap, dist, 0, i);
    }
}

void ordenaBasesPorDistancia(struct mundo *m, int missao) {
    int numBases = m->numBases;
    int *distancias = malloc(numBases * sizeof(int));
    if (!distancias) {
        printf("Erro ao alocar memória para distâncias\n");
        return;
    }

    // Calcula a distância de cada base até a missão
    for (int i = 0; i < numBases; i++) {
        distancias[i] = disCartesiana(m->missoes[missao].local.x, m->missoes[missao].local.y,
                                      m->bases[i].local.x, m->bases[i].local.y);
    }

    // Inicializar o array de índices das bases
    for (int i = 0; i < numBases; i++) {
        m->missoes[missao].bases[i] = i;
    }

    // Ordenar as bases por distância usando heapsort
    heapSort(m->missoes[missao].bases, distancias, numBases);

    free(distancias);
}

void chega(struct mundo *m, int tempo, int h, int b) {
    if (m->herois[h].status == 0)
        return;

    // Atualiza a base do herói
    m->herois[h].base = b;
    char espera;

    // Verifica se o herói deve esperar ou desistir
    if (m->bases[b].h_ids.num < m->bases[b].cap && m->bases[b].fila.nItens == 0)
        espera = 1;
    else
        espera = m->herois[h].pacie > 10 * m->bases[b].fila.nItens;

    // Aloca memória para args
    struct args *args = malloc(sizeof(struct args));
    if (!args) {
        printf("Erro ao alocar memória para args\n");
        return;
    }
    args->a = m->herois[h].id;
    args->b = m->bases[b].id;

    if (espera) {
        printf("%6d: CHEGA  HEROI %2d BASE %d (%2d/%2d) ESPERA\n", tempo, h, b, cjto_card(&m->bases[b].h_ids), m->bases[b].cap);
        fprio_insere(m->lef, args, ESPERA, tempo);
    } else {
        fprio_insere(m->lef, args, DESISTE, tempo);
        printf("%6d: CHEGA  HEROI %2d BASE %d (%2d/%2d) DESISTE\n", tempo, h, b, cjto_card(&m->bases[b].h_ids), m->bases[b].cap);
    }
}

void espera(struct mundo *m, int tempo, int h, int b) {
    if(m->herois[h].status == 0)
        return;
    printf("%6d: ESPERA HEROI %2d BASE %d (%2d)\n", tempo, h, b, fila_tamanho(&m->bases[b].fila));
    fila_insere(&m->bases[b].fila, m->herois[h].id);
    if(fila_tamanho(&m->bases[b].fila) > m->bases[b].numMaxFila)
        m->bases[b].numMaxFila = fila_tamanho(&m->bases[b].fila);

    // Aloca memória para args do evento AVISA
    struct args *args = malloc(sizeof(struct args));
    if (!args) {
        printf("Erro ao alocar memória para args\n");
        return;
    }
    args->a = m->bases[b].id;
    args->b = 0;
    fprio_insere(m->lef, args, AVISA, tempo);
}

void desiste(struct mundo *m, int tempo, int h, int b) {
    if(m->herois[h].status == 0)
        return;
    printf("%6d: DESIST HEROI %2d BASE %d\n", tempo, h, b);
    int bAleat = rand() % m->numBases;

    // Aloca memória para args do evento VIAJA
    struct args *args = malloc(sizeof(struct args));
    if (!args) {
        printf("Erro ao alocar memória para args\n");
        return;
    }
    args->a = m->herois[h].id;
    args->b = bAleat;
    fprio_insere(m->lef, args, VIAJA, tempo);
}

int raiz(int n) {
    if (n == 0) return 0;
    int x = n;
    int y = (x + 1) / 2;
    while (y < x) {
        x = y;
        y = (x + n / x) / 2;
    }
    return x;
}

void avisa(struct mundo *m, int tempo, int b) {
    printf("%6d: AVISA  PORTEIRO BASE %d (%2d/%2d) FILA [ ", tempo, b, fila_tamanho(&m->bases[b].fila), m->bases[b].cap);
    fila_imprime(&m->bases[b].fila);
    printf(" ]\n");

    while (cjto_card(&m->bases[b].h_ids) < m->bases[b].cap && fila_tamanho(&m->bases[b].fila) > 0) {
        int h = 1;
        fila_retira(&m->bases[b].fila, &h);
        printf("%6d: AVISA  PORTEIRO BASE %d ADMITE %2d\n", tempo, b, h);
        cjto_insere(&m->bases[b].h_ids, h);

        // Aloca memória para args do evento ENTRA
        struct args *args_entra = malloc(sizeof(struct args));
        if (!args_entra) {
            printf("Erro ao alocar memória para args_entra\n");
            return;
        }
        args_entra->a = h;
        args_entra->b = b;
        fprio_insere(m->lef, args_entra, ENTRA, tempo);
    }
}

void entra(struct mundo *m, int tempo, int h, int b) {
    if(m->herois[h].status == 0)
        return;
    int tpb = 15 + m->herois[h].pacie * ((rand() % 20) + 1);
    printf("%6d: ENTRA  HEROI %2d BASE %d (%2d/%2d) SAI %d\n", tempo, h, b, fila_tamanho(&m->bases[b].fila), m->bases[b].cap, tempo + tpb);

    // Aloca memória para args do evento SAI
    struct args *args_sai = malloc(sizeof(struct args));
    if (!args_sai) {
        printf("Erro ao alocar memória para args_sai\n");
        return;
    }
    args_sai->a = m->herois[h].id;
    args_sai->b = m->bases[b].id;
    fprio_insere(m->lef, args_sai, SAI, tempo + tpb);
}

void sai(struct mundo *m, int tempo, int h, int b) {
    if(m->herois[h].status == 0)
        return;

    cjto_retira(&m->bases[b].h_ids, h);
    int bAleat = rand() % m->numBases;
    printf("%6d: SAI    HEROI %2d BASE %d (%2d/%2d)\n", tempo, h, b, fila_tamanho(&m->bases[b].fila), m->bases[b].cap);

    // Aloca memória para args do evento VIAJA
    struct args *args_viaja = malloc(sizeof(struct args));
    if (!args_viaja) {
        printf("Erro ao alocar memória para args_viaja\n");
        return;
    }
    args_viaja->a = h;
    args_viaja->b = bAleat;
    fprio_insere(m->lef, args_viaja, VIAJA, tempo);

    // Aloca memória para args do evento AVISA
    struct args *args_avisa = malloc(sizeof(struct args));
    if (!args_avisa) {
        printf("Erro ao alocar memória para args_avisa\n");
        return;
    }
    args_avisa->a = b;
    args_avisa->b = -1;
    fprio_insere(m->lef, args_avisa, AVISA, tempo);
}

int disCartesiana(int x1, int y1, int x2, int y2){
    int co = (y1 > y2) ? (y1 - y2) : (y2 - y1);
    int ca = (x1 > x2) ? (x1 - x2) : (x2 - x1);
    return raiz(co * co + ca * ca);
}

void viaja(struct mundo *m, int tempo, int h, int d) {
    if(m->herois[h].status == 0)
        return;
    int distancia = disCartesiana(m->bases[m->herois[h].base].local.x, m->bases[m->herois[h].base].local.y, m->bases[d].local.x, m->bases[d].local.y);
    int duracao = distancia / m->herois[h].vel;
    printf("%6d: VIAJA  HEROI %2d BASE %d BASE %d DIST %d VEL %d CHEGA %d\n", tempo, h, m->herois[h].base, d, distancia, m->herois[h].vel, tempo + duracao);
    m->herois[h].base = -1;
    // Aloca memória para args
    struct args *args = malloc(sizeof(struct args));
    if (!args) {
        printf("Erro ao alocar memória para args\n");
        return;
    }

    // Preenche os campos de args
    args->a = m->herois[h].id;
    args->b = d;

    // Insere o evento CHEGA na lista de eventos futuros
    fprio_insere(m->lef, args, CHEGA, tempo + duracao);
}

void morre(struct mundo *m, int tempo, int h, int b) {
    m->herois[h].status = 0;
    (m->mortos)++;
    // Retira o herói da base
    cjto_retira(&m->bases[b].h_ids, h);
    // Aloca memória para args
    struct args *args = malloc(sizeof(struct args));
    if (!args) {
        printf("Erro ao alocar memória para args\n");
        return;
    }

    // Preenche os campos de args
    args->a = m->bases[b].id;
    args->b = 0;

    // Insere o evento AVISA na lista de eventos futuros
    fprio_insere(m->lef, args, AVISA, tempo);
}



void missao(struct mundo *m, int tempo, int missao) {
    (m->missoes[missao].tentativas)++; /** Incrementa o número de tentativas da missão */
    printf("%6d: MISSAO %d TENT %d HAB REQ: [ ", tempo, missao, m->missoes[missao].tentativas);
    cjto_imprime(&m->missoes[missao].habs); /** Imprime as habilidades requeridas pela missão */
    printf(" ]\n");
    char possivel = 0; /** Flag para indicar se a missão pode ser cumprida */

    for (int i = 0; i < m->numBases; i++) {
        struct cjto_t *bHabs = cjto_cria(NUMHABS); /** Cria um conjunto para armazenar as habilidades dos heróis da base */

        printf("%6d: ids dos Herois da base %2d : ", tempo, i);
        cjto_imprime(&m->bases[m->missoes[missao].bases[i]].h_ids);
        printf("\n");

        for (int j = 0; j < m->numHerois; j++) {
            if (cjto_pertence(&m->bases[m->missoes[missao].bases[i]].h_ids, j)) {
                *bHabs = *cjto_uniao(&m->herois[j].habs, bHabs); /** Atualiza o conjunto bHabs com as habilidades do herói */
            }
        }

        if (cjto_contem(bHabs, &m->missoes[missao].habs)) { /** Verifica se as habilidades dos heróis cobrem as requeridas */
            (m->bases[i].qntMissoes)++; /** Incrementa o número de missões da base */
            (m->missoesCump)++; /** Incrementa o total de missões cumpridas */
            printf("%6d: MISSAO %d CUMPRIDA BASE %d HABS: [ ", tempo, missao, m->missoes[missao].bases[i]);
            cjto_imprime(bHabs);
            printf("]\n");

            for (int j = 0; j < m->numHerois; j++) {
                if (cjto_pertence(&m->bases[m->missoes[missao].bases[i]].h_ids, j)) {
                    int risco = m->missoes[missao].perigo / (m->herois[j].pacie + m->herois[j].xp + 1.0); /** Calcula o risco */
                    if (risco > rand() % 31) { /** Verifica se o herói morre */
                        struct args *args = malloc(sizeof(struct args)); /** Aloca memória para o evento MORRE */
                        if (!args) {
                            printf("Erro em alocar memória para args\n");
                            cjto_destroi(bHabs);
                            return;
                        }
                        args->a = j; /** Define o ID do herói */
                        args->b = m->missoes[missao].bases[i];
                        printf("%6d: MORRE HEROI %2d DA BASE %2d MISSAO %d\n", tempo, j, m->missoes[missao].bases[i], missao);
                        fprio_insere(m->lef, args, MORRE, tempo); /** Insere o evento MORRE na LEF */
                    } else {
                        m->herois[j].xp++; /** Incrementa a experiência do herói */
                    }
                }
            }
            possivel = 1; /** Marca que a missão foi possível */
            break; /** Sai do loop após encontrar a primeira base apta */
        }
        cjto_destroi(bHabs); /** Destroi o conjunto de habilidades */
    }

    if (!possivel) {
        printf("%6d: MISSAO %d IMPOSSIVEL\n", tempo, missao);
        struct args *args = malloc(sizeof(struct args)); /** Aloca memória para reagendar a missão */
        if (!args) {
            printf("Erro em alocar memória para args\n");
            return;
        }
        args->a = m->missoes[missao].id; /** Define o ID da missão */
        args->b = 0;
        fprio_insere(m->lef, args, MISSAO, tempo + 1440); /** Reagenda a missão para daqui a 24 horas */
    }
}
void cria_evento(int a, int b, struct fprio_t *lef, int tipo, int tempo) {
    struct args *args = malloc(sizeof(struct args));
    if (!args) {
        printf("Erro ao alocar memória para args\n");
        return;
    }
    args->a = a;
    args->b = b;
    fprio_insere(lef, args, tipo, tempo);
}

char fim(struct mundo *m, int tempo) {
    printf("%6d: FIM\n", tempo);

    /* Imprimindo status dos herois */
    for(int i = 0; i < m->numHerois; i++){
        printf("HEROI %2d ", i);
        if(m->herois[i].status)
            printf("VIVO ");
        else
            printf("MORTO ");
        printf("PAC %3d VEL %4d EXP %4d HABS [ ", m->herois[i].pacie, m->herois[i].vel, m->herois[i].xp);
        cjto_imprime(&m->herois[i].habs);
        printf(" ]\n");
    }
    for(int i = 0; i < m->numBases; i++){
        printf("BASE %2d LOT %2d FILA MAX %2d MISSOES %d\n", i, m->bases[i].cap, m->bases[i].numMaxFila, m->bases[i].qntMissoes);
    }

    printf("EVENTOS TRATADOS: %d\n", m->qntEventos);
    printf("MISSOES CUMPRIDAS: %d/%d (%.1f%%)\n", m->missoesCump, m->numMissoes, (double)m->missoesCump / m->numMissoes * 100);
    
    int minTentativas, maxTentativas;
    double mediaTentativas;
    minTentativas = m->missoes[0].tentativas;
    mediaTentativas = 0;
    maxTentativas = 0;
    for(int i = 0; i < m->numMissoes; i++){
        mediaTentativas += m->missoes[i].tentativas;
        if (m->missoes[i].tentativas > maxTentativas) 
            maxTentativas = m->missoes[i].tentativas;
        
        if (m->missoes[i].tentativas < minTentativas) 
            minTentativas = m->missoes[i].tentativas;
    }
    mediaTentativas /= m->numMissoes;
    printf("TENTATIVAS/MISSAO: MIN %d, MAX %d, MEDIA %.1f\n", minTentativas, maxTentativas, mediaTentativas);

    printf("MORTOS: %2d TOTAL: %2d\n",m->mortos, m->numHerois);
    printf("TAXA MORTALIDADE: %.1f%%\n", ((double)m->mortos / m->numHerois) * 100);
    printf("\n");
    return 1;
}
