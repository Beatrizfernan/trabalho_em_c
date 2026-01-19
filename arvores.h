#ifndef ARVORES_H
#define ARVORES_H

#include <stdbool.h>

// ==================== ÁRVORE AVL ====================

typedef struct NoAVL {
    int chave;
    int altura;
    struct NoAVL *esq, *dir;
} NoAVL;

// Funções AVL
NoAVL* novo_no_avl(int chave);
NoAVL* inserir_avl(NoAVL *raiz, int chave, int *rot_count);
NoAVL* remover_avl(NoAVL *raiz, int chave, int *rot_count);
bool buscar_avl(NoAVL *raiz, int chave);
int contar_nos_avl(NoAVL *raiz);
bool verificar_avl(NoAVL *raiz);
void liberar_avl(NoAVL *raiz);
int altura_avl(NoAVL *n);
int fator_balanceamento(NoAVL *n);

// ==================== ÁRVORE RUBRO-NEGRA ====================

typedef enum { VERMELHO, PRETO } Cor;

typedef struct NoRN {
    int chave;
    Cor cor;
    struct NoRN *esq, *dir, *pai;
} NoRN;

// Funções RN
NoRN* novo_no_rn(int chave);
void inserir_rn(NoRN **raiz, int chave, int *rot_count);
void remover_rn(NoRN **raiz, int chave, int *rot_count);
bool buscar_rn(NoRN *raiz, int chave);
int contar_nos_rn(NoRN *raiz);
bool verificar_rn(NoRN *raiz, NoRN *pai);
void liberar_rn(NoRN *raiz);

#endif // ARVORES_H