#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ========== CONSTANTES E DEFINIÇÕES ========== */
#define MAX_JOGADORES 6
#define MAX_NOME 50
#define PONTOS_VITORIA 21
#define VITORIAS_JOGO 3
#define MAX_JOGOS 6

/* ========== ESTRUTURAS DE DADOS ========== */
typedef struct {
    char nome[MAX_NOME];
    int pontos;
    int vitorias_rodada_total;
    int vitorias_rodada_jogo;
    int vitorias_jogo;
    int estouros;
    int vezes_21;
    int status;
} Jogador;

/* ========== VARIÁVEIS GLOBAIS ========== */
Jogador jogadores[MAX_JOGADORES];
int num_jogadores = 0;
int rodada_atual = 1;
int jogo_atual = 1;

/* ========== PROTÓTIPOS DE FUNÇÕES ========== */
void limpar_buffer();
void inserir_nomes_jogadores();
int comprar_carta_probabilidade_alta();
void distribuir_primeira_carta();
void realizar_turno();
int verificar_vencedor_rodada();
void mostrar_placar();
void resetar_rodada();
void resetar_jogo();
void gerar_log_partida();

/* ========== IMPLEMENTAÇÃO DAS FUNÇÕES ========== */
void limpar_buffer() {
    while (getchar() != '\n');
}

void inserir_nomes_jogadores() {
    printf("Quantos jogadores (2-6)? ");
    scanf("%d", &num_jogadores);
    limpar_buffer();
    
    if (num_jogadores < 2 || num_jogadores > 6) {
        printf("Numero invalido. Usando 2 jogadores.\n");
        num_jogadores = 2;
    }
    
    for (int i = 0; i < num_jogadores; i++) {
        printf("Nome do jogador %d: ", i+1);
        fgets(jogadores[i].nome, MAX_NOME, stdin);
        jogadores[i].nome[strcspn(jogadores[i].nome, "\n")] = '\0';
        
        jogadores[i].pontos = 0;
        jogadores[i].vitorias_rodada_total = 0;
        jogadores[i].vitorias_rodada_jogo = 0;
        jogadores[i].vitorias_jogo = 0;
        jogadores[i].estouros = 0;
        jogadores[i].vezes_21 = 0;
        jogadores[i].status = 1;
    }
}

int comprar_carta_probabilidade_alta() {
    int prob = rand() % 10 + 1;
    
    if (rand() % 10 < 6) {
        prob = 8 + rand() % 3;
    }
    
    return prob;
}

void distribuir_primeira_carta() {
    for (int i = 0; i < num_jogadores; i++) {
        jogadores[i].pontos = comprar_carta_probabilidade_alta();
        jogadores[i].status = 1;
    }
}

void realizar_turno() {
    for (int i = 0; i < num_jogadores; i++) {
        while (jogadores[i].status == 1 && jogadores[i].pontos < PONTOS_VITORIA) {
            printf("\n%s: %d pontos. Comprar mais? (s/n): ", jogadores[i].nome, jogadores[i].pontos);
            char resposta;
            scanf(" %c", &resposta);
            limpar_buffer();
            
            if (resposta == 's' || resposta == 'S') {
                int carta = comprar_carta_probabilidade_alta();
                jogadores[i].pontos += carta;
                printf("+%d pontos. Total: %d\n", carta, jogadores[i].pontos);
                
                if (jogadores[i].pontos > PONTOS_VITORIA) {
                    printf("Estourou!\n");
                    jogadores[i].estouros++;
                    jogadores[i].status = 0;
                } else if (jogadores[i].pontos == PONTOS_VITORIA) {
                    printf("21 pontos!\n");
                    jogadores[i].vezes_21++;
                    jogadores[i].status = 0;
                }
            } else {
                jogadores[i].status = 0;
            }
        }
    }
}

int verificar_vencedor_rodada() {
    int vencedor = -1;
    int max_pontos = -1;
    int empate = 0;
    
    for (int i = 0; i < num_jogadores; i++) {
        if (jogadores[i].pontos == PONTOS_VITORIA) {
            return i;
        }
        
        if (jogadores[i].pontos < PONTOS_VITORIA && jogadores[i].pontos > max_pontos) {
            max_pontos = jogadores[i].pontos;
            vencedor = i;
            empate = 0;
        } else if (jogadores[i].pontos == max_pontos) {
            empate = 1;
        }
    }
    
    if (max_pontos == -1) {
        int menor_excesso = PONTOS_VITORIA;
        for (int i = 0; i < num_jogadores; i++) {
            int excesso = jogadores[i].pontos - PONTOS_VITORIA;
            if (excesso < menor_excesso) {
                menor_excesso = excesso;
                vencedor = i;
                empate = 0;
            } else if (excesso == menor_excesso) {
                empate = 1;
            }
        }
    }
    
    return empate ? -1 : vencedor;
}

void mostrar_placar() {
    printf("\n=== Placar (Rodada %d, Jogo %d) ===\n", rodada_atual, jogo_atual);
    
    for (int i = 0; i < num_jogadores; i++) {
        printf("%s: %d pts | Rodadas (jogo): %d | Rodadas (total): %d | Jogos: %d\n",
               jogadores[i].nome, jogadores[i].pontos,
               jogadores[i].vitorias_rodada_jogo,
               jogadores[i].vitorias_rodada_total,
               jogadores[i].vitorias_jogo);
    }
}

