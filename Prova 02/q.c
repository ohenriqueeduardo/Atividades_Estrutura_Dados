#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

#define MAX_FNAME 100
#define MAX_VAL 256

typedef struct {
    long ts;
    char val_str[MAX_VAL];
} Rd;

Rd* ld_rds(const char *f_name, int *num_rds) {
    FILE *f = fopen(f_name, "r");
    if (f == NULL) {
        perror("Erro ao abrir arquivo do sensor");
        *num_rds = 0;
        return NULL;
    }

    Rd *rds = NULL;
    int cap = 0;
    *num_rds = 0;

    char line_buf[MAX_VAL + 20];
    while (fgets(line_buf, sizeof(line_buf), f) != NULL) {
        if (*num_rds >= cap) {
            cap = (cap == 0) ? 10 : cap * 2;
            Rd *tmp = realloc(rds, cap * sizeof(Rd));
            if (tmp == NULL) {
                perror("Erro ao realocar memoria para leituras");
                free(rds);
                fclose(f);
                *num_rds = 0;
                return NULL;
            }
            rds = tmp;
        }

        if (sscanf(line_buf, "%ld %255[^\n]", &rds[*num_rds].ts, rds[*num_rds].val_str) == 2) {
            (*num_rds)++;
        } else {
            fprintf(stderr, "Aviso: Linha mal formatada ignorada: %s", line_buf);
        }
    }

    fclose(f);
    return rds;
}

int fnd_cls_rd_idx(Rd *rds, int n_rds, long tgt_ts) {
    if (n_rds == 0) return -1;

    int l = 0;
    int r = n_rds - 1;
    int cls_idx = 0;
    long min_diff = labs(rds[0].ts - tgt_ts);

    while (l <= r) {
        int m = l + (r - l) / 2;
        long curr_diff = labs(rds[m].ts - tgt_ts);

        if (curr_diff < min_diff || (curr_diff == min_diff && rds[m].ts > rds[cls_idx].ts)) {
            min_diff = curr_diff;
            cls_idx = m;
        }

        if (rds[m].ts == tgt_ts) {
            return m;
        } else if (rds[m].ts < tgt_ts) {
            l = m + 1;
        } else {
            r = m - 1;
        }
    }
    return cls_idx;
}

void consultar_dados() {
    char snsr_name[MAX_FNAME];
    long tgt_ts;

    printf("Digite o nome do sensor (ex: sensorA): ");
    scanf("%s", snsr_name);

    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    printf("Digite o timestamp Unix Epoch (ex: 1630454410): ");
    if (scanf("%ld", &tgt_ts) != 1) {
        printf("Timestamp invalido! Por favor, digite um numero.\n");
        while ((c = getchar()) != '\n' && c != EOF);
        return;
    }
    while ((c = getchar()) != '\n' && c != EOF);


    char f_name[MAX_FNAME];
    snprintf(f_name, sizeof(f_name), "%s.txt", snsr_name);

    int n_rds = 0;
    Rd *rds = ld_rds(f_name, &n_rds);

    if (rds == NULL || n_rds == 0) {
        fprintf(stderr, "Erro: Nenhuma leitura encontrada para o sensor '%s' ou arquivo nao existe/esta vazio.\n", snsr_name);
        return;
    }

    int cls_rd_idx = fnd_cls_rd_idx(rds, n_rds, tgt_ts);

    if (cls_rd_idx != -1) {
        printf("\n--- Leitura Mais Proxima Encontrada ---\n");
        printf("Sensor: %s\n", snsr_name);
        printf("Timestamp Buscado: %ld\n", tgt_ts);
        printf("Timestamp Encontrado: %ld\n", rds[cls_rd_idx].ts);
        printf("Valor: %s\n", rds[cls_rd_idx].val_str);
        printf("Diferenca (segundos): %ld\n", labs(rds[cls_rd_idx].ts - tgt_ts));
    } else {
        printf("Nenhuma leitura encontrada proxima do timestamp %ld para o sensor %s.\n", tgt_ts, snsr_name);
    }

    free(rds);
}

void main_menu() {
    int choice;
    do {
        printf("\n--- Menu Principal (Consulta) ---\n");
        printf("1. Consultar Dados de um Sensor\n");
        printf("2. Sair\n");
        printf("Escolha uma opcao: ");

        if (scanf(" %d", &choice) != 1) {
            printf("Opcao invalida! Digite um numero.\n");
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            choice = 0;
            continue;
        }
        int c;
        while ((c = getchar()) != '\n' && c != EOF);


        switch (choice) {
            case 1:
                consultar_dados();
                break;
            case 2:
                printf("Saindo do programa. Ate mais!\n");
                break;
            default:
                printf("Opcao invalida! Por favor, tente novamente.\n");
                break;
        }
    } while (choice != 2);
}

int main() {
    main_menu();
    return 0;
}