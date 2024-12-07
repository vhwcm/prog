#ifndef THEBOYS
#define THEBOYS

#include "fprio.h"
#include "fila.h"
#include "conjunto.h"

/* Definições de constantes */
#define NADA -1

// Definições de eventos
#define ESPERA 0
#define DESISTE 1
#define AVISA 2
#define VIAJA 3
#define ENTRA 4
#define SAI 5
#define MISSAO 6
#define MORRE 7
#define CHEGA 8
#define FIM 9

// Definições de parâmetros do mundo
#define NUMHABS 10
#define T_INICIO 0
#define T_FIM_DO_MUNDO 5256
#define N_TAMANHO_MUNDO 20000
#define N_HABILIDADES 10
#define N_HEROIS 50
#define N_BASES  (N_HEROIS / 5)
#define N_MISSOES (T_FIM_DO_MUNDO / 100)

struct local
{
    int x;
    int y;
};

struct base
{
    int id;
    int cap;
    struct cjto_t h_ids;
    struct fila_t fila;
    struct local local;
    int numMaxFila;
    int qntMissoes;
};


struct heroi
{
    unsigned char status;
    int id;
    struct cjto_t habs;
    int pacie;
    int vel;
    int xp;
    int base;
};

struct missao
{
    int tentativas;
    int id;
    struct cjto_t habs;
    int perigo;
    struct local local;
    int* bases; // todas as bases ordenadas por distancia da missão

};

struct mundo {
    int qntEventos;
    int missoesCump;
    int mortos;
    int numHerois;
    struct heroi herois[N_HEROIS];
    int numBases;
    struct base bases[N_BASES];
    int numMissoes;
    struct missao missoes[N_MISSOES];
    int numHabs;
    int tamMundo;
    struct fprio_t *lef;
};

struct args
{
    int  a;
    int  b;
};

void cria_evento(int a, int b, struct fprio_t *lef, int tipo, int tempo);

void ordenaBasesPorDistancia(struct mundo *m, int missao);

void chega(struct mundo *m, int tempo, int h, int b);

void espera(struct mundo *m, int tempo, int h, int b);

void desiste(struct mundo *m, int tempo, int h, int b);

int raiz(int n);

void avisa(struct mundo *m, int tempo, int b);

void entra(struct mundo *m, int tempo, int h, int b);

void sai(struct mundo *m, int tempo, int h, int b);

int disCartesiana(int x1, int y1, int x2, int y2);

void viaja(struct mundo *m, int tempo, int h, int d);

void morre(struct mundo *m, int tempo, int h, int b);

void missao(struct mundo *m, int tempo, int missao);

char fim(struct mundo *m, int tempo);

#endif

