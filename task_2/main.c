#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_ITER 100000000

static double e_series(double eps) {
    double term = 1.0, sum = 1.0;
    int n = 0;
    while (fabs(term) >= eps && n < MAX_ITER) {
        n++;
        term /= n;
        sum += term;
    }
    return sum;
}

static double e_limit(double eps) {
    double prev = 0.0;
    for (long n = 1; n < MAX_ITER; n *= 2) {
        double val = pow(1.0 + 1.0/n, (double)n);
        if (fabs(val - prev) < eps) return val;
        prev = val;
    }
    return prev;
}

static double e_equation(double eps) {
    double x = 2.0;
    for (int i = 0; i < 100; i++) {
        double xn = x - (log(x) - 1.0) * x;
        if (fabs(xn - x) < eps) return xn;
        x = xn;
    }
    return x;
}

static double pi_series(double eps) {
    double sum = 0.0, term = 1.0;
    long n = 0;
    while (fabs(term) >= eps && n < MAX_ITER) {
        sum += term;
        n++;
        term = (n % 2 == 1 ? -1.0 : 1.0) / (2.0*n + 1.0);
    }
    return 4.0 * sum;
}

static double pi_limit(double eps) {
    double prod = 1.0, prev = 0.0;
    for (long n = 1; n < MAX_ITER; n++) {
        prod *= (2.0*n)*(2.0*n) / ((2.0*n - 1.0)*(2.0*n + 1.0));
        double val = 2.0 * prod;
        if (fabs(val - prev) < eps) return val;
        prev = val;
    }
    return prev;
}

static double pi_equation(double eps) {
    double x = 3.14;
    for (int i = 0; i < 100; i++) {
        double xn = x - tan(x);
        if (fabs(xn - x) < eps) return xn;
        x = xn;
    }
    return x;
}

static double ln2_series(double eps) {
    double sum = 0.0, term = 1.0;
    int n = 0;
    while (fabs(term) >= eps && n < MAX_ITER) {
        n++;
        term = 1.0 / ((double)n * pow(2.0, n));
        sum += term;
    }
    return sum;
}

static double ln2_limit(double eps) {
    double prev = 0.0;
    for (long n = 1; n < MAX_ITER; n *= 2) {
        double val = n * (pow(2.0, 1.0/n) - 1.0);
        if (fabs(val - prev) < eps) return val;
        prev = val;
    }
    return prev;
}

static double ln2_equation(double eps) {
    double x = 0.5;
    for (int i = 0; i < 100; i++) {
        double ex = exp(x);
        double xn = x - (ex - 2.0) / ex;
        if (fabs(xn - x) < eps) return xn;
        x = xn;
    }
    return x;
}

static double sqrt2_series(double eps) {
    double term = 1.0, sum = 1.0;
    int n = 1;
    while (fabs(term) >= eps && n < MAX_ITER) {
        term *= (1.5 - (double)n) / (double)n;
        sum += term;
        n++;
    }
    return sum;
}

static double sqrt2_limit(double eps) {
    double x = -0.5;
    for (int i = 0; i < 10000; i++) {
        double xn = x - x*x/2.0 + 1.0;
        if (fabs(xn - x) < eps) return xn;
        x = xn;
    }
    return x;
}

static double sqrt2_equation(double eps) {
    double x = 1.5;
    for (int i = 0; i < 100; i++) {
        double xn = (x + 2.0/x) / 2.0;
        if (fabs(xn - x) < eps) return xn;
        x = xn;
    }
    return x;
}

static double gamma_limit(double eps) {
    long N = (long)(1.0/eps) + 100;
    if (N > MAX_ITER) N = MAX_ITER;
    double H = 0.0;
    for (long k = 1; k <= N; k++) H += 1.0/k;
    return H - log((double)N);
}

static double gamma_series(double eps) {
    long N = (long)(1.0/eps) + 100;
    if (N > MAX_ITER) N = MAX_ITER;
    double sum = 0.0;
    for (long k = 1; k <= N; k++) {
        sum += 1.0/k - log(1.0 + 1.0/k);
    }
    return sum;
}

static double gamma_euler_maclaurin(double eps) {
    (void)eps;
    long n = 1000000;
    double H = 0.0;
    for (long k = 1; k <= n; k++) H += 1.0/k;
    return H - log((double)n) - 1.0/(2.0*(double)n) + 1.0/(12.0*(double)n*n);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <eps>\n", argv[0]);
        return 1;
    }
    char *end;
    double eps = strtod(argv[1], &end);
    if (*end != '\0' || eps <= 0.0 || eps >= 1.0) {
        fprintf(stderr, "Invalid eps\n");
        return 1;
    }

    printf("e:\n");
    printf("  series:    %.10f\n", e_series(eps));
    printf("  limit:     %.10f\n", e_limit(eps));
    printf("  equation:  %.10f\n", e_equation(eps));

    printf("pi:\n");
    printf("  series:    %.10f\n", pi_series(eps));
    printf("  limit:     %.10f\n", pi_limit(eps));
    printf("  equation:  %.10f\n", pi_equation(eps));

    printf("ln 2:\n");
    printf("  series:    %.10f\n", ln2_series(eps));
    printf("  limit:     %.10f\n", ln2_limit(eps));
    printf("  equation:  %.10f\n", ln2_equation(eps));

    printf("sqrt(2):\n");
    printf("  series:    %.10f\n", sqrt2_series(eps));
    printf("  limit:     %.10f\n", sqrt2_limit(eps));
    printf("  equation:  %.10f\n", sqrt2_equation(eps));

    printf("gamma:\n");
    printf("  limit:     %.10f\n", gamma_limit(eps));
    printf("  series:    %.10f\n", gamma_series(eps));
    printf("  euler-mac: %.10f\n", gamma_euler_maclaurin(eps));

    return 0;
}