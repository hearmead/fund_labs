#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_ITER_SUM  1000000
#define MAX_ITER_INT  25
#define BUF_SIZE      256

typedef enum {
    STATUS_OK = 0,
    STATUS_ERR_ARGC,
    STATUS_ERR_NULL,
    STATUS_ERR_PARSE,
    STATUS_ERR_EPSILON,
    STATUS_ERR_X_RANGE,
    STATUS_ERR_CONVERGENCE,
    STATUS_ERR_DOMAIN
} Status;

static int dbl_less(double a, double b, double eps) {
    return a < b - eps;
}
static int dbl_geq(double a, double b, double eps) {
    return !dbl_less(a, b, eps);
}

Status parse_double(const char *str, double *out) {
    if (!str || !out) return STATUS_ERR_NULL;
    char *end = NULL;
    double val = strtod(str, &end);
    if (end == str || *end != '\0') return STATUS_ERR_PARSE;
    *out = val;
    return STATUS_OK;
}

Status validate_epsilon(double eps) {
    if (eps <= 0.0 || eps >= 1.0) return STATUS_ERR_EPSILON;
    return STATUS_OK;
}

Status sum_a(double x, double eps, double *res) {
    if (!res) return STATUS_ERR_NULL;
    double term = 1.0;
    double sum  = term;
    int n = 0;
    while (fabs(term) >= eps) {
        ++n;
        term *= x / n;
        sum  += term;
        if (n > MAX_ITER_SUM) return STATUS_ERR_CONVERGENCE;
    }
    *res = sum;
    return STATUS_OK;
}

Status sum_b(double x, double eps, double *res) {
    if (!res) return STATUS_ERR_NULL;
    double term = 1.0;
    double sum  = term;
    int n = 0;
    while (fabs(term) >= eps) {
        ++n;
        term *= -x * x / ((2.0 * n - 1.0) * (2.0 * n));
        sum  += term;
        if (n > MAX_ITER_SUM) return STATUS_ERR_CONVERGENCE;
    }
    *res = sum;
    return STATUS_OK;
}

Status sum_c(double x, double eps, double *res) {
    if (!res) return STATUS_ERR_NULL;
    if (dbl_geq(fabs(x), 1.0, eps)) return STATUS_ERR_X_RANGE;

    double term = 1.0;
    double sum  = term;
    int n = 0;
    while (fabs(term) >= eps) {
        ++n;
        term *= 27.0 * n * n * n * x * x /
                ((3.0 * n - 2.0) * (3.0 * n - 1.0) * (3.0 * n));
        sum  += term;
        if (n > MAX_ITER_SUM) return STATUS_ERR_CONVERGENCE;
    }
    *res = sum;
    return STATUS_OK;
}

Status sum_d(double x, double eps, double *res) {
    if (!res) return STATUS_ERR_NULL;
    if (dbl_less(1.0, fabs(x), eps)) return STATUS_ERR_X_RANGE;

    double term = -x * x / 2.0;
    double sum  = term;
    int n = 1;
    while (fabs(term) >= eps) {
        ++n;
        term *= -(2.0 * n - 1.0) * x * x / (2.0 * n);
        sum  += term;
        if (n > MAX_ITER_SUM) return STATUS_ERR_CONVERGENCE;
    }
    *res = sum;
    return STATUS_OK;
}

static double f_a(double x) {
    if (fabs(x) < 1e-12) return 1.0;
    return log(1.0 + x) / x;
}
static double f_b(double x) {
    return exp(-x * x / 2.0);
}
static double f_c(double x) {
    return -log(1.0 - x);
}
static double f_d(double x) {
    if (x <= 0.0) return 1.0;
    return pow(x, x);
}

Status integrate_simpson(double (*f)(double), double a, double b,
                         double eps, double *res) {
    if (!f || !res) return STATUS_ERR_NULL;
    if (eps <= 0.0 || eps >= 1.0) return STATUS_ERR_EPSILON;

    int N = 10;
    double h = (b - a) / N;

    double sum = f(a) + f(b);
    for (int i = 1; i < N; ++i)
        sum += (i % 2 == 0 ? 2.0 : 4.0) * f(a + i * h);
    double I_prev = sum * h / 3.0;

    for (int iter = 0; iter < MAX_ITER_INT; ++iter) {
        N *= 2;
        h = (b - a) / N;
        sum = f(a) + f(b);
        for (int i = 1; i < N; ++i)
            sum += (i % 2 == 0 ? 2.0 : 4.0) * f(a + i * h);
        double I_curr = sum * h / 3.0;

        if (fabs(I_curr - I_prev) < eps) {
            *res = I_curr;
            return STATUS_OK;
        }
        I_prev = I_curr;
    }
    return STATUS_ERR_CONVERGENCE;
}

