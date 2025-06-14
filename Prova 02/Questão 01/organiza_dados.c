#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_LEITURAS 10000
#define MAX_SENSORES 100
#define MAX_ID_LENGTH 50
#define MAX_STRING_VALUE 16

// Enum para os tipos de dados suportados
typedef enum {
    INT,
    FLOAT,
    BOOL,
    STRING,
    UNKNOWN
} TipoDado;

// Estrutura para armazenar informações sobre um sensor
typedef struct {
    char id[MAX_ID_LENGTH];
    TipoDado tipo;
} SensorInfo;

// Leitura de dados do sensor com union para diferentes tipos
typedef struct {
    long timestamp;
    char id_sensor[MAX_ID_LENGTH];
    TipoDado tipo;
    union {
        int int_val;
        float float_val;
        bool bool_val;
        char string_val[MAX_STRING_VALUE + 1];
    } valor;
} Leitura;

// Função para determinar o tipo de dado de uma string
TipoDado determinar_tipo(const char *valor) {
    // Verifica se é booleano
    if (strcmp(valor, "true") == 0 || strcmp(valor, "false") == 0) {
        return BOOL;
    }
    
    // Verifica se é inteiro
    char *endptr;
    long int_val = strtol(valor, &endptr, 10);
    if (*endptr == '\0') {
        return INT;
    }
    
    // Verifica se é float
    float float_val = strtof(valor, &endptr);
    if (*endptr == '\0') {
        return FLOAT;
    }
    
    // Verifica se é string (até 16 caracteres)
    if (strlen(valor) <= MAX_STRING_VALUE) {
        return STRING;
    }
    
    return UNKNOWN;
}

// Função para ordenar por timestamp
void ordenar_por_timestamp(Leitura *leituras, int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (leituras[j].timestamp > leituras[j + 1].timestamp) {
                Leitura temp = leituras[j];
                leituras[j] = leituras[j + 1];
                leituras[j + 1] = temp;
            }
        }
    }
}

// Função para processar o valor de acordo com o tipo
void processar_valor(Leitura *leitura, const char *valor_str) {
    switch (leitura->tipo) {
        case INT:
            leitura->valor.int_val = atoi(valor_str);
            break;
        case FLOAT:
            leitura->valor.float_val = atof(valor_str);
            break;
        case BOOL:
            leitura->valor.bool_val = (strcmp(valor_str, "true") == 0);
            break;
        case STRING:
            strncpy(leitura->valor.string_val, valor_str, MAX_STRING_VALUE);
            leitura->valor.string_val[MAX_STRING_VALUE] = '\0';
            break;
        default:
            break;
    }
}

// Função para escrever leituras em arquivo
void escrever_leituras_arquivo(const char *nome_arquivo, Leitura *leituras, int count) {
    FILE *file = fopen(nome_arquivo, "w");
    if (file == NULL) {
        perror("Erro ao abrir arquivo");
        return;
    }
    
    for (int i = 0; i < count; i++) {
        fprintf(file, "%ld ", leituras[i].timestamp);
        
        switch (leituras[i].tipo) {
            case INT:
                fprintf(file, "%d\n", leituras[i].valor.int_val);
                break;
            case FLOAT:
                fprintf(file, "%f\n", leituras[i].valor.float_val);
                break;
            case BOOL:
                fprintf(file, "%s\n", leituras[i].valor.bool_val ? "true" : "false");
                break;
            case STRING:
                fprintf(file, "%s\n", leituras[i].valor.string_val);
                break;
            default:
                break;
        }
    }
    
    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <arquivo_de_entrada>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        perror("Erro ao abrir arquivo");
        return 1;
    }

    Leitura leituras[MAX_LEITURAS];
    SensorInfo sensores[MAX_SENSORES];
    int total_leituras = 0;
    int total_sensores = 0;

    // Ler todas as leituras do arquivo
    char linha[256];
    while (fgets(linha, sizeof(linha), file) && total_leituras < MAX_LEITURAS) {
        char *token;
        char *resto = linha;
        
        // Extrair timestamp
        token = strtok_r(resto, " ", &resto);
        if (token == NULL) continue;
        leituras[total_leituras].timestamp = atol(token);
        
        // Extrair ID do sensor
        token = strtok_r(resto, " ", &resto);
        if (token == NULL) continue;
        strncpy(leituras[total_leituras].id_sensor, token, MAX_ID_LENGTH);
        leituras[total_leituras].id_sensor[MAX_ID_LENGTH - 1] = '\0';
        
        // Extrair valor (pode conter espaços se for string)
        token = strtok_r(resto, "\n", &resto);
        if (token == NULL) continue;
        
        // Determinar tipo de dado
        leituras[total_leituras].tipo = determinar_tipo(token);
        processar_valor(&leituras[total_leituras], token);
        
        // Verificar se o sensor já foi registrado
        int sensor_index = -1;
        for (int i = 0; i < total_sensores; i++) {
            if (strcmp(sensores[i].id, leituras[total_leituras].id_sensor) == 0) {
                sensor_index = i;
                break;
            }
        }
        
        if (sensor_index == -1 && total_sensores < MAX_SENSORES) {
            strncpy(sensores[total_sensores].id, leituras[total_leituras].id_sensor, MAX_ID_LENGTH);
            sensores[total_sensores].tipo = leituras[total_leituras].tipo;
            total_sensores++;
        }
        
        total_leituras++;
    }
    
    fclose(file);
    
    // Para cada sensor, filtrar suas leituras, ordenar e salvar em arquivo
    for (int i = 0; i < total_sensores; i++) {
        Leitura leituras_sensor[MAX_LEITURAS];
        int count = 0;
        
        // Filtrar leituras para este sensor
        for (int j = 0; j < total_leituras; j++) {
            if (strcmp(leituras[j].id_sensor, sensores[i].id) == 0) {
                leituras_sensor[count++] = leituras[j];
            }
        }
        
        // Ordenar leituras por timestamp
        ordenar_por_timestamp(leituras_sensor, count);
        
        // Criar nome do arquivo de saída
        char nome_arquivo[100];
        snprintf(nome_arquivo, sizeof(nome_arquivo), "%s.txt", sensores[i].id);
        
        // Escrever leituras no arquivo
        escrever_leituras_arquivo(nome_arquivo, leituras_sensor, count);
    }
    
    return 0;
}