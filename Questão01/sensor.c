#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEITURAS 10000
#define MAX_SENSORES 100

typedef struct {
    long int timestamp;
    char id_sensor[50];
    float valor;
} Leitura;

// Funcao para ordenar leituras usando selection sort
void ordenar_por_timestamp(Leitura *leituras, int n) {
    int i, j, min_idx;
    Leitura temp;
    for (i = 0; i < n-1; i++) {
        min_idx = i;
        for (j = i+1; j < n; j++) {
            if (leituras[j].timestamp < leituras[min_idx].timestamp) {
                min_idx = j;
            }
        }
        if (min_idx != i) {
            temp = leituras[i];
            leituras[i] = leituras[min_idx];
            leituras[min_idx] = temp;
        }
    }
}

// Funcao para verificar se um sensor ja foi registrado
int sensor_existe(char sensores[][50], int num_sensores, char *id_sensor) {
    for (int i = 0; i < num_sensores; i++) {
        if (strcmp(sensores[i], id_sensor) == 0) {
            return 1;
        }
    }
    return 0;
}

// Funcao para processar um sensor especifico
void processar_sensor(Leitura *leituras, int num_leituras, char *sensor_escolhido) {
    Leitura leituras_sensor[MAX_LEITURAS];
    int n = 0;

    // Separa as leituras do sensor escolhido
    for (int j = 0; j < num_leituras; j++) {
        if (strcmp(leituras[j].id_sensor, sensor_escolhido) == 0) {
            leituras_sensor[n] = leituras[j];
            n++;
        }
    }

    if (n == 0) {
        printf("Nenhuma leitura encontrada para o sensor %s.\n", sensor_escolhido);
        return;
    }

    ordenar_por_timestamp(leituras_sensor, n);

    char nome_arquivo[60];
    sprintf(nome_arquivo, "%s.txt", sensor_escolhido);

    FILE *saida = fopen(nome_arquivo, "w");
    if (saida == NULL) {
        printf("Erro ao criar o arquivo %s\n", nome_arquivo);
        return;
    }

    for (int j = 0; j < n; j++) {
        fprintf(saida, "%ld %s %.2f\n", leituras_sensor[j].timestamp, leituras_sensor[j].id_sensor, leituras_sensor[j].valor);
    }
    fclose(saida);

    printf("Arquivo %s gerado com sucesso!\n", nome_arquivo);
}

int main() {
    FILE *entrada;
    Leitura leituras[MAX_LEITURAS];
    int num_leituras = 0;

    char sensores[MAX_SENSORES][50];
    int num_sensores = 0;

    entrada = fopen("entrada.txt", "r");
    if (entrada == NULL) {
        printf("Erro ao abrir o arquivo de entrada.\n");
        return 1;
    }

    // Leitura do arquivo
    while (fscanf(entrada, "%ld %s %f", &leituras[num_leituras].timestamp, leituras[num_leituras].id_sensor, &leituras[num_leituras].valor) == 3) {
        if (!sensor_existe(sensores, num_sensores, leituras[num_leituras].id_sensor)) {
            strcpy(sensores[num_sensores], leituras[num_leituras].id_sensor);
            num_sensores++;
        }
        num_leituras++;
    }
    fclose(entrada);

    int opcao;
    do {
        printf("\n===== MENU =====\n");
        printf("1 - Processar TODOS os sensores\n");
        printf("2 - Processar UM sensor especifico\n");
        printf("3 - Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1:
                for (int i = 0; i < num_sensores; i++) {
                    processar_sensor(leituras, num_leituras, sensores[i]);
                }
                break;

            case 2: {
                char sensor_desejado[50];
                printf("Digite o ID do sensor desejado: ");
                scanf("%s", sensor_desejado);
                processar_sensor(leituras, num_leituras, sensor_desejado);
                break;
            }

            case 3:
                printf("Encerrando o programa.\n");
                break;

            default:
                printf("Opcao invalida. Tente novamente.\n");
        }

    } while (opcao != 3);

    return 0;
}
