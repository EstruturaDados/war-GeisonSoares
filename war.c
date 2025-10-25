#include <stdio.h>
#include <stdlib.h> // calloc, malloc, free, rand
#include <string.h> // strcpy, strcmp
#include <time.h>   // time, srand
#include <stdbool.h> // Uso de bool

// ============================================================================
//         PROJETO WAR ESTRUTURADO - DESAFIO DE CÓDIGO (Nível Mestre Final)
// ============================================================================
// 
// OBJETIVOS:
// - Modularizar completamente o código em funções especializadas.
// - Implementar um sistema de missões para um jogador.
// - Criar uma função para verificar se a missão foi cumprida.
// - Utilizar passagem por referência (ponteiros) para modificar dados e
//   passagem por valor/referência constante (const) para apenas ler.
// - Foco em: Design de software, modularização, const correctness, lógica de jogo.
//
// ============================================================================

// --- Constantes Globais ---
#define MAX_TERRITORIOS 5
#define TAM_MISSAO 100
#define NUM_MISSOES 5

// --- Estrutura de Dados ---
// Define a estrutura para um território, contendo seu nome, a cor do exército que o domina e o número de tropas.
typedef struct {
    char nome[30];
    char cor[10];
    int tropas;
} Territorio;

// --- Protótipos das Funções ---
// Funções de setup e gerenciamento de memória:
Territorio* alocarMapa(int tamanho);
void inicializarTerritorios(Territorio* mapa, int tamanho);
void sortearMissao(char* destino, const char missoes[][TAM_MISSAO], int totalMissoes);
void liberarMemoria(Territorio* mapa, char* missaoJogador);

// Funções de interface com o usuário:
void exibirMenuPrincipal();
void exibirMapa(const Territorio* mapa, int tamanho); // Usa 'const'
void exibirMissao(const char* missaoJogador);

// Funções de lógica principal do jogo:
void faseDeAtaque(Territorio* mapa, int tamanho, const char* corJogador);
void simularAtaque(Territorio* atacante, Territorio* defensor);
bool verificarVitoria(const char* missaoJogador, const Territorio* mapa, int tamanho, const char* corJogador); // Usa 'const'

// Função utilitária:
void limparBufferEntrada();
int rolarDado();

// ----------------------------------------------------------------------------
// --- Implementação das Funções Auxiliares e Utilitárias ---
// ----------------------------------------------------------------------------

// limparBufferEntrada():
void limparBufferEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

// limparTela():
void limparTela() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// rolarDado():
// Simula uma rolagem de dados (valores entre 1 e 6).
int rolarDado() {
    return (rand() % 6) + 1;
}

// ----------------------------------------------------------------------------
// --- Implementação das Funções de Setup e Gerenciamento de Memória ---
// ----------------------------------------------------------------------------

// alocarMapa():
// Aloca dinamicamente a memória para o vetor de territórios usando calloc.
// Retorna um ponteiro para a memória alocada ou NULL em caso de falha.
Territorio* alocarMapa(int tamanho) {
    // Alocação de memória: usa calloc para inicializar com zeros
    Territorio *novo_mapa = (Territorio *)calloc(tamanho, sizeof(Territorio));

    if (novo_mapa == NULL) {
        fprintf(stderr, "[ERRO CRÍTICO] Falha na alocação de memória para o mapa.\n");
    }
    return novo_mapa;
}

// inicializarTerritorios():
// Preenche os dados iniciais de cada território no mapa (nome, cor do exército, número de tropas).
// Esta função modifica o mapa passado por referência (ponteiro).
void inicializarTerritorios(Territorio* mapa, int tamanho) {
    // Define 5 territórios iniciais e distribui 2 cores
    printf("\n--- Preenchendo Territórios Iniciais ---\n");

    for (int i = 0; i < tamanho; i++) {
        printf("Território %d:\n", i + 1);
        printf("  Nome: ");
        if (scanf("%29s", mapa[i].nome) != 1) continue;

        // Distribuição simples de cores e tropas para 2 jogadores (ex: Vermelho/Azul)
        if (i % 2 == 0) {
            strcpy(mapa[i].cor, "Vermelho");
            mapa[i].tropas = 3;
        } else {
            strcpy(mapa[i].cor, "Azul");
            mapa[i].tropas = 2;
        }
    }
    // O último território pode ser neutro ou ter 1 tropa da cor inicial
    if (tamanho > 0) {
        strcpy(mapa[tamanho - 1].cor, "Azul");
        mapa[tamanho - 1].tropas = 4;
    }
    limparBufferEntrada();
    printf("[SETUP] Inicialização do mapa concluída.\n");
}

