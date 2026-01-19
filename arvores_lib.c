#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "arvores.h"

// ==================== FUNÇÕES AUXILIARES ====================

int max(int a, int b) { return (a > b) ? a : b; }

// ==================== ÁRVORE AVL ====================

int altura_avl(NoAVL *n) { 
    return n ? n->altura : 0; 
}

int fator_balanceamento(NoAVL *n) { 
    return n ? altura_avl(n->esq) - altura_avl(n->dir) : 0; 
}

NoAVL* novo_no_avl(int chave) {
    NoAVL *n = (NoAVL*)malloc(sizeof(NoAVL));
    n->chave = chave;
    n->esq = n->dir = NULL;
    n->altura = 1;
    return n;
}

NoAVL* rotacao_direita(NoAVL *y, int *rot_count) {
    if (rot_count) (*rot_count)++;
    NoAVL *x = y->esq;
    NoAVL *T2 = x->dir;
    x->dir = y;
    y->esq = T2;
    y->altura = max(altura_avl(y->esq), altura_avl(y->dir)) + 1;
    x->altura = max(altura_avl(x->esq), altura_avl(x->dir)) + 1;
    return x;
}

NoAVL* rotacao_esquerda(NoAVL *x, int *rot_count) {
    if (rot_count) (*rot_count)++;
    NoAVL *y = x->dir;
    NoAVL *T2 = y->esq;
    y->esq = x;
    x->dir = T2;
    x->altura = max(altura_avl(x->esq), altura_avl(x->dir)) + 1;
    y->altura = max(altura_avl(y->esq), altura_avl(y->dir)) + 1;
    return y;
}

NoAVL* inserir_avl(NoAVL *raiz, int chave, int *rot_count) {
    if (!raiz) return novo_no_avl(chave);
    
    if (chave < raiz->chave)
        raiz->esq = inserir_avl(raiz->esq, chave, rot_count);
    else if (chave > raiz->chave)
        raiz->dir = inserir_avl(raiz->dir, chave, rot_count);
    else
        return raiz;
    
    raiz->altura = 1 + max(altura_avl(raiz->esq), altura_avl(raiz->dir));
    int fb = fator_balanceamento(raiz);
    
    if (fb > 1 && chave < raiz->esq->chave)
        return rotacao_direita(raiz, rot_count);
    if (fb < -1 && chave > raiz->dir->chave)
        return rotacao_esquerda(raiz, rot_count);
    if (fb > 1 && chave > raiz->esq->chave) {
        raiz->esq = rotacao_esquerda(raiz->esq, rot_count);
        return rotacao_direita(raiz, rot_count);
    }
    if (fb < -1 && chave < raiz->dir->chave) {
        raiz->dir = rotacao_direita(raiz->dir, rot_count);
        return rotacao_esquerda(raiz, rot_count);
    }
    return raiz;
}

NoAVL* min_valor_no_avl(NoAVL *n) {
    while (n->esq) n = n->esq;
    return n;
}

NoAVL* remover_avl(NoAVL *raiz, int chave, int *rot_count) {
    if (!raiz) return raiz;
    
    if (chave < raiz->chave)
        raiz->esq = remover_avl(raiz->esq, chave, rot_count);
    else if (chave > raiz->chave)
        raiz->dir = remover_avl(raiz->dir, chave, rot_count);
    else {
        if (!raiz->esq || !raiz->dir) {
            NoAVL *temp = raiz->esq ? raiz->esq : raiz->dir;
            if (!temp) {
                temp = raiz;
                raiz = NULL;
            } else {
                *raiz = *temp;
            }
            free(temp);
        } else {
            NoAVL *temp = min_valor_no_avl(raiz->dir);
            raiz->chave = temp->chave;
            raiz->dir = remover_avl(raiz->dir, temp->chave, rot_count);
        }
    }
    
    if (!raiz) return raiz;
    
    raiz->altura = 1 + max(altura_avl(raiz->esq), altura_avl(raiz->dir));
    int fb = fator_balanceamento(raiz);
    
    if (fb > 1 && fator_balanceamento(raiz->esq) >= 0)
        return rotacao_direita(raiz, rot_count);
    if (fb > 1 && fator_balanceamento(raiz->esq) < 0) {
        raiz->esq = rotacao_esquerda(raiz->esq, rot_count);
        return rotacao_direita(raiz, rot_count);
    }
    if (fb < -1 && fator_balanceamento(raiz->dir) <= 0)
        return rotacao_esquerda(raiz, rot_count);
    if (fb < -1 && fator_balanceamento(raiz->dir) > 0) {
        raiz->dir = rotacao_direita(raiz->dir, rot_count);
        return rotacao_esquerda(raiz, rot_count);
    }
    return raiz;
}

bool buscar_avl(NoAVL *raiz, int chave) {
    if (!raiz) return false;
    if (chave == raiz->chave) return true;
    if (chave < raiz->chave) return buscar_avl(raiz->esq, chave);
    return buscar_avl(raiz->dir, chave);
}

