#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef enum {
    STATUS_OK = 0,
    STATUS_ERR_ARGC,
    STATUS_ERR_FLAG,
    STATUS_ERR_FILE,
    STATUS_ERR_MEMORY
} Status;

static int is_space(int c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f';
}

static char *read_lexeme(FILE *f) {
    int c;
    while ((c = fgetc(f)) != EOF && is_space(c)) { }
    if (c == EOF) return NULL;

    size_t cap = 32, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;

    while (c != EOF && !is_space(c)) {
        if (len + 1 >= cap) {
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
        buf[len++] = (char)c;
        c = fgetc(f);
    }
    buf[len] = '\0';
    return buf;
}

static Status interleave(const char *path1, const char *path2, const char *pathout) {
    FILE *f1 = fopen(path1, "r");
    if (!f1) return STATUS_ERR_FILE;
    FILE *f2 = fopen(path2, "r");
    if (!f2) { fclose(f1); return STATUS_ERR_FILE; }
    FILE *fo = fopen(pathout, "w");
    if (!fo) { fclose(f1); fclose(f2); return STATUS_ERR_FILE; }

    int first = 1;
    while (1) {
        char *lex1 = read_lexeme(f1);
        char *lex2 = read_lexeme(f2);

        if (!lex1 && !lex2) break;

        if (lex1) {
            if (!first) fputc(' ', fo);
            fputs(lex1, fo);
            first = 0;
            free(lex1);
        }
        if (lex2) {
            if (!first) fputc(' ', fo);
            fputs(lex2, fo);
            first = 0;
            free(lex2);
        }
    }

    fclose(f1);
    fclose(f2);
    fclose(fo);
    return STATUS_OK;
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s -r <file1> <file2> <output>\n", argv[0]);
        return STATUS_ERR_ARGC;
    }

    const char *flag = argv[1];
    if (!(flag[0] == '-' || flag[0] == '/') || flag[1] != 'r' || flag[2] != '\0') {
        fprintf(stderr, "Unknown flag\n");
        return STATUS_ERR_FLAG;
    }

    Status st = interleave(argv[2], argv[3], argv[4]);
    if (st != STATUS_OK) {
        fprintf(stderr, "Error: %d\n", (int)st);
        return (int)st;
    }
    return STATUS_OK;
}