// sortearMissao():
// Sorteia uma missão e copia para a variável de missão do jogador usando strcpy.
void sortearMissao(char* destino, const char missoes[][TAM_MISSAO], int totalMissoes) {
    int idSorteado = rand() % totalMissoes;
    // Utiliza strcpy para copiar a missão sorteada (índice aleatório) para o destino (missão do jogador)
    strcpy(destino, missoes[idSorteado]);
}

// liberarMemoria():
// Libera a memória previamente alocada para o mapa e missão usando free.
void liberarMemoria(Territorio* mapa, char* missaoJogador) {
    if (mapa != NULL) {
        free(mapa);
    }
    if (missaoJogador != NULL) {
        free(missaoJogador);
    }
    printf("[LIMPEZA] Memória alocada (mapa e missão) liberada com sucesso.\n");
}

// ----------------------------------------------------------------------------
// --- Implementação das Funções de Interface e Exibição ---
// ----------------------------------------------------------------------------

// exibirMenuPrincipal():
void exibirMenuPrincipal() {
    printf("\n==========================================\n");
    printf("MENU DE AÇÕES\n");
    printf("1. Iniciar Fase de Ataque\n");
    printf("2. Verificar Condição de Vitória (Missão)\n");
    printf("3. Exibir Mapa\n");
    printf("0. Sair e Finalizar Jogo\n");
    printf("Escolha: ");
}

// exibirMapa():
// Mostra o estado atual de todos os territórios no mapa, formatado como uma tabela.
// Usa 'const' para garantir que a função apenas leia os dados do mapa, sem modificá-los.
void exibirMapa(const Territorio* mapa, int tamanho) {
    printf("\n================================================\n");
    printf("              MAPA DE TERRITÓRIOS\n");
    printf("================================================\n");
    printf("ID  | Nome                      | Cor        | Tropas\n");
    printf("----------------------------------------------------\n");

    for (int i = 0; i < tamanho; i++) {
        printf("%02d  | %-25s | %-10s | %d\n",
               i, mapa[i].nome, mapa[i].cor, mapa[i].tropas);
    }
    printf("================================================\n");
}

// exibirMissao():
// Exibe a descrição da missão atual do jogador.
void exibirMissao(const char* missaoJogador) {
    printf("\n[MISSÃO SECRETA] Objetivo Atual: %s\n", missaoJogador);
}

// ----------------------------------------------------------------------------
// --- Implementação das Funções de Lógica de Jogo ---
// ----------------------------------------------------------------------------

// simularAtaque():
// Executa a lógica de uma batalha entre dois territórios.
void simularAtaque(Territorio* atacante, Territorio* defensor) {
    int dadoAtacante = rolarDado();
    int dadoDefensor = rolarDado();

    printf("\n--- Simulação: %s (%s) vs %s (%s) ---\n", 
           atacante->nome, atacante->cor, defensor->nome, defensor->cor);
    printf("Dados Rolados: Atacante (%d) | Defensor (%d)\n", dadoAtacante, dadoDefensor);

    if (dadoAtacante > dadoDefensor) {
        // Venceu
        printf("[RESULTADO] ATAQUE BEM SUCEDIDO!\n");
        
        // Atualização de campos: transferir a cor e metade das tropas
        if (atacante->tropas >= 2) {
            int tropasTransferidas = atacante->tropas / 2;
            
            strcpy(defensor->cor, atacante->cor); // Defensor muda de cor
            defensor->tropas = tropasTransferidas; // Defensor fica com tropas do atacante
            atacante->tropas -= tropasTransferidas; 

            printf("  > Território CONQUISTADO! Novo Dono: %s\n", defensor->cor);
            printf("  > %d tropas movidas para ocupação.\n", tropasTransferidas);
        } else {
             printf("[ALERTA] Conquista, mas atacante com tropas insuficientes para ocupar (>2).\n");
        }

    } else {
        // Perdeu ou Empatou
        printf("[RESULTADO] ATAQUE FRACASSADO! Defensor resistiu.\n");

        // Atualização de campos: atacante perde uma tropa.
        if (atacante->tropas > 1) { 
            atacante->tropas -= 1;
            printf("  > Atacante perdeu 1 tropa. Tropas restantes: %d\n", atacante->tropas);
        } else {
            printf("  > Atacante mantém 1 tropa de ocupação.\n");
        }
    }
}

