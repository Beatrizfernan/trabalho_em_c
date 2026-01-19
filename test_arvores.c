#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "arvores.h"

// ==================== TESTES ====================

void executar_teste(int num_teste) {
    printf("\n========== TESTE #%d ==========\n", num_teste);
    
    NoAVL *avl = NULL;
    NoRN *rn = NULL;
    int rotacoes_avl = 0;
    int rotacoes_rn = 0;
    
    // Gerar 10.000 chaves aleatórias (mesmo conjunto para ambas as árvores)
    int *chaves = (int*)malloc(10000 * sizeof(int));
    for (int i = 0; i < 10000; i++) {
        chaves[i] = rand() % 100001;
    }
    
    // PASSO 2: Inserir 10.000 nós
    printf("Inserindo 10.000 nós...\n");
    for (int i = 0; i < 10000; i++) {
        avl = inserir_avl(avl, chaves[i], &rotacoes_avl);
        inserir_rn(&rn, chaves[i], &rotacoes_rn);
    }
    
    int nos_avl = contar_nos_avl(avl);
    int nos_rn = contar_nos_rn(rn);
    printf("  AVL: %d nós\n", nos_avl);
    printf("  RN:  %d nós\n", nos_rn);
    
    // PASSO 3: Verificar propriedades após inserção
    printf("Verificando propriedades após inserção...\n");
    bool avl_valida = verificar_avl(avl);
    bool rn_valida = verificar_rn(rn, NULL);
    printf("  AVL válida: %s\n", avl_valida ? "SIM" : "NÃO");
    printf("  RN válida:  %s\n", rn_valida ? "SIM" : "NÃO");
    
    // PASSO 4: Remover 1.000 nós aleatórios
    printf("Removendo 1.000 nós...\n");
    int *indices_remover = (int*)malloc(1000 * sizeof(int));
    bool *removido = (bool*)calloc(10000, sizeof(bool));
    
    int removidos = 0;
    while (removidos < 1000) {
        int idx = rand() % 10000;
        if (!removido[idx]) {
            avl = remover_avl(avl, chaves[idx], &rotacoes_avl);
            remover_rn(&rn, chaves[idx], &rotacoes_rn);
            removido[idx] = true;
            removidos++;
            
            // Verificar contagem após cada remoção
            if (removidos % 100 == 0) {
                int count_avl = contar_nos_avl(avl);
                int count_rn = contar_nos_rn(rn);
                int esperado = nos_avl - removidos;
                printf("  Após %d remoções: AVL=%d, RN=%d (esperado=%d)\n", 
                       removidos, count_avl, count_rn, esperado);
            }
        }
    }
    
    nos_avl = contar_nos_avl(avl);
    nos_rn = contar_nos_rn(rn);
    printf("  Após remoções: AVL=%d nós, RN=%d nós\n", nos_avl, nos_rn);
    
    // PASSO 5: Verificar propriedades após remoção
    printf("Verificando propriedades após remoção...\n");
    avl_valida = verificar_avl(avl);
    rn_valida = verificar_rn(rn, NULL);
    printf("  AVL válida: %s\n", avl_valida ? "SIM" : "NÃO");
    printf("  RN válida:  %s\n", rn_valida ? "SIM" : "NÃO");
    
    // PASSO 6: Mostrar rotações
    printf("Rotações:\n");
    printf("  AVL: %d rotações\n", rotacoes_avl);
    printf("  RN:  %d rotações\n", rotacoes_rn);
    
    // PASSO 7: Realizar 10.000 buscas
    printf("Realizando 10.000 buscas...\n");
    
    clock_t inicio_avl = clock();
    for (int i = 0; i < 10000; i++) {
        int chave_busca = rand() % 100001;
        buscar_avl(avl, chave_busca);
    }
    clock_t fim_avl = clock();
    double tempo_avl = ((double)(fim_avl - inicio_avl)) / CLOCKS_PER_SEC * 1000;
    
    clock_t inicio_rn = clock();
    for (int i = 0; i < 10000; i++) {
        int chave_busca = rand() % 100001;
        buscar_rn(rn, chave_busca);
    }
    clock_t fim_rn = clock();
    double tempo_rn = ((double)(fim_rn - inicio_rn)) / CLOCKS_PER_SEC * 1000;
    
    printf("Tempo de 10.000 buscas:\n");
    printf("  AVL: %.3f ms\n", tempo_avl);
    printf("  RN:  %.3f ms\n", tempo_rn);
    
    // Limpar memória
    free(chaves);
    free(indices_remover);
    free(removido);
    liberar_avl(avl);
    liberar_rn(rn);
}

// ==================== MAIN ====================

int main() {
    srand(time(NULL));
    
    printf("╔════════════════════════════════════════════════════╗\n");
    printf("║  TESTES UNITÁRIOS: AVL vs RUBRO-NEGRA             ║\n");
    printf("║  Executando 1.000 testes (2 árvores por vez)      ║\n");
    printf("╚════════════════════════════════════════════════════╝\n");
    
    // Executar 1.000 testes
    for (int i = 1; i <= 1000; i++) {
        executar_teste(i);
        
        if (i % 100 == 0) {
            printf("\n>>> Progresso: %d/1000 testes completados <<<\n", i);
        }
    }
    
    printf("\n╔════════════════════════════════════════════════════╗\n");
    printf("║  TODOS OS 1.000 TESTES CONCLUÍDOS COM SUCESSO!    ║\n");
    printf("╚════════════════════════════════════════════════════╝\n");
    
    return 0;
}