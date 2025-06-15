#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

#define MAX_LINE_LENGTH 100

typedef struct {
    long timestamp;
    char value_str[MAX_LINE_LENGTH];
} Leitura;

// Função para carregar as leituras do arquivo do sensor
int carregar_leituras(const char *nome_arquivo, Leitura *leituras) {
    FILE *file = fopen(nome_arquivo, "r");
    if (file == NULL) {
        perror("Erro ao abrir arquivo do sensor");
        return -1;
    }

    int count = 0;
    while (fscanf(file, "%ld %[^\n]", &leituras[count].timestamp, leituras[count].value_str) == 2) {
        count++;
    }

    fclose(file);
    return count;
}

// Função de busca binária para encontrar a leitura mais próxima
Leitura* encontrar_leitura_mais_proxima(Leitura *leituras, int num_leituras, long timestamp_alvo) {
    if (num_leituras == 0) return NULL;

    int left = 0;
    int right = num_leituras - 1;
    int mid = 0;
    Leitura *mais_proxima = &leituras[0];
    long menor_diff = labs(leituras[0].timestamp - timestamp_alvo);

    while (left <= right) {
        mid = left + (right - left) / 2;

        long diff_atual = labs(leituras[mid].timestamp - timestamp_alvo);
        if (diff_atual < menor_diff) {
            menor_diff = diff_atual;
            mais_proxima = &leituras[mid];
        }

        if (leituras[mid].timestamp == timestamp_alvo) {
            return &leituras[mid];
        } else if (leituras[mid].timestamp < timestamp_alvo) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    // Verifica os vizinhos para garantir que é o mais próximo
    if (mid > 0) {
        long diff_anterior = labs(leituras[mid-1].timestamp - timestamp_alvo);
        if (diff_anterior < menor_diff) {
            mais_proxima = &leituras[mid-1];
            menor_diff = diff_anterior;
        }
    }
    if (mid < num_leituras - 1) {
        long diff_proximo = labs(leituras[mid+1].timestamp - timestamp_alvo);
        if (diff_proximo < menor_diff) {
            mais_proxima = &leituras[mid+1];
        }
    }

    return mais_proxima;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Uso: %s <nome_sensor> <timestamp>\n", argv[0]);
        printf("Exemplo: %s sensor_temp 1630454410\n", argv[0]);
        return 1;
    }

    const char *nome_sensor = argv[1];
    long timestamp_alvo = atol(argv[2]);

    char nome_arquivo[100];
    snprintf(nome_arquivo, sizeof(nome_arquivo), "%s.txt", nome_sensor);

    Leitura leituras[MAX_LINE_LENGTH];
    int num_leituras = carregar_leituras(nome_arquivo, leituras);

    if (num_leituras <= 0) {
        printf("Nenhuma leitura encontrada para o sensor %s\n", nome_sensor);
        return 1;
    }

    Leitura *leitura_proxima = encontrar_leitura_mais_proxima(leituras, num_leituras, timestamp_alvo);

    if (leitura_proxima != NULL) {
        printf("Leitura mais proxima encontrada:\n");
        printf("Timestamp: %ld\n", leitura_proxima->timestamp);
        printf("Valor: %s\n", leitura_proxima->value_str);
    } else {
        printf("Nenhuma leitura encontrada proxima do timestamp %ld\n", timestamp_alvo);
    }

    return 0;
}