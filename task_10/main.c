#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define STR_BUF 128

typedef enum {
    STATUS_OK = 0,
    STATUS_ERR_NULL,        
    STATUS_ERR_BASE,        
    STATUS_ERR_EMPTY,       
    STATUS_ERR_INVALID,     
    STATUS_ERR_OVERFLOW,    
    STATUS_ERR_BUFSIZE,     
    STATUS_ERR_NODATA,      
    STATUS_ERR_IO           
} Status;

static int char_to_digit(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'Z') return c - 'A' + 10;
    return -1;
}

static char digit_to_char(int d) {
    if (d < 0 || d > 35) return '?';
    if (d < 10) return (char)('0' + d);
    return (char)('A' + d - 10);
}


Status parse_in_base(const char *s, int base, long long *out) {
    if (!s || !out) return STATUS_ERR_NULL;
    if (base < 2 || base > 36) return STATUS_ERR_BASE;
    if (s[0] == '\0') return STATUS_ERR_EMPTY;

    int i = 0;
    int neg = 0;
    if (s[0] == '-') { neg = 1; i = 1; }
    if (s[i] == '\0') return STATUS_ERR_EMPTY;

    long long result = 0;
    for (; s[i] != '\0'; ++i) {
        int d = char_to_digit(s[i]);
        if (d < 0 || d >= base) return STATUS_ERR_INVALID;

        if (result > (LLONG_MAX - d) / base) return STATUS_ERR_OVERFLOW;
        result = result * base + d;
    }

    *out = neg ? -result : result;
    return STATUS_OK;
}

Status format_in_base(long long n, int base, char *out, size_t out_size) {
    if (!out || out_size == 0) return STATUS_ERR_NULL;
    if (base < 2 || base > 36) return STATUS_ERR_BASE;

    char tmp[STR_BUF];
    int i = 0;
    int neg = 0;
    unsigned long long un;

    if (n < 0) {
        neg = 1;
        un = (unsigned long long)(-(n + 1)) + 1ULL;
    } else {
        un = (unsigned long long)n;
    }

    if (un == 0) {
        tmp[i++] = '0';
    } else {
        while (un > 0 && i < STR_BUF - 1) {
            tmp[i++] = digit_to_char((int)(un % (unsigned)base));
            un /= (unsigned)base;
        }
    }

    size_t need = (size_t)i + (size_t)neg + 1;
    if (need > out_size) return STATUS_ERR_BUFSIZE;

    size_t k = 0;
    if (neg) out[k++] = '-';
    for (int j = i - 1; j >= 0; --j) out[k++] = tmp[j];
    out[k] = '\0';
    return STATUS_OK;
}

Status print_value_in_base(long long n, int base) {
    char buf[STR_BUF];
    Status st = format_in_base(n, base, buf, sizeof(buf));
    if (st != STATUS_OK) return st;
    printf("%s", buf);
    return STATUS_OK;
}

Status add_checked(long long a, long long b, long long *out) {
    if (!out) return STATUS_ERR_NULL;
    if ((b > 0 && a > LLONG_MAX - b) ||
        (b < 0 && a < LLONG_MIN - b)) {
        return STATUS_ERR_OVERFLOW;
    }
    *out = a + b;
    return STATUS_OK;
}

static unsigned long long abs_ll(long long v) {
    if (v < 0) return (unsigned long long)(-(v + 1)) + 1ULL;
    return (unsigned long long)v;
}

Status print_results(long long max_val, long long sum, int count) {
    const int bases[4] = { 9, 18, 27, 36 };

    printf("\nКоличество чисел: %d\n", count);

    printf("Максимум по модулю (10): ");
    if (print_value_in_base(max_val, 10) != STATUS_OK) return STATUS_ERR_IO;
    printf("\n");

    printf("Сумма (10): ");
    if (print_value_in_base(sum, 10) != STATUS_OK) return STATUS_ERR_IO;
    printf("\n");

    for (int i = 0; i < 4; ++i) {
        printf("Максимум по модулю (%d): ", bases[i]);
        if (print_value_in_base(max_val, bases[i]) != STATUS_OK)
            return STATUS_ERR_IO;
        printf("\n");

        printf("Сумма (%d): ", bases[i]);
        if (print_value_in_base(sum, bases[i]) != STATUS_OK)
            return STATUS_ERR_IO;
        printf("\n");
    }
    return STATUS_OK;
}

Status read_base(int *base) {
    if (!base) return STATUS_ERR_NULL;
    printf("Введите основание (2..36): ");
    if (scanf("%d", base) != 1) {
        int c; while ((c = getchar()) != '\n' && c != EOF) { }
        return STATUS_ERR_IO;
    }
    if (*base < 2 || *base > 36) return STATUS_ERR_BASE;
    return STATUS_OK;
}

Status process_numbers(int base, long long *max_val, long long *sum, int *count) {
    if (!max_val || !sum || !count) return STATUS_ERR_NULL;
    if (base < 2 || base > 36) return STATUS_ERR_BASE;

    long long local_max = 0;
    unsigned long long local_max_abs = 0;
    long long local_sum = 0;
    int local_count = 0;
    int has_max = 0;

    char buf[STR_BUF];
    printf("Вводите числа (Stop — окончание ввода):\n");

    while (scanf("%127s", buf) == 1) {
        if (strcmp(buf, "Stop") == 0) break;

        long long val;
        Status st = parse_in_base(buf, base, &val);
        if (st != STATUS_OK) {
            fprintf(stderr, "Пропущено невалидное число: %s (код %d)\n",
                    buf, (int)st);
            continue;
        }

        long long new_sum;
        if (add_checked(local_sum, val, &new_sum) != STATUS_OK) {
            fprintf(stderr, "Переполнение суммы, число пропущено: %s\n", buf);
            continue;
        }
        local_sum = new_sum;


        unsigned long long a = abs_ll(val);
        if (!has_max || a > local_max_abs) {
            local_max_abs = a;
            local_max = val;
            has_max = 1;
        }
        ++local_count;
    }

    if (local_count == 0) return STATUS_ERR_NODATA;

    *max_val = local_max;
    *sum = local_sum;
    *count = local_count;
    return STATUS_OK;
}

int main(void) {
    int base;
    Status st = read_base(&base);
    if (st != STATUS_OK) {
        fprintf(stderr, "Ошибка основания: код %d\n", (int)st);
        return 1;
    }

    long long max_val, sum;
    int count;
    st = process_numbers(base, &max_val, &sum, &count);
    if (st != STATUS_OK) {
        fprintf(stderr, "Ошибка обработки: код %d\n", (int)st);
        return 1;
    }

    st = print_results(max_val, sum, count);
    if (st != STATUS_OK) {
        fprintf(stderr, "Ошибка вывода: код %d\n", (int)st);
        return 1;
    }

    return 0;
}