// faseDeAtaque():
// Gerencia a interface para a ação de ataque, solicitando ao jogador os territórios de origem e destino.
void faseDeAtaque(Territorio* mapa, int tamanho, const char* corJogador) {
    int idAtacante, idDefensor;
    
    printf("\n--- FASE DE ATAQUE ---\n");
    
    printf("ID do seu Território Atacante (0 a %d): ", tamanho - 1);
    if (scanf("%d", &idAtacante) != 1 || idAtacante < 0 || idAtacante >= tamanho) {
        printf("[ERRO] ID do atacante inválido.\n"); limparBufferEntrada(); return;
    }
    
    printf("ID do Território Defensor (0 a %d): ", tamanho - 1);
    if (scanf("%d", &idDefensor) != 1 || idDefensor < 0 || idDefensor >= tamanho) {
        printf("[ERRO] ID do defensor inválido.\n"); limparBufferEntrada(); return;
    }

    if (idAtacante == idDefensor) {
        printf("[ALERTA] ID de origem e destino são iguais.\n"); return;
    }

    // Validação de ataques: só pode atacar territórios inimigos
    if (strcmp(mapa[idAtacante].cor, corJogador) != 0) {
        printf("[ERRO] O território atacante não pertence à sua cor (%s).\n", corJogador); return;
    }
    if (strcmp(mapa[idAtacante].cor, mapa[idDefensor].cor) == 0) {
        printf("[ALERTA] Não é possível atacar um território aliado.\n"); return;
    }
    if (mapa[idAtacante].tropas <= 1) {
        printf("[ALERTA] É necessário ter pelo menos 2 tropas no atacante (1 de ocupação + 1 de ataque).\n"); return;
    }

    // Chamada da função de simulação
    simularAtaque(&mapa[idAtacante], &mapa[idDefensor]);
    exibirMapa(mapa, tamanho); // Exibição pós-ataque
}

// verificarVitoria():
// Avalia se a missão do jogador foi cumprida. (Lógica simples inicial)
// Retorna true se a missão foi cumprida, e false caso contrário.
bool verificarVitoria(const char* missaoJogador, const Territorio* mapa, int tamanho, const char* corJogador) {
    // Lógica 1: Conquistar 3 territórios seguidos (simplificado: 3 territórios no total)
    if (strstr(missaoJogador, "Conquistar 3 territórios") != NULL) {
        int count = 0;
        for (int i = 0; i < tamanho; i++) {
            if (strcmp(mapa[i].cor, corJogador) == 0) {
                count++;
            }
        }
        if (count >= 3) return true;
    }
    
    // Lógica 2: Eliminar todas as tropas da cor Vermelha (simulando a cor do inimigo)
    if (strstr(missaoJogador, "Eliminar todas as tropas da cor Vermelha") != NULL) {
        // Encontra a cor inimiga (se for Vermelho, o inimigo é Azul, vice-versa)
        const char* corInimiga = (strcmp(corJogador, "Vermelho") == 0) ? "Azul" : "Vermelho";
        
        // Verifica se ainda existe algum território da cor inimiga no mapa
        for (int i = 0; i < tamanho; i++) {
            if (strcmp(mapa[i].cor, corInimiga) == 0) {
                return false; // Ainda existe inimigo.
            }
        }
        return true; // Todos os territórios inimigos foram eliminados.
    }
    
    return false;
}

// ----------------------------------------------------------------------------
// --- Função Principal (main) ---
// ----------------------------------------------------------------------------

int main() {
    // Vetor de strings contendo missões (Requisito Funcional)
    const char missoes[NUM_MISSOES][TAM_MISSAO] = {
        "Conquistar 3 territórios seguidos e fortificados.",
        "Eliminar todas as tropas da cor Vermelha do mapa.",
        "Dominar todos os territórios do 'Continente A' (IDs 0-2).",
        "Ter no mínimo 10 tropas em um único território.",
        "Conquistar 5 territórios."
    };
    
    // --- Variáveis de Jogo ---
    char corJogador[10] = "Vermelho"; // Cor assumida pelo jogador (Pode ser estático no Nível Mestre)
    Territorio* mapa = NULL;
    int escolha;
    
    // Armazenamento e acesso: A missão deve ser armazenada dinamicamente utilizando malloc.
    char* missaoJogador = (char*)malloc(TAM_MISSAO * sizeof(char));
    if (missaoJogador == NULL) {
        fprintf(stderr, "[ERRO CRÍTICO] Falha na alocação de memória para a missão.\n");
        return EXIT_FAILURE;
    }

    // 1. Configuração Inicial (Setup):
    limparTela();
    printf("**************************************************\n");
    printf("* WAR ESTRUTURADO - DESAFIO FINAL (MISSÕES) *\n");
    printf("**************************************************\n");
    
    // Inicializa a semente para geração de números aleatórios. (Requisito Técnico)
    srand(time(NULL));

    // Aloca e verifica a memória para o mapa (Requisito Estrutural)
    mapa = alocarMapa(MAX_TERRITORIOS);
    if (mapa == NULL) {
        free(missaoJogador);
        return EXIT_FAILURE;
    }

    // Preenche os territórios com seus dados iniciais
    inicializarTerritorios(mapa, MAX_TERRITORIOS);
    
    // Sorteia e copia a missão secreta
    sortearMissao(missaoJogador, missoes, NUM_MISSOES);
    printf("[SETUP] Sua cor neste jogo é: %s\n", corJogador);


    // 2. Laço Principal do Jogo (Game Loop):
    do {
        exibirMissao(missaoJogador);
        exibirMapa(mapa, MAX_TERRITORIOS);
        exibirMenuPrincipal();
        
        if (scanf("%d", &escolha) != 1) {
            printf("\n[ERRO] Opção inválida.\n");
            escolha = -1;
            limparBufferEntrada();
        }

        switch (escolha) {
            case 1: 
                faseDeAtaque(mapa, MAX_TERRITORIOS, corJogador); 
                break;
            case 2: 
                // Exibição condicional: Verifica se a condição de vitória foi alcançada
                if (verificarVitoria(missaoJogador, mapa, MAX_TERRITORIOS, corJogador)) {
                    printf("\n======================================================\n");
                    printf("         PARABÉNS! VOCÊ CONCLUIU SUA MISSÃO!          \n");
                    printf("         A cor %s é a vencedora!                      \n", corJogador);
                    printf("======================================================\n");
                    escolha = 0; // Encerra o jogo
                } else {
                    printf("\n[VERIFICAÇÃO] Missão ainda NÃO cumprida. Continue atacando!\n");
                }
                break;
            case 3: 
                // Já é exibido no loop, mas permite exibição extra
                exibirMapa(mapa, MAX_TERRITORIOS); 
                break; 
            case 0: 
                printf("\nEncerrando o jogo. Não alcançou a vitória desta vez.\n"); 
                break;
            default: 
                printf("\nOpção desconhecida.\n");
        }
    } while (escolha != 0);

    // 3. Limpeza:
    // Utiliza free() ao final para evitar vazamentos de memória (Requisito Técnico)
    liberarMemoria(mapa, missaoJogador);

    return 0;
}