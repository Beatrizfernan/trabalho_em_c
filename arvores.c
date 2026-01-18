#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

// ==================== ÁRVORE AVL ====================

typedef struct NoAVL {
    int chave;
    int altura;
    struct NoAVL *esq, *dir;
} NoAVL;

int rotacoes_avl = 0;

int max(int a, int b) { return (a > b) ? a : b; }
int altura_avl(NoAVL *n) { return n ? n->altura : 0; }
int fator_balanceamento(NoAVL *n) { return n ? altura_avl(n->esq) - altura_avl(n->dir) : 0; }

NoAVL* novo_no_avl(int chave) {
    NoAVL *n = (NoAVL*)malloc(sizeof(NoAVL));
    n->chave = chave;
    n->esq = n->dir = NULL;
    n->altura = 1;
    return n;
}

NoAVL* rotacao_direita(NoAVL *y) {
    rotacoes_avl++;
    NoAVL *x = y->esq;
    NoAVL *T2 = x->dir;
    x->dir = y;
    y->esq = T2;
    y->altura = max(altura_avl(y->esq), altura_avl(y->dir)) + 1;
    x->altura = max(altura_avl(x->esq), altura_avl(x->dir)) + 1;
    return x;
}

NoAVL* rotacao_esquerda(NoAVL *x) {
    rotacoes_avl++;
    NoAVL *y = x->dir;
    NoAVL *T2 = y->esq;
    y->esq = x;
    x->dir = T2;
    x->altura = max(altura_avl(x->esq), altura_avl(x->dir)) + 1;
    y->altura = max(altura_avl(y->esq), altura_avl(y->dir)) + 1;
    return y;
}

NoAVL* inserir_avl(NoAVL *raiz, int chave) {
    if (!raiz) return novo_no_avl(chave);
    
    if (chave < raiz->chave)
        raiz->esq = inserir_avl(raiz->esq, chave);
    else if (chave > raiz->chave)
        raiz->dir = inserir_avl(raiz->dir, chave);
    else
        return raiz;
    
    raiz->altura = 1 + max(altura_avl(raiz->esq), altura_avl(raiz->dir));
    int fb = fator_balanceamento(raiz);
    
    if (fb > 1 && chave < raiz->esq->chave)
        return rotacao_direita(raiz);
    if (fb < -1 && chave > raiz->dir->chave)
        return rotacao_esquerda(raiz);
    if (fb > 1 && chave > raiz->esq->chave) {
        raiz->esq = rotacao_esquerda(raiz->esq);
        return rotacao_direita(raiz);
    }
    if (fb < -1 && chave < raiz->dir->chave) {
        raiz->dir = rotacao_direita(raiz->dir);
        return rotacao_esquerda(raiz);
    }
    return raiz;
}

NoAVL* min_valor_no_avl(NoAVL *n) {
    NoAVL *atual = n;
    while (atual->esq) atual = atual->esq;
    return atual;
}

NoAVL* remover_avl(NoAVL *raiz, int chave) {
    if (!raiz) return raiz;
    
    if (chave < raiz->chave)
        raiz->esq = remover_avl(raiz->esq, chave);
    else if (chave > raiz->chave)
        raiz->dir = remover_avl(raiz->dir, chave);
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
            raiz->dir = remover_avl(raiz->dir, temp->chave);
        }
    }
    
    if (!raiz) return raiz;
    
    raiz->altura = 1 + max(altura_avl(raiz->esq), altura_avl(raiz->dir));
    int fb = fator_balanceamento(raiz);
    
    if (fb > 1 && fator_balanceamento(raiz->esq) >= 0)
        return rotacao_direita(raiz);
    if (fb > 1 && fator_balanceamento(raiz->esq) < 0) {
        raiz->esq = rotacao_esquerda(raiz->esq);
        return rotacao_direita(raiz);
    }
    if (fb < -1 && fator_balanceamento(raiz->dir) <= 0)
        return rotacao_esquerda(raiz);
    if (fb < -1 && fator_balanceamento(raiz->dir) > 0) {
        raiz->dir = rotacao_direita(raiz->dir);
        return rotacao_esquerda(raiz);
    }
    return raiz;
}

int contar_nos_avl(NoAVL *raiz) {
    if (!raiz) return 0;
    return 1 + contar_nos_avl(raiz->esq) + contar_nos_avl(raiz->dir);
}

void gerar_json_avl(NoAVL *raiz) {
    if (!raiz) {
        printf("null");
        return;
    }
    printf("{\"v\":%d,\"l\":", raiz->chave);
    gerar_json_avl(raiz->esq);
    printf(",\"r\":");
    gerar_json_avl(raiz->dir);
    printf("}");
}