Status integrate_c(double eps, double *res) {
    if (!res) return STATUS_ERR_NULL;
    if (eps <= 0.0 || eps >= 1.0) return STATUS_ERR_EPSILON;

    const double delta = 1e-10;
    double part;
    Status st = integrate_simpson(f_c, 0.0, 1.0 - delta, eps, &part);
    if (st != STATUS_OK) return st;

    double tail = delta - delta * log(delta);
    *res = part + tail;
    return STATUS_OK;
}

Status print_sums(double x, double eps) {
    double ra, rb, rc, rd;
    Status st;

    st = sum_a(x, eps, &ra);
    if (st == STATUS_OK) printf("5.1 a) sum = %.10f\n", ra);
    else                 printf("5.1 a) error: %d\n", (int)st);

    st = sum_b(x, eps, &rb);
    if (st == STATUS_OK) printf("5.1 b) sum = %.10f\n", rb);
    else                 printf("5.1 b) error: %d\n", (int)st);

    st = sum_c(x, eps, &rc);
    if (st == STATUS_OK) printf("5.1 c) sum = %.10f\n", rc);
    else                 printf("5.1 c) error: %d\n", (int)st);

    st = sum_d(x, eps, &rd);
    if (st == STATUS_OK) printf("5.1 d) sum = %.10f\n", rd);
    else                 printf("5.1 d) error: %d\n", (int)st);

    return STATUS_OK;
}

Status print_integrals(double eps) {
    double ia, ib, ic, id;
    Status st;

    st = integrate_simpson(f_a, 0.0, 1.0, eps, &ia);
    if (st == STATUS_OK) printf("5.2 a) integral = %.10f\n", ia);
    else                 printf("5.2 a) error: %d\n", (int)st);

    st = integrate_simpson(f_b, 0.0, 1.0, eps, &ib);
    if (st == STATUS_OK) printf("5.2 b) integral = %.10f\n", ib);
    else                 printf("5.2 b) error: %d\n", (int)st);

    st = integrate_c(eps, &ic);
    if (st == STATUS_OK) printf("5.2 c) integral = %.10f\n", ic);
    else                 printf("5.2 c) error: %d\n", (int)st);

    st = integrate_simpson(f_d, 0.0, 1.0, eps, &id);
    if (st == STATUS_OK) printf("5.2 d) integral = %.10f\n", id);
    else                 printf("5.2 d) error: %d\n", (int)st);

    return STATUS_OK;
}

int main(int argc, char *argv[]) {
    double x = 0.0, eps = 0.0;
    Status st;

    if (argc == 2) {
        st = parse_double(argv[1], &eps);
        if (st != STATUS_OK) {
            fprintf(stderr, "Ошибка парсинга eps\n");
            return 1;
        }
        st = validate_epsilon(eps);
        if (st != STATUS_OK) {
            fprintf(stderr, "eps должен быть в (0, 1)\n");
            return 1;
        }
        printf("Задание 5.2\n");
        print_integrals(eps);
    }
    else if (argc == 3) {
        st = parse_double(argv[1], &x);
        if (st != STATUS_OK) {
            fprintf(stderr, "Ошибка парсинга x\n");
            return 1;
        }
        st = parse_double(argv[2], &eps);
        if (st != STATUS_OK) {
            fprintf(stderr, "Ошибка парсинга eps\n");
            return 1;
        }
        st = validate_epsilon(eps);
        if (st != STATUS_OK) {
            fprintf(stderr, "eps должен быть в (0, 1)\n");
            return 1;
        }
        printf("Задание 5.1, x = %g, eps = %g\n", x, eps);
        print_sums(x, eps);
        printf("\n=== Задание 5.2, eps = %g\n", eps);
        print_integrals(eps);
    }
    else {
        fprintf(stderr, "Использование:\n");
        fprintf(stderr, "  %s <eps> — только интегралы\n", argv[0]);
        fprintf(stderr, "  %s <x> <eps> — суммы + интегралы\n", argv[0]);
        return 1;
    }

    return 0;
}