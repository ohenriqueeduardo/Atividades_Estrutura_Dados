#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>

#define MAX_LEITURAS 2000
#define MAX_SENSORS 10
#define MAX_STR_LEN 16
#define MAX_BUF_LEN 256

typedef enum {
    TIPO_INT,
    TIPO_FLOAT,
    TIPO_BOOL,
    TIPO_STRING
} TipoDado;

typedef struct {
    char id[50];
    TipoDado tipo;
} Sensor;

void limpa_buf() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

TipoDado str_para_tipo(const char *s) {
    if (strcmp(s, "CONJ_Z") == 0) return TIPO_INT;
    if (strcmp(s, "CONJ_Q") == 0) return TIPO_FLOAT;
    if (strcmp(s, "BINARIO") == 0) return TIPO_BOOL;
    if (strcmp(s, "TEXTO") == 0) return TIPO_STRING;
    return -1;
}

long gera_ts_rand(long ini, long fim) {
    if (fim < ini) return ini;
    return ini + rand() % (fim - ini + 1);
}

void gera_val_rand(TipoDado tipo, char *val_str) {
    switch(tipo) {
        case TIPO_INT:
            snprintf(val_str, 20, "%d", rand() % 100);
            break;
        case TIPO_FLOAT:
            snprintf(val_str, 20, "%.2f", (float)(rand() % 10000) / 100.0);
            break;
        case TIPO_BOOL:
            snprintf(val_str, 20, "%s", rand() % 2 ? "true" : "false");
            break;
        case TIPO_STRING: {
            const char chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
            int len = 5 + rand() % (MAX_STR_LEN - 5 + 1);
            for (int i = 0; i < len; i++) {
                val_str[i] = chars[rand() % (sizeof(chars) - 1)];
            }
            val_str[len] = '\0';
            break;
        }
    }
}

void gera_arq_teste() {
    long ini_ts, fim_ts;
    char buf[MAX_BUF_LEN];
    Sensor sensors[MAX_SENSORS];
    int num_sensors = 0;

    printf("\n--- Gerar Arquivo de Teste ---\n");

    printf("Digite o timestamp de inicio (Unix Epoch): ");
    if (scanf("%ld", &ini_ts) != 1) {
        printf("Entrada invalida para timestamp de inicio. Abortando.\n");
        limpa_buf();
        return;
    }
    limpa_buf();

    printf("Digite o timestamp de fim (Unix Epoch): ");
    if (scanf("%ld", &fim_ts) != 1) {
        printf("Entrada invalida para timestamp de fim. Abortando.\n");
        limpa_buf();
        return;
    }
    limpa_buf();

    if (fim_ts < ini_ts) {
        printf("Timestamp de fim nao pode ser menor que o de inicio. Abortando.\n");
        return;
    }

    printf("\nAdicione sensores (maximo %d). Digite 'fim' para parar.\n", MAX_SENSORS);
    printf("Tipos de dados aceitos: CONJ_Z (inteiro), CONJ_Q (float), BINARIO (booleano), TEXTO (string).\n");
    while (num_sensors < MAX_SENSORS) {
        char s_nome[50];
        char s_tipo_str[20];
        TipoDado s_tipo;

        printf("ID do sensor %d (ou 'fim' para encerrar): ", num_sensors + 1);
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            printf("Erro de leitura. Abortando.\n");
            return;
        }
        sscanf(buf, "%s", s_nome);

        if (strcmp(s_nome, "fim") == 0) {
            break;
        }
        
        if (strlen(s_nome) == 0) {
            printf("ID do sensor nao pode ser vazio. Tente novamente.\n");
            continue;
        }

        printf("Tipo do dado para '%s' (CONJ_Z, CONJ_Q, BINARIO, TEXTO): ", s_nome);
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            printf("Erro de leitura. Abortando.\n");
            return;
        }
        buf[strcspn(buf, "\n")] = 0;
        strncpy(s_tipo_str, buf, sizeof(s_tipo_str) -1);
        s_tipo_str[sizeof(s_tipo_str) - 1] = '\0';

        s_tipo = str_para_tipo(s_tipo_str);
        if (s_tipo == -1) {
            printf("Tipo de dado '%s' invalido. Por favor, use CONJ_Z, CONJ_Q, BINARIO ou TEXTO. Tente novamente.\n", s_tipo_str);
            continue;
        }

        strncpy(sensors[num_sensors].id, s_nome, sizeof(sensors[num_sensors].id) - 1);
        sensors[num_sensors].id[sizeof(sensors[num_sensors].id) - 1] = '\0';
        sensors[num_sensors].tipo = s_tipo;
        num_sensors++;
    }

    if (num_sensors == 0) {
        printf("Nenhum sensor adicionado. Nao foi possivel gerar o arquivo.\n");
        return;
    }

    srand(time(NULL) ^ getpid());

    FILE *out_f = fopen("dados_sensores_gerados.txt", "w");
    if (!out_f) {
        perror("Erro ao criar arquivo");
        return;
    }

    for (int i = 0; i < MAX_LEITURAS; i++) {
        Sensor s = sensors[rand() % num_sensors];
        long ts = gera_ts_rand(ini_ts, fim_ts);
        char val[20];
        gera_val_rand(s.tipo, val);
        
        fprintf(out_f, "%ld %s %s\n", ts, s.id, val);
    }

    fclose(out_f);
    printf("Arquivo 'dados_sensores_gerados.txt' criado com %d leituras aleatorias.\n", MAX_LEITURAS);
}

void menu_principal() {
    int opt;
    do {
        printf("\n--- Menu Principal (Gerador de Dados) ---\n");
        printf("1. Gerar Arquivo de Teste\n");
        printf("2. Sair\n");
        printf("Escolha uma opcao: ");

        if (scanf(" %d", &opt) != 1) {
            printf("Opcao invalida! Digite um numero.\n");
            limpa_buf();
            opt = 0;
            continue;
        }
        limpa_buf();

        switch (opt) {
            case 1:
                gera_arq_teste();
                break;
            case 2:
                printf("Saindo do programa. Ate mais!\n");
                break;
            default:
                printf("Opcao invalida! Por favor, tente novamente.\n");
                break;
        }
    } while (opt != 2);
}

int main() {
    menu_principal();
    return 0;
}