void resetar_rodada() {
    for (int i = 0; i < num_jogadores; i++) {
        jogadores[i].pontos = 0;
        jogadores[i].status = 1;
    }
}

void resetar_jogo() {
    for (int i = 0; i < num_jogadores; i++) {
        jogadores[i].vitorias_rodada_jogo = 0;
    }
    rodada_atual = 1;
}

void gerar_log_partida() {
    FILE *log = fopen("log_partida.txt", "w");
    if (!log) {
        printf("Erro ao criar arquivo de log.\n");
        return;
    }
    
    fprintf(log, "Resultado final da partida:\n");
    fprintf(log, "Total de jogos completos: %d\n", jogo_atual-1);
    fprintf(log, "Rodadas vencidas por jogador:\n");
    
    int mais_21 = 0, mais_estouros = 0, menos_vitorias = 0;
    int idx_mais_21 = 0, idx_mais_estouros = 0, idx_menos_vitorias = 0;
    
    for (int i = 0; i < num_jogadores; i++) {
        fprintf(log, "- %s: %d\n", jogadores[i].nome, jogadores[i].vitorias_rodada_total);
        
        if (jogadores[i].vezes_21 > mais_21) {
            mais_21 = jogadores[i].vezes_21;
            idx_mais_21 = i;
        }
        
        if (jogadores[i].estouros > mais_estouros) {
            mais_estouros = jogadores[i].estouros;
            idx_mais_estouros = i;
        }
        
        if (jogadores[i].vitorias_jogo < jogadores[menos_vitorias].vitorias_jogo) {
            menos_vitorias = i;
        }
    }
    
    fprintf(log, "\nMais vezes com 21 pontos: %s (%dx)\n", jogadores[idx_mais_21].nome, mais_21);
    fprintf(log, "Mais estouros: %s (%dx)\n", jogadores[idx_mais_estouros].nome, mais_estouros);
    fprintf(log, "Jogador que mais perdeu: %s (%d jogos vencidos)\n", 
            jogadores[menos_vitorias].nome, jogadores[menos_vitorias].vitorias_jogo);
    
    fclose(log);
}

/* ========== FUNÇÃO PRINCIPAL ========== */
int main() {
    srand(time(NULL));
    
    printf("=== Jogo de 21 ===\n");
    inserir_nomes_jogadores();
    
    int partida_encerrada = 0;
    
    while (!partida_encerrada && jogo_atual <= MAX_JOGOS) {
        resetar_jogo();
        int jogo_encerrado = 0;
        
        while (!jogo_encerrado && !partida_encerrada) {
            printf("\n=== Rodada %d ===\n", rodada_atual);
            distribuir_primeira_carta();
            
            int rodada_encerrada = 0;
            while (!rodada_encerrada) {
                realizar_turno();
                
                int ativos = 0;
                for (int i = 0; i < num_jogadores; i++) {
                    if (jogadores[i].status == 1 && jogadores[i].pontos < PONTOS_VITORIA) {
                        ativos++;
                    }
                }
                rodada_encerrada = (ativos == 0);
            }
            
            int vencedor = verificar_vencedor_rodada();
            if (vencedor != -1) {
                printf("\nVencedor da rodada: %s\n", jogadores[vencedor].nome);
                jogadores[vencedor].vitorias_rodada_total++;
                jogadores[vencedor].vitorias_rodada_jogo++;
                
                if (jogadores[vencedor].vitorias_rodada_jogo >= VITORIAS_JOGO) {
                    printf("\n%s venceu o jogo %d!\n", jogadores[vencedor].nome, jogo_atual);
                    jogadores[vencedor].vitorias_jogo++;
                    
                    if (jogadores[vencedor].vitorias_jogo >= 3) {
                        printf("\n%s atingiu 3 jogos vencidos!\n", jogadores[vencedor].nome);
                        gerar_log_partida();
                        
                        printf("\nDeseja continuar jogando? (s/n): ");
                        char resposta;
                        scanf(" %c", &resposta);
                        limpar_buffer();
                        
                        if (resposta != 's' && resposta != 'S') {
                            partida_encerrada = 1;
                        }
                    }
                    
                    jogo_encerrado = 1;
                    jogo_atual++;
                }
            } else {
                printf("\nRodada anulada (empate)\n");
            }
            
            mostrar_placar();
            resetar_rodada();
            rodada_atual++;
        }
        
        if (!partida_encerrada && jogo_atual <= MAX_JOGOS) {
            printf("\nContinuar para o proximo jogo? (s/n): ");
            char resposta;
            scanf(" %c", &resposta);
            limpar_buffer();
            if (resposta != 's' && resposta != 'S') {
                partida_encerrada = 1;
            }
        }
    }
    
    if (jogo_atual >= MAX_JOGOS) {
        printf("\nLimite de %d jogos alcancado!\n", MAX_JOGOS);
    }
    
    gerar_log_partida();
    printf("\nFim do jogo. Obrigado por jogar!\n");
    return 0;
}