#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_ID 50
#define MAX_STR_VAL 16
#define MAX_LINE 256

typedef enum {
    INT_T,
    FLT_T,
    BOOL_T,
    STR_T,
    UNK_T
} DType;

typedef struct {
    long ts;
    DType typ;
    union {
        int i_val;
        float f_val;
        bool b_val;
        char s_val[MAX_STR_VAL + 1];
    } val;
} Rd;

typedef struct {
    char id[MAX_ID];
    DType typ;
    Rd *rds;
    int cnt;
    int cap;
} Snsr;

DType dt_typ(const char *v) {
    if (strcmp(v, "true") == 0 || strcmp(v, "false") == 0) return BOOL_T;
    char *end;
    strtol(v, &end, 10);
    if (*end == '\0') return INT_T;
    strtof(v, &end);
    if (*end == '\0') return FLT_T;
    if (strlen(v) <= MAX_STR_VAL) return STR_T;
    return UNK_T;
}

void proc_val(Rd *r, const char *v_str) {
    switch (r->typ) {
        case INT_T: r->val.i_val = (int)strtol(v_str, NULL, 10); break;
        case FLT_T: r->val.f_val = strtof(v_str, NULL); break;
        case BOOL_T: r->val.b_val = (strcmp(v_str, "true") == 0); break;
        case STR_T: strncpy(r->val.s_val, v_str, MAX_STR_VAL); r->val.s_val[MAX_STR_VAL] = '\0'; break;
        default: break;
    }
}

int cmp_rds(const void *a, const void *b) {
    const Rd *r_a = (const Rd *)a;
    const Rd *r_b = (const Rd *)b;
    if (r_a->ts < r_b->ts) return -1;
    if (r_a->ts > r_b->ts) return 1;
    return 0;
}

void wr_rds_file(const char *f_name, const Snsr *s_data) {
    FILE *f = fopen(f_name, "w");
    if (f == NULL) {
        perror("Erro ao abrir arquivo de saida");
        return;
    }
    for (int i = 0; i < s_data->cnt; i++) {
        fprintf(f, "%ld ", s_data->rds[i].ts);
        switch (s_data->typ) {
            case INT_T: fprintf(f, "%d\n", s_data->rds[i].val.i_val); break;
            case FLT_T: fprintf(f, "%.6f\n", s_data->rds[i].val.f_val); break;
            case BOOL_T: fprintf(f, "%s\n", s_data->rds[i].val.b_val ? "true" : "false"); break;
            case STR_T: fprintf(f, "%s\n", s_data->rds[i].val.s_val); break;
            default: break;
        }
    }
    fclose(f);
}

void organizar_dados() {
    char input_file_name[MAX_LINE];
    printf("Digite o nome do arquivo de entrada: ");
    scanf("%s", input_file_name);

    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    FILE *in_f = fopen(input_file_name, "r");
    if (in_f == NULL) {
        perror("Erro ao abrir arquivo de entrada");
        return;
    }

    Snsr *snsrs = NULL;
    int tot_snsrs = 0;
    int cap_snsrs = 0;

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), in_f)) {
        long ts_v;
        char id_s[MAX_ID];
        char val_s[MAX_LINE];

        if (sscanf(line, "%ld %s %s", &ts_v, id_s, val_s) != 3) {
            fprintf(stderr, "Aviso: Linha mal formatada ignorada: %s", line);
            continue;
        }

        DType d_typ = dt_typ(val_s);
        if (d_typ == UNK_T) {
            fprintf(stderr, "Aviso: Tipo de dado desconhecido para o valor '%s' do sensor '%s'. Linha ignorada.\n", val_s, id_s);
            continue;
        }

        int s_idx = -1;
        for (int i = 0; i < tot_snsrs; i++) {
            if (strcmp(snsrs[i].id, id_s) == 0) {
                s_idx = i;
                if (snsrs[i].typ != d_typ) {
                    fprintf(stderr, "Erro: Tipo de dado inconsistente para o sensor '%s'. Abortando.\n", id_s);
                    for (int j = 0; j < tot_snsrs; j++) free(snsrs[j].rds);
                    free(snsrs);
                    fclose(in_f);
                    return;
                }
                break;
            }
        }

        if (s_idx == -1) {
            if (tot_snsrs >= cap_snsrs) {
                cap_snsrs = (cap_snsrs == 0) ? 2 : cap_snsrs * 2;
                Snsr *tmp = realloc(snsrs, cap_snsrs * sizeof(Snsr));
                if (tmp == NULL) {
                    perror("Erro ao realocar memoria para sensores");
                    for (int j = 0; j < tot_snsrs; j++) free(snsrs[j].rds);
                    free(snsrs);
                    fclose(in_f);
                    return;
                }
                snsrs = tmp;
            }
            s_idx = tot_snsrs++;
            strncpy(snsrs[s_idx].id, id_s, MAX_ID);
            snsrs[s_idx].id[MAX_ID - 1] = '\0';
            snsrs[s_idx].typ = d_typ;
            snsrs[s_idx].rds = NULL;
            snsrs[s_idx].cnt = 0;
            snsrs[s_idx].cap = 0;
        }

        Snsr *curr_s = &snsrs[s_idx];
        if (curr_s->cnt >= curr_s->cap) {
            curr_s->cap = (curr_s->cap == 0) ? 10 : curr_s->cap * 2;
            Rd *tmp_rds = realloc(curr_s->rds, curr_s->cap * sizeof(Rd));
            if (tmp_rds == NULL) {
                perror("Erro ao realocar memoria para leituras do sensor");
                for (int j = 0; j < tot_snsrs; j++) free(snsrs[j].rds);
                free(snsrs);
                fclose(in_f);
                return;
            }
            curr_s->rds = tmp_rds;
        }

        Rd new_rd;
        new_rd.ts = ts_v;
        new_rd.typ = d_typ;
        proc_val(&new_rd, val_s);
        curr_s->rds[curr_s->cnt++] = new_rd;
    }

    fclose(in_f);

    for (int i = 0; i < tot_snsrs; i++) {
        qsort(snsrs[i].rds, snsrs[i].cnt, sizeof(Rd), cmp_rds);
        char out_f_name[MAX_ID + 5];
        snprintf(out_f_name, sizeof(out_f_name), "%s.txt", snsrs[i].id);
        wr_rds_file(out_f_name, &snsrs[i]);
        free(snsrs[i].rds);
    }
    free(snsrs);

    printf("Dados organizados com sucesso!\n");
}

void main_menu() {
    int choice;
    do {
        printf("\n--- Menu Principal ---\n");
        printf("1. Organizar Dados\n");
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
                organizar_dados();
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