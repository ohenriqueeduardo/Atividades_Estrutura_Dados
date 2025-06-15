#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>

#define NUM_LEITURAS 2000
#define MAX_SENSORES 10
#define MAX_STRING_LEN 16

typedef enum {
    INT,
    FLOAT,
    BOOL,
    STRING
} TipoDado;

typedef struct {
    char nome[50];
    TipoDado tipo;
} Sensor;

long gerar_timestamp_aleatorio(long inicio, long fim) {
    return inicio + rand() % (fim - inicio + 1);
}

void gerar_valor_aleatorio(TipoDado tipo, char *valor_str) {
    switch(tipo) {
        case INT:
            snprintf(valor_str, 20, "%d", rand() % 100);
            break;
        case FLOAT:
            snprintf(valor_str, 20, "%.2f", (float)(rand() % 10000) / 100);
            break;
        case BOOL:
            snprintf(valor_str, 20, "%s", rand() % 2 ? "true" : "false");
            break;
        case STRING: {
            const char chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
            int len = 5 + rand() % (MAX_STRING_LEN - 5 + 1);
            for (int i = 0; i < len; i++) {
                valor_str[i] = chars[rand() % (sizeof(chars) - 1)];
            }
            valor_str[len] = '\0';
            break;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 6 || (argc - 3) % 2 != 0) {
        printf("Uso: %s <inicio_timestamp> <fim_timestamp> <sensor1> <tipo1> <sensor2> <tipo2> ...\n", argv[0]);
        printf("Tipos disponiveis: int, float, bool, string\n");
        printf("Exemplo: %s 1630454400 1630540800 temp float umid int status bool codigo string\n", argv[0]);
        return 1;
    }

    srand(time(NULL) ^ getpid());

    long inicio = atol(argv[1]);
    long fim = atol(argv[2]);
    int num_sensores = (argc - 3) / 2;
    Sensor sensores[MAX_SENSORES];

    // Parse dos sensores e tipos
    for (int i = 0; i < num_sensores; i++) {
        strncpy(sensores[i].nome, argv[3 + i*2], 49);
        
        if (strcmp(argv[4 + i*2], "int") == 0) sensores[i].tipo = INT;
        else if (strcmp(argv[4 + i*2], "float") == 0) sensores[i].tipo = FLOAT;
        else if (strcmp(argv[4 + i*2], "bool") == 0) sensores[i].tipo = BOOL;
        else if (strcmp(argv[4 + i*2], "string") == 0) sensores[i].tipo = STRING;
        else {
            printf("Tipo inválido: %s\n", argv[4 + i*2]);
            return 1;
        }
    }

    FILE *saida = fopen("dados_sensores_gerados.txt", "w");
    if (!saida) {
        perror("Erro ao criar arquivo");
        return 1;
    }

    for (int i = 0; i < NUM_LEITURAS; i++) {
        Sensor sensor = sensores[rand() % num_sensores];
        long timestamp = gerar_timestamp_aleatorio(inicio, fim);
        char valor[20];
        gerar_valor_aleatorio(sensor.tipo, valor);
        
        fprintf(saida, "%ld %s %s\n", timestamp, sensor.nome, valor);
    }

    fclose(saida);
    printf("Arquivo 'dados_sensores_gerados.txt' criado com %d leituras aleatorias.\n", NUM_LEITURAS);
    return 0;
}