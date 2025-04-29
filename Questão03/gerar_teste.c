#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_SENSORES 5
#define LEITURAS_POR_SENSOR 1000

const char *sensores[NUM_SENSORES] = {"TEMP", "PRES", "VIBR", "UMID", "FLUX"};

// Valor aleatório 
float gerar_valor(int tipo_sensor) {
    switch (tipo_sensor) {
        case 0:
            return (rand() % 1000) / 10.0 + 20.0;
        case 1:
            return (rand() % 1000) / 10.0 + 1000.0;
        case 2:
            return (rand() % 1000) / 1000.0; 
        case 3: 
            return rand() % 101; 
        case 4: 
            return (rand() % 5000) / 10.0; 
        default:
            return 0.0;
    }
}

// timestamp aleatorio de data
time_t gerar_timestamp(int dia, int mes, int ano) {
    struct tm t = {0};

    t.tm_year = ano - 1900; 
    t.tm_mon = mes - 1; 
    t.tm_mday = dia;
    t.tm_hour = rand() % 24; 
    t.tm_min = rand() % 60;  
    t.tm_sec = rand() % 60;

    return mktime(&t);
}

int main() {
    int dia, mes, ano;

    printf("Digite o dia (1-31): ");
    scanf("%d", &dia);
    printf("Digite o mes (1-12): ");
    scanf("%d", &mes);
    printf("Digite o ano: ");
    scanf("%d", &ano);

    srand(time(NULL));

    FILE *fp = fopen("dados_teste.txt", "w");
    if (!fp) {
        printf("Erro ao abrir o arquivo para escrita.\n");
        return 1;
    }

    // Leituras aleatorias
    for (int i = 0; i < NUM_SENSORES; i++) {
        for (int j = 0; j < LEITURAS_POR_SENSOR; j++) {
            time_t timestamp = gerar_timestamp(dia, mes, ano);
            float valor = gerar_valor(i);
            fprintf(fp, "%ld %s %.2f\n", timestamp, sensores[i], valor);
        }
    }
    fclose(fp);

    printf("\nArquivo de teste gerado com sucesso em 'dados_teste.txt'.\n");
    return 0;
}