void liberar_avl(NoAVL *raiz) {
    if (raiz) {
        liberar_avl(raiz->esq);
        liberar_avl(raiz->dir);
        free(raiz);
    }
}

// ==================== ÁRVORE RUBRO-NEGRA ====================

typedef enum { VERMELHO, PRETO } Cor;

typedef struct NoRN {
    int chave;
    Cor cor;
    struct NoRN *esq, *dir, *pai;
} NoRN;

int rotacoes_rn = 0;

NoRN* novo_no_rn(int chave) {
    NoRN *n = (NoRN*)malloc(sizeof(NoRN));
    n->chave = chave;
    n->cor = VERMELHO;
    n->esq = n->dir = n->pai = NULL;
    return n;
}

void rotacao_esquerda_rn(NoRN **raiz, NoRN *x) {
    rotacoes_rn++;
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

void rotacao_direita_rn(NoRN **raiz, NoRN *y) {
    rotacoes_rn++;
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

void corrigir_insercao_rn(NoRN **raiz, NoRN *z) {
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
                    rotacao_esquerda_rn(raiz, z);
                }
                z->pai->cor = PRETO;
                z->pai->pai->cor = VERMELHO;
                rotacao_direita_rn(raiz, z->pai->pai);
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
                    rotacao_direita_rn(raiz, z);
                }
                z->pai->cor = PRETO;
                z->pai->pai->cor = VERMELHO;
                rotacao_esquerda_rn(raiz, z->pai->pai);
            }
        }
    }
    (*raiz)->cor = PRETO;
}

void inserir_rn(NoRN **raiz, int chave) {
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
    
    corrigir_insercao_rn(raiz, z);
}

NoRN* min_valor_no_rn(NoRN *n) {
    while (n->esq) n = n->esq;
    return n;
}

void corrigir_remocao_rn(NoRN **raiz, NoRN *x, NoRN *pai_x) {
    while (x != *raiz && (!x || x->cor == PRETO)) {
        if (!pai_x) break;
        
        if (x == pai_x->esq) {
            NoRN *w = pai_x->dir;
            if (w && w->cor == VERMELHO) {
                w->cor = PRETO;
                pai_x->cor = VERMELHO;
                rotacao_esquerda_rn(raiz, pai_x);
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
                    rotacao_direita_rn(raiz, w);
                    w = pai_x->dir;
                }
                if (w) {
                    w->cor = pai_x->cor;
                    pai_x->cor = PRETO;
                    if (w->dir) w->dir->cor = PRETO;
                    rotacao_esquerda_rn(raiz, pai_x);
                }
                x = *raiz;
            }
        } else {
            NoRN *w = pai_x->esq;
            if (w && w->cor == VERMELHO) {
                w->cor = PRETO;
                pai_x->cor = VERMELHO;
                rotacao_direita_rn(raiz, pai_x);
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
                    rotacao_esquerda_rn(raiz, w);
                    w = pai_x->esq;
                }
                if (w) {
                    w->cor = pai_x->cor;
                    pai_x->cor = PRETO;
                    if (w->esq) w->esq->cor = PRETO;
                    rotacao_direita_rn(raiz, pai_x);
                }
                x = *raiz;
            }
        }
    }
    if (x) x->cor = PRETO;
}

void remover_rn(NoRN **raiz, int chave) {
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
        corrigir_remocao_rn(raiz, x, pai_x);
}

int contar_nos_rn(NoRN *raiz) {
    if (!raiz) return 0;
    return 1 + contar_nos_rn(raiz->esq) + contar_nos_rn(raiz->dir);
}

void gerar_json_rn(NoRN *raiz) {
    if (!raiz) {
        printf("null");
        return;
    }
    printf("{\"v\":%d,\"c\":\"%s\",\"l\":", raiz->chave, raiz->cor == VERMELHO ? "r" : "b");
    gerar_json_rn(raiz->esq);
    printf(",\"r\":");
    gerar_json_rn(raiz->dir);
    printf("}");
}

void liberar_rn(NoRN *raiz) {
    if (raiz) {
        liberar_rn(raiz->esq);
        liberar_rn(raiz->dir);
        free(raiz);
    }
}

// ==================== PERSISTÊNCIA CORRIGIDA ====================

// Salva apenas a SEQUÊNCIA de inserções
void salvar_sequencia(int *sequencia, int tamanho) {
    FILE *f = fopen("/tmp/sequencia_insercoes.dat", "w");
    if (f) {
        fprintf(f, "%d\n", tamanho);
        for (int i = 0; i < tamanho; i++) {
            fprintf(f, "%d ", sequencia[i]);
        }
        fclose(f);
    }
    
    f = fopen("/tmp/rotacoes.dat", "w");
    if (f) {
        fprintf(f, "%d %d", rotacoes_avl, rotacoes_rn);
        fclose(f);
    }
}