int contar_nos_avl(NoAVL *raiz) {
    if (!raiz) return 0;
    return 1 + contar_nos_avl(raiz->esq) + contar_nos_avl(raiz->dir);
}

bool verificar_avl(NoAVL *raiz) {
    if (!raiz) return true;
    
    int fb = fator_balanceamento(raiz);
    if (fb < -1 || fb > 1) return false;
    
    return verificar_avl(raiz->esq) && verificar_avl(raiz->dir);
}

void liberar_avl(NoAVL *raiz) {
    if (raiz) {
        liberar_avl(raiz->esq);
        liberar_avl(raiz->dir);
        free(raiz);
    }
}

// ==================== ÁRVORE RUBRO-NEGRA ====================

NoRN* novo_no_rn(int chave) {
    NoRN *n = (NoRN*)malloc(sizeof(NoRN));
    n->chave = chave;
    n->cor = VERMELHO;
    n->esq = n->dir = n->pai = NULL;
    return n;
}

void rotacao_esquerda_rn(NoRN **raiz, NoRN *x, int *rot_count) {
    if (rot_count) (*rot_count)++;
    NoRN *y = x->dir;
    x->dir = y->esq;
    if (y->esq) y->esq->pai = x;
    y->pai = x->pai;
    if (!x->pai) *raiz = y;
    else if (x == x->pai->esq) x->pai->esq = y;
    else x->pai->dir = y;
    y->esq = x;
    x->pai = y;
}

void rotacao_direita_rn(NoRN **raiz, NoRN *y, int *rot_count) {
    if (rot_count) (*rot_count)++;
    NoRN *x = y->esq;
    y->esq = x->dir;
    if (x->dir) x->dir->pai = y;
    x->pai = y->pai;
    if (!y->pai) *raiz = x;
    else if (y == y->pai->esq) y->pai->esq = x;
    else y->pai->dir = x;
    x->dir = y;
    y->pai = x;
}

void corrigir_insercao_rn(NoRN **raiz, NoRN *z, int *rot_count) {
    while (z->pai && z->pai->cor == VERMELHO) {
        if (!z->pai->pai) break;
        
        if (z->pai == z->pai->pai->esq) {
            NoRN *y = z->pai->pai->dir;
            
            if (y && y->cor == VERMELHO) {
                z->pai->cor = PRETO;
                y->cor = PRETO;
                z->pai->pai->cor = VERMELHO;
                z = z->pai->pai;
            } else {
                if (z == z->pai->dir) {
                    z = z->pai;
                    rotacao_esquerda_rn(raiz, z, rot_count);
                }
                z->pai->cor = PRETO;
                z->pai->pai->cor = VERMELHO;
                rotacao_direita_rn(raiz, z->pai->pai, rot_count);
            }
        } else {
            NoRN *y = z->pai->pai->esq;
            
            if (y && y->cor == VERMELHO) {
                z->pai->cor = PRETO;
                y->cor = PRETO;
                z->pai->pai->cor = VERMELHO;
                z = z->pai->pai;
            } else {
                if (z == z->pai->esq) {
                    z = z->pai;
                    rotacao_direita_rn(raiz, z, rot_count);
                }
                z->pai->cor = PRETO;
                z->pai->pai->cor = VERMELHO;
                rotacao_esquerda_rn(raiz, z->pai->pai, rot_count);
            }
        }
    }
    (*raiz)->cor = PRETO;
}

void inserir_rn(NoRN **raiz, int chave, int *rot_count) {
    NoRN *z = novo_no_rn(chave);
    NoRN *y = NULL;
    NoRN *x = *raiz;
    
    while (x) {
        y = x;
        if (z->chave < x->chave) x = x->esq;
        else if (z->chave > x->chave) x = x->dir;
        else { free(z); return; }
    }
    
    z->pai = y;
    if (!y) *raiz = z;
    else if (z->chave < y->chave) y->esq = z;
    else y->dir = z;
    
    corrigir_insercao_rn(raiz, z, rot_count);
}

NoRN* min_valor_no_rn(NoRN *n) {
    while (n->esq) n = n->esq;
    return n;
}

