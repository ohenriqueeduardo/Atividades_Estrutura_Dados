#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_LINHAS 10000

typedef struct
{
    time_t timestamp;
    float valor;
} Leitura;

// Compara duas leituras
int comparar(const void *a, const void *b)
{
    Leitura *l1 = (Leitura *)a;
    Leitura *l2 = (Leitura *)b;
    if (l1->timestamp < l2->timestamp)
        return -1;
    else if (l1->timestamp > l2->timestamp)
        return 1;
    else
        return 0;
}

// Leitura binária
int busca_binaria(Leitura leituras[], int n, time_t alvo)
{
    int inicio = 0, fim = n - 1;
    int melhor_indice = -1;
    long menor_diferenca = -1;

    while (inicio <= fim)
    {
        int meio = (inicio + fim) / 2;
        long diferenca = labs(difftime(leituras[meio].timestamp, alvo));

        if (menor_diferenca == -1 || diferenca < menor_diferenca)
        {
            menor_diferenca = diferenca;
            melhor_indice = meio;
        }

        if (leituras[meio].timestamp < alvo)
        {
            inicio = meio + 1;
        }
        else if (leituras[meio].timestamp > alvo)
        {
            fim = meio - 1;
        }
        else
        {
            return meio; // Preciso
        }
    }
    return melhor_indice;
}

int main()
{
    char sensor[10];
    char datahora[20];

    printf("Informe o nome do sensor (ex: TEMP, PRES, VIBR): ");
    scanf("%s", sensor);

    printf("Informe a data e hora (AAAA-MM-DD HH:MM:SS): ");
    getchar();
    fgets(datahora, sizeof(datahora), stdin);
    datahora[strcspn(datahora, "\n")] = 0;

    // Data e hora em timestamp
    struct tm t;
    memset(&t, 0, sizeof(struct tm));
    sscanf(datahora, "%d-%d-%d %d:%d:%d",
           &t.tm_year, &t.tm_mon, &t.tm_mday,
           &t.tm_hour, &t.tm_min, &t.tm_sec);
    t.tm_year -= 1900;
    t.tm_mon -= 1;
    time_t momento = mktime(&t);

    // Arquivo completo
    FILE *fp = fopen("dados.txt", "r");
    if (!fp)
    {
        printf("Erro ao abrir o arquivo dados.txt.\n");
        return 1;
    }

    Leitura leituras[MAX_LINHAS];
    int total = 0;

    char linha[100];
    while (fgets(linha, sizeof(linha), fp))
    {
        time_t ts;
        char sensor_lido[10];
        float valor;

        if (sscanf(linha, "%ld %s %f", &ts, sensor_lido, &valor) == 3)
        {
            if (strcmp(sensor_lido, sensor) == 0)
            {
                if (total < MAX_LINHAS)
                {
                    leituras[total].timestamp = ts;
                    leituras[total].valor = valor;
                    total++;
                }
            }
        }
    }
    fclose(fp);

    if (total == 0)
    {
        printf("Nenhuma leitura encontrada para o sensor %s.\n", sensor);
        return 1;
    }

    // Leituras por timestamp
    qsort(leituras, total, sizeof(Leitura), comparar);

    // Leitura mais próxima
    int indice = busca_binaria(leituras, total, momento);
    if (indice >= 0)
    {
        struct tm *tm_info = localtime(&leituras[indice].timestamp);
        char buffer[20];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm_info);

        printf("\nLeitura mais proxima:\n");
        printf("Timestamp: %s\n", buffer);
        printf("Valor: %.2f\n", leituras[indice].valor);
    }
    else
    {
        printf("\nNenhuma leitura encontrada.\n");
    }

    return 0;
}