// Carrega e reconstrói as árvores na mesma ordem
int* carregar_sequencia(int *tamanho) {
    FILE *f = fopen("/tmp/sequencia_insercoes.dat", "r");
    if (!f) {
        *tamanho = 0;
        return NULL;
    }
    
    fscanf(f, "%d", tamanho);
    if (*tamanho <= 0) {
        fclose(f);
        *tamanho = 0;
        return NULL;
    }
    
    int *sequencia = (int*)malloc((*tamanho) * sizeof(int));
    for (int i = 0; i < *tamanho; i++) {
        fscanf(f, "%d", &sequencia[i]);
    }
    fclose(f);
    
    f = fopen("/tmp/rotacoes.dat", "r");
    if (f) {
        fscanf(f, "%d %d", &rotacoes_avl, &rotacoes_rn);
        fclose(f);
    }
    
    return sequencia;
}

// ==================== GERADOR HTML ====================

void gerar_html(NoAVL *avl, NoRN *rn) {
    printf("Content-Type: text/html\n\n");
    printf("<!DOCTYPE html><html><head><meta charset='UTF-8'>");
    printf("<title>Árvores AVL e RN</title><style>");
    printf("*{margin:0;padding:0;box-sizing:border-box}");
    printf("body{font-family:Arial;background:linear-gradient(135deg,#667eea,#764ba2);padding:20px}");
    printf(".container{max-width:1400px;margin:0 auto;background:#fff;border-radius:20px;padding:30px;box-shadow:0 20px 60px rgba(0,0,0,0.3)}");
    printf("h1{text-align:center;color:#667eea;margin-bottom:30px}");
    printf(".controls{display:flex;gap:15px;margin-bottom:30px;flex-wrap:wrap;justify-content:center}");
    printf("input{padding:12px 20px;border:2px solid #667eea;border-radius:10px;font-size:16px;width:150px}");
    printf("button{padding:12px 30px;background:#667eea;color:#fff;border:none;border-radius:10px;font-size:16px;cursor:pointer}");
    printf("button:hover{background:#764ba2}");
    printf(".trees{display:grid;grid-template-columns:1fr 1fr;gap:30px;margin-top:30px}");
    printf(".tree{background:#f8f9fa;padding:20px;border-radius:15px}");
    printf(".tree h2{text-align:center;margin-bottom:15px}");
    printf("canvas{width:100%%;height:500px;background:#fff;border-radius:10px}");
    printf(".stats{display:grid;grid-template-columns:repeat(4,1fr);gap:20px;margin-top:30px}");
    printf(".stat{background:#667eea;padding:20px;border-radius:15px;color:#fff;text-align:center}");
    printf(".stat h3{font-size:1em;margin-bottom:10px}.stat .val{font-size:2em;font-weight:bold}");
    printf("</style></head><body><div class='container'>");
    printf("<h1>🌳 Árvores AVL e Rubro-Negra</h1>");
    printf("<form class='controls' method='GET'>");
    printf("<input type='number' name='valor' placeholder='Valor (0-100000)' min='0' max='100000'>");
    printf("<button type='submit' name='acao' value='inserir'>➕ Inserir</button>");
    printf("<button type='submit' name='acao' value='remover'>➖ Remover</button>");
    printf("<button type='submit' name='acao' value='aleatorio'>🎲 10 Aleatórios</button>");
    printf("<button type='submit' name='acao' value='limpar'>🗑️ Limpar</button>");
    printf("</form><div class='trees'>");
    printf("<div class='tree'><h2>Árvore AVL</h2><canvas id='avl'></canvas></div>");
    printf("<div class='tree'><h2>Árvore Rubro-Negra</h2><canvas id='rn'></canvas></div>");
    printf("</div><div class='stats'>");
    printf("<div class='stat'><h3>Rotações AVL</h3><div class='val'>%d</div></div>", rotacoes_avl);
    printf("<div class='stat'><h3>Rotações RN</h3><div class='val'>%d</div></div>", rotacoes_rn);
    printf("<div class='stat'><h3>Nós AVL</h3><div class='val'>%d</div></div>", contar_nos_avl(avl));
    printf("<div class='stat'><h3>Nós RN</h3><div class='val'>%d</div></div>", contar_nos_rn(rn));
    printf("</div></div><script>");
    printf("const avlData="); gerar_json_avl(avl); printf(";");
    printf("const rnData="); gerar_json_rn(rn); printf(";");
    printf("function draw(c,d,isRN){const ctx=c.getContext('2d');ctx.clearRect(0,0,c.width,c.height);");
    printf("if(!d){ctx.font='20px Arial';ctx.fillStyle='#999';ctx.textAlign='center';");
    printf("ctx.fillText('Árvore vazia',c.width/2,c.height/2);return}");
    printf("function dr(n,x,y,o){if(!n)return;");
    printf("if(n.l){ctx.strokeStyle='#999';ctx.lineWidth=2;ctx.beginPath();");
    printf("ctx.moveTo(x,y);ctx.lineTo(x-o,y+80);ctx.stroke();dr(n.l,x-o,y+80,o/2)}");
    printf("if(n.r){ctx.strokeStyle='#999';ctx.lineWidth=2;ctx.beginPath();");
    printf("ctx.moveTo(x,y);ctx.lineTo(x+o,y+80);ctx.stroke();dr(n.r,x+o,y+80,o/2)}");
    printf("ctx.fillStyle=isRN&&n.c=='r'?'#f5576c':'#667eea';");
    printf("ctx.beginPath();ctx.arc(x,y,25,0,2*Math.PI);ctx.fill();");
    printf("ctx.strokeStyle='#fff';ctx.lineWidth=3;ctx.stroke();");
    printf("ctx.fillStyle='#fff';ctx.font='bold 14px Arial';ctx.textAlign='center';");
    printf("ctx.textBaseline='middle';ctx.fillText(n.v,x,y)}dr(d,c.width/2,40,c.width/4)}");
    printf("const c1=document.getElementById('avl');c1.width=c1.offsetWidth;c1.height=500;");
    printf("const c2=document.getElementById('rn');c2.width=c2.offsetWidth;c2.height=500;");
    printf("draw(c1,avlData,false);draw(c2,rnData,true);");
    printf("</script></body></html>");
}

// ==================== MAIN ====================

#define MAX_SEQUENCIA 10000

int main() {
    NoAVL *avl = NULL;
    NoRN *rn = NULL;
    
    // Carrega sequência anterior
    int tamanho = 0;
    int *sequencia = carregar_sequencia(&tamanho);
    
    // Array para manter sequência atualizada
    int sequencia_atual[MAX_SEQUENCIA];
    int tamanho_atual = 0;
    
    // Reconstrói as árvores na ordem correta
    if (sequencia) {
        for (int i = 0; i < tamanho; i++) {
            avl = inserir_avl(avl, sequencia[i]);
            inserir_rn(&rn, sequencia[i]);
            sequencia_atual[tamanho_atual++] = sequencia[i];
        }
        free(sequencia);
    }
    
    char *query = getenv("QUERY_STRING");
    
    if (query) {
        char acao[50] = "", valor_str[50] = "";
        char *token = strtok(query, "&");
        
        while (token) {
            if (strncmp(token, "acao=", 5) == 0) {
                strcpy(acao, token + 5);
            } else if (strncmp(token, "valor=", 6) == 0) {
                strcpy(valor_str, token + 6);
            }
            token = strtok(NULL, "&");
        }
        
        if (strcmp(acao, "inserir") == 0 && strlen(valor_str) > 0) {
            int valor = atoi(valor_str);
            avl = inserir_avl(avl, valor);
            inserir_rn(&rn, valor);
            if (tamanho_atual < MAX_SEQUENCIA) {
                sequencia_atual[tamanho_atual++] = valor;
            }
        } else if (strcmp(acao, "remover") == 0 && strlen(valor_str) > 0) {
            int valor = atoi(valor_str);
            avl = remover_avl(avl, valor);
            remover_rn(&rn, valor);
            // Remove da sequência
            for (int i = 0; i < tamanho_atual; i++) {
                if (sequencia_atual[i] == valor) {
                    for (int j = i; j < tamanho_atual - 1; j++) {
                        sequencia_atual[j] = sequencia_atual[j + 1];
                    }
                    tamanho_atual--;
                    break;
                }
            }
        } else if (strcmp(acao, "aleatorio") == 0) {
            srand(time(NULL));
            for (int i = 0; i < 10 && tamanho_atual < MAX_SEQUENCIA; i++) {
                int valor = rand() % 100001;
                avl = inserir_avl(avl, valor);
                inserir_rn(&rn, valor);
                sequencia_atual[tamanho_atual++] = valor;
            }
        } else if (strcmp(acao, "limpar") == 0) {
            liberar_avl(avl);
            liberar_rn(rn);
            avl = NULL;
            rn = NULL;
            tamanho_atual = 0;
            rotacoes_avl = 0;
            rotacoes_rn = 0;
        }
    }
    
    // Salva sequência atualizada
    salvar_sequencia(sequencia_atual, tamanho_atual);
    
    gerar_html(avl, rn);
    
    liberar_avl(avl);
    liberar_rn(rn);
    
    return 0;
}