#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    STATUS_OK = 0,
    STATUS_ERR_ARGC,
    STATUS_ERR_FILE,
    STATUS_ERR_MEMORY,
    STATUS_ERR_INVALID,
    STATUS_EOF
} Status;

static int char_to_val(int c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'Z') return c - 'A' + 10;
    if (c >= 'a' && c <= 'z') return c - 'a' + 10;
    return -1;
}

static int is_space(int c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f';
}

static Status read_token(FILE *f, char **out) {
    if (!f || !out) return STATUS_ERR_INVALID;
    int c;
    while ((c = fgetc(f)) != EOF && is_space(c)) { }
    if (c == EOF) return STATUS_EOF;

    size_t cap = 32, len = 0;
    char *buf = malloc(cap);
    if (!buf) return STATUS_ERR_MEMORY;

    while (c != EOF && !is_space(c)) {
        if (len + 1 >= cap) {
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return STATUS_ERR_MEMORY; }
            buf = tmp;
        }
        buf[len++] = (char)c;
        c = fgetc(f);
    }
    buf[len] = '\0';
    *out = buf;
    return STATUS_OK;
}

static Status trim_zeros(const char *src, char **out) {
    if (!src || !out) return STATUS_ERR_INVALID;
    size_t len = strlen(src);
    size_t i = 0;
    while (i < len - 1 && src[i] == '0') i++;
    const char *start = src + i;
    size_t new_len = strlen(start);
    char *res = malloc(new_len + 1);
    if (!res) return STATUS_ERR_MEMORY;
    strcpy(res, start);
    *out = res;
    return STATUS_OK;
}

static Status to_decimal(const char *token, int base, char **out) {
    if (!token || !out) return STATUS_ERR_INVALID;
    int *digits = malloc(64 * sizeof(int));
    if (!digits) return STATUS_ERR_MEMORY;
    size_t cap = 64, len = 1;
    digits[0] = 0;

    for (size_t i = 0; token[i] != '\0'; ++i) {
        int d = char_to_val(token[i]);
        if (d < 0 || d >= base) { free(digits); return STATUS_ERR_INVALID; }
        int carry = d;
        for (size_t j = 0; j < len; ++j) {
            int val = digits[j] * base + carry;
            digits[j] = val % 10;
            carry = val / 10;
        }
        while (carry > 0) {
            if (len == cap) {
                cap *= 2;
                int *tmp = realloc(digits, cap * sizeof(int));
                if (!tmp) { free(digits); return STATUS_ERR_MEMORY; }
                digits = tmp;
            }
            digits[len++] = carry % 10;
            carry /= 10;
        }
    }

    char *res = malloc(len + 1);
    if (!res) { free(digits); return STATUS_ERR_MEMORY; }
    for (size_t i = 0; i < len; ++i) {
        res[i] = (char)('0' + digits[len - 1 - i]);
    }
    res[len] = '\0';
    free(digits);
    *out = res;
    return STATUS_OK;
}

static Status process_token(const char *token, FILE *fout) {
    int max_val = 0;
    for (size_t i = 0; token[i] != '\0'; ++i) {
        int v = char_to_val(token[i]);
        if (v < 0) return STATUS_ERR_INVALID;
        if (v > max_val) max_val = v;
    }
    int base = max_val + 1;
    if (base < 2) base = 2;
    if (base > 36) return STATUS_ERR_INVALID;

    char *trimmed = NULL;
    Status st = trim_zeros(token, &trimmed);
    if (st != STATUS_OK) return st;

    char *dec = NULL;
    st = to_decimal(trimmed, base, &dec);
    if (st != STATUS_OK) { free(trimmed); return st; }

    fprintf(fout, "%s %d %s\n", trimmed, base, dec);

    free(trimmed);
    free(dec);
    return STATUS_OK;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input> <output>\n", argv[0]);
        return STATUS_ERR_ARGC;
    }

    FILE *fin = fopen(argv[1], "r");
    if (!fin) return STATUS_ERR_FILE;
    FILE *fout = fopen(argv[2], "w");
    if (!fout) { fclose(fin); return STATUS_ERR_FILE; }

    char *token = NULL;
    Status st;
    while ((st = read_token(fin, &token)) == STATUS_OK) {
        Status pst = process_token(token, fout);
        free(token);
        if (pst != STATUS_OK) {
            fclose(fin);
            fclose(fout);
            return pst;
        }
    }

    fclose(fin);
    fclose(fout);
    if (st != STATUS_EOF) return st;
    return STATUS_OK;
}