void corrigir_remocao_rn(NoRN **raiz, NoRN *x, NoRN *pai_x, int *rot_count) {
    while (x != *raiz && (!x || x->cor == PRETO)) {
        if (!pai_x) break;
        
        if (x == pai_x->esq) {
            NoRN *w = pai_x->dir;
            if (w && w->cor == VERMELHO) {
                w->cor = PRETO;
                pai_x->cor = VERMELHO;
                rotacao_esquerda_rn(raiz, pai_x, rot_count);
                w = pai_x->dir;
            }
            if (w && (!w->esq || w->esq->cor == PRETO) && (!w->dir || w->dir->cor == PRETO)) {
                if (w) w->cor = VERMELHO;
                x = pai_x;
                pai_x = x ? x->pai : NULL;
            } else {
                if (w && (!w->dir || w->dir->cor == PRETO)) {
                    if (w->esq) w->esq->cor = PRETO;
                    w->cor = VERMELHO;
                    rotacao_direita_rn(raiz, w, rot_count);
                    w = pai_x->dir;
                }
                if (w) {
                    w->cor = pai_x->cor;
                    pai_x->cor = PRETO;
                    if (w->dir) w->dir->cor = PRETO;
                    rotacao_esquerda_rn(raiz, pai_x, rot_count);
                }
                x = *raiz;
            }
        } else {
            NoRN *w = pai_x->esq;
            if (w && w->cor == VERMELHO) {
                w->cor = PRETO;
                pai_x->cor = VERMELHO;
                rotacao_direita_rn(raiz, pai_x, rot_count);
                w = pai_x->esq;
            }
            if (w && (!w->dir || w->dir->cor == PRETO) && (!w->esq || w->esq->cor == PRETO)) {
                if (w) w->cor = VERMELHO;
                x = pai_x;
                pai_x = x ? x->pai : NULL;
            } else {
                if (w && (!w->esq || w->esq->cor == PRETO)) {
                    if (w->dir) w->dir->cor = PRETO;
                    if (w) w->cor = VERMELHO;
                    rotacao_esquerda_rn(raiz, w, rot_count);
                    w = pai_x->esq;
                }
                if (w) {
                    w->cor = pai_x->cor;
                    pai_x->cor = PRETO;
                    if (w->esq) w->esq->cor = PRETO;
                    rotacao_direita_rn(raiz, pai_x, rot_count);
                }
                x = *raiz;
            }
        }
    }
    if (x) x->cor = PRETO;
}

void remover_rn(NoRN **raiz, int chave, int *rot_count) {
    NoRN *z = *raiz;
    while (z) {
        if (chave < z->chave) z = z->esq;
        else if (chave > z->chave) z = z->dir;
        else break;
    }
    
    if (!z) return;
    
    NoRN *y = z;
    NoRN *x;
    Cor cor_original = y->cor;
    NoRN *pai_x;
    
    if (!z->esq) {
        x = z->dir;
        pai_x = z->pai;
        if (!z->pai) *raiz = z->dir;
        else if (z == z->pai->esq) z->pai->esq = z->dir;
        else z->pai->dir = z->dir;
        if (z->dir) z->dir->pai = z->pai;
    } else if (!z->dir) {
        x = z->esq;
        pai_x = z->pai;
        if (!z->pai) *raiz = z->esq;
        else if (z == z->pai->esq) z->pai->esq = z->esq;
        else z->pai->dir = z->esq;
        if (z->esq) z->esq->pai = z->pai;
    } else {
        y = min_valor_no_rn(z->dir);
        cor_original = y->cor;
        x = y->dir;
        
        if (y->pai == z) {
            pai_x = y;
            if (x) x->pai = y;
        } else {
            pai_x = y->pai;
            if (y->pai) y->pai->esq = y->dir;
            if (y->dir) y->dir->pai = y->pai;
            y->dir = z->dir;
            y->dir->pai = y;
        }
        
        if (!z->pai) *raiz = y;
        else if (z == z->pai->esq) z->pai->esq = y;
        else z->pai->dir = y;
        y->pai = z->pai;
        y->esq = z->esq;
        y->esq->pai = y;
        y->cor = z->cor;
    }
    
    free(z);
    
    if (cor_original == PRETO)
        corrigir_remocao_rn(raiz, x, pai_x, rot_count);
}

bool buscar_rn(NoRN *raiz, int chave) {
    while (raiz) {
        if (chave == raiz->chave) return true;
        if (chave < raiz->chave) raiz = raiz->esq;
        else raiz = raiz->dir;
    }
    return false;
}

int contar_nos_rn(NoRN *raiz) {
    if (!raiz) return 0;
    return 1 + contar_nos_rn(raiz->esq) + contar_nos_rn(raiz->dir);
}

int altura_negra(NoRN *raiz, bool *valida) {
    if (!raiz) return 1;
    
    int altura_esq = altura_negra(raiz->esq, valida);
    int altura_dir = altura_negra(raiz->dir, valida);
    
    if (altura_esq != altura_dir) *valida = false;
    
    return (raiz->cor == PRETO ? 1 : 0) + altura_esq;
}

bool verificar_rn(NoRN *raiz, NoRN *pai) {
    if (!raiz) return true;
    
    // Propriedade 1: Raiz é preta
    if (!pai && raiz->cor == VERMELHO) return false;
    
    // Propriedade 2: Não pode haver dois vermelhos consecutivos
    if (raiz->cor == VERMELHO && pai && pai->cor == VERMELHO) return false;
    
    // Propriedade 3: Altura negra
    bool valida = true;
    altura_negra(raiz, &valida);
    if (!valida) return false;
    
    return verificar_rn(raiz->esq, raiz) && verificar_rn(raiz->dir, raiz);
}

void liberar_rn(NoRN *raiz) {
    if (raiz) {
        liberar_rn(raiz->esq);
        liberar_rn(raiz->dir);
        free(raiz);
    }
}