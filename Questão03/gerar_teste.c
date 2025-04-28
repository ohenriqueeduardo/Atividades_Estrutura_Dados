#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_SENSORES 5
#define LEITURAS_POR_SENSOR 1000

// Definição de sensores
const char *sensores[NUM_SENSORES] = {"TEMP", "PRES", "VIBR", "UMID", "FLUX"};

// Função para gerar um valor aleatório para o sensor
float gerar_valor(int tipo_sensor) {
    switch (tipo_sensor) {
        case 0: // TEMP
            return (rand() % 1000) / 10.0 + 20.0; // Temperatura aleatória entre 20.0 e 130.0
        case 1: // PRES
            return (rand() % 1000) / 10.0 + 1000.0; // Pressão aleatória entre 1000.0 e 2000.0
        case 2: // VIBR
            return (rand() % 1000) / 1000.0; // Vibração aleatória entre 0.0 e 1.0
        case 3: // UMID
            return rand() % 101; // Umidade aleatória entre 0 e 100
        case 4: // FLUX
            return (rand() % 5000) / 10.0; // Fluxo aleatório entre 0.0 e 500.0
        default:
            return 0.0;
    }
}

// Função para gerar um timestamp aleatório dentro de uma data específica
time_t gerar_timestamp(int dia, int mes, int ano) {
    struct tm t = {0};

    // Preenche a data fornecida pelo usuário
    t.tm_year = ano - 1900; // O ano é contado a partir de 1900
    t.tm_mon = mes - 1; // O mês é contado de 0 a 11
    t.tm_mday = dia;
    t.tm_hour = rand() % 24; // Hora aleatória
    t.tm_min = rand() % 60;  // Minuto aleatório
    t.tm_sec = rand() % 60;  // Segundo aleatório

    return mktime(&t); // Retorna o timestamp no formato Unix Epoch
}

int main() {
    // Variáveis para entrada de data
    int dia, mes, ano;

    // Solicita a data ao usuário
    printf("Digite o dia (1-31): ");
    scanf("%d", &dia);
    printf("Digite o mes (1-12): ");
    scanf("%d", &mes);
    printf("Digite o ano: ");
    scanf("%d", &ano);

    // Inicializa o gerador de números aleatórios
    srand(time(NULL));

    // Abre o arquivo de saída
    FILE *fp = fopen("dados_teste.txt", "w");
    if (!fp) {
        printf("Erro ao abrir o arquivo para escrita.\n");
        return 1;
    }

    // Gera as leituras aleatórias para cada sensor
    for (int i = 0; i < NUM_SENSORES; i++) {
        for (int j = 0; j < LEITURAS_POR_SENSOR; j++) {
            // Gera o timestamp aleatório para cada leitura
            time_t timestamp = gerar_timestamp(dia, mes, ano);

            // Gera o valor aleatório para o sensor
            float valor = gerar_valor(i);

            // Escreve a leitura no arquivo
            fprintf(fp, "%ld %s %.2f\n", timestamp, sensores[i], valor);
        }
    }

    // Fecha o arquivo
    fclose(fp);

    printf("\nArquivo de teste gerado com sucesso em 'dados_teste.txt'.\n");
    return 0;
}
