#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

enum Status {
    OK = 0,
    ERR_ARGS,
    ERR_FILE_OPEN,
    ERR_MEMORY,
    ERR_UNKNOWN_FLAG
};

enum Status process_d(FILE* in, FILE* out);
enum Status process_i(FILE* in, FILE* out);
enum Status process_s(FILE* in, FILE* out);
enum Status process_a(FILE* in, FILE* out);

int main(int argc, char *argv[]){

    if(argc < 3){
        fprintf(stderr, "недостаточное количество аргументов\n");
        fprintf(stderr, "использование: %s -<флаг> <input.txt> [output.txt]\n", argv[0]);
        return ERR_ARGS;
    }

    if(argv[1][0] != '-' && argv[1][0] != '/'){
        fprintf(stderr, "флаг должен начинаться с '-' или '/'\n");
        return ERR_ARGS;
    }

    char flag = argv[1][1];
    bool n_in_flag = false;
    if(flag == 'n'){
        n_in_flag = true;
        flag = argv[1][2];
        if(flag == '\0'){
            fprintf(stderr, "некорректный флаг: после 'n' нет символа\n");
            return ERR_ARGS;
        }
    }

    if(flag != 'd' && flag != 'i' && flag != 's' && flag != 'a'){
        fprintf(stderr, "неизвестный флаг '%c'\n", flag);
        return ERR_UNKNOWN_FLAG;
    }

    char* output_path = NULL;
    bool output_allocated = false;

    if(n_in_flag){
        if(argc != 4){
            fprintf(stderr, "для флага с 'n' требуется output_file\n");
            return ERR_ARGS;
        }
        output_path = argv[3];
    }
    else{
        if(argc != 3){
            fprintf(stderr, "слишком много аргументов\n");
            return ERR_ARGS;
        }

        size_t len = strlen("out_") + strlen(argv[2]) + 1;
        output_path = (char*)malloc(len);
        if(!output_path){
            fprintf(stderr, "ошибка выделения памяти\n");
            return ERR_MEMORY;
        }
        output_allocated = true;
        strcpy(output_path, "out_");
        strcat(output_path, argv[2]);
    }

    FILE* fin = fopen(argv[2], "r");
    if(!fin){
        fprintf(stderr, "невозможно открыть input_file '%s'\n", argv[2]);
        if(output_allocated) free(output_path);
        return ERR_FILE_OPEN;
    }

    FILE* fout = fopen(output_path, "w");
    if(!fout){
        fprintf(stderr, "невозможно открыть output_file '%s'\n", output_path);
        fclose(fin);
        if(output_allocated) free(output_path);
        return ERR_FILE_OPEN;
    }

    enum Status status = ERR_UNKNOWN_FLAG;
    switch(flag){
        case 'd': status = process_d(fin, fout); break;
        case 'i': status = process_i(fin, fout); break;
        case 's': status = process_s(fin, fout); break;
        case 'a': status = process_a(fin, fout); break;
        default:
            fprintf(stderr, "неизвестный флаг '%c'\n", flag);
            fclose(fin);
            fclose(fout);
            if(output_allocated) free(output_path);
            return ERR_UNKNOWN_FLAG;
    }

    if(status != OK){
        fprintf(stderr, "ошибка во время выполнения (code %d)\n", status);
    }

    fclose(fin);
    fclose(fout);
    if(output_allocated) free(output_path);

    return (status == OK) ? OK : 1;
}

enum Status process_d(FILE* in, FILE* out) {
    int c;
    while ((c = fgetc(in)) != EOF) {
        if (c < '0' || c > '9') {
            if (fputc(c, out) == EOF) {
                return ERR_FILE_OPEN;
            }
        }
    }
    return OK;
}

enum Status process_i(FILE* in, FILE* out) {
    char line[4096];

    while (fgets(line, sizeof(line), in)) {
        int count = 0;
        for (int i = 0; line[i] != '\0'; i++) {
            char c = line[i];
            if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
                count++;
            }
        }
        if (fprintf(out, "%d\n", count) < 0) {
            return ERR_FILE_OPEN;
        }
    }
    return OK;
}

enum Status process_s(FILE* in, FILE* out) {
    char line[4096];

    while (fgets(line, sizeof(line), in)) {
        int count = 0;
        for (int i = 0; line[i] != '\0'; i++) {
            char c = line[i];

            int is_latin = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
            int is_digit = (c >= '0' && c <= '9');
            int is_space = (c == ' ');

            if (!is_latin && !is_digit && !is_space) {
                count++;
            }
        }
        if (fprintf(out, "%d\n", count) < 0) {
            return ERR_FILE_OPEN;
        }
    }
    return OK;
}

enum Status process_a(FILE* in, FILE* out) {
    int c;
    while ((c = fgetc(in)) != EOF) {
        if (c >= '0' && c <= '9') {
            if (fputc(c, out) == EOF) return ERR_FILE_OPEN;
        } else {
            if (fprintf(out, "%X", (unsigned char)c) < 0) {
                return ERR_FILE_OPEN;
            }
        }
    }
    return OK;
}