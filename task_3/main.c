#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


int HandlerFlagM(const long long a, const long long b);
int HandlerFlagT(const double eps, const double a, const double b, const double c);
int HandlerFlagQ(const double eps, const double a, const double b, const double c);






int main(int argc, char *argv[]){
    if(argc < 3){
        fprintf(stderr, "Неверное количество аргументов\n");
        fprintf(stderr, "использование: %s <флаг> <число>\n", argv[0]);
        return 1;
    }

    if(strlen(argv[1]) != 2 || (argv[1][0] != '-' &&
              argv[1][0] != '/')){
                fprintf(stderr, "флаг должен начинаться с '-' или '/' и содержать один символ\n");
                return 1;
    }

    char flag = argv[1][1];
    int status = 0;

    switch(flag){
        case 'm':{
            if(argc != 4){
                fprintf(stderr, "недостаточное количество(или слишком много) аргументов для флага -m.\nиспользование: %s -m <число 1> <число 2>\n", argv[0]);
                return 1;
            }
            char *endptr1, *endptr2;
            long long a = strtoll(argv[2], &endptr1, 10);
            long long b = strtoll(argv[3], &endptr2, 10);

            if(*endptr1 != '\0' || *endptr2 != '\0'){
                fprintf(stderr, "ошибка: параметры должны быть целыми числами\n");
                return 1;
            }
            status = HandlerFlagM(a, b);
            break;
        }

        case 't':
        {
            if(argc != 6){
                fprintf(stderr, "недостаточное количество(или слишком много) аргументов для флага -t.\nиспользование: %s -t <eps> <a> <b> <c>\n", argv[0]);
                return 1;
            }

            char *endptr1, *endptr2, *endptr3, *endptr4;
            double eps = strtod(argv[2], &endptr1);
            double a = strtod(argv[3], &endptr2);
            double b = strtod(argv[4], &endptr3);
            double c = strtod(argv[5], &endptr4);

            if(*endptr1 != '\0' || *endptr2 != '\0' || *endptr3 != '\0' || *endptr4 != '\0'){
                fprintf(stderr, "ошибка: все аргументы должны быть вещественными числами\n");
                return 1;
            }
            status = HandlerFlagT(eps, a, b, c);
            break;
        }

        case 'q':{
            if(argc != 6){
                fprintf(stderr, "недостаточное количество(или слишком много) аргументов для флага -q.\nиспользование: %s -q <eps> <a> <b> <c>\n", argv[0]);
                return 1;
            }

            char *endptr1, *endptr2, *endptr3, *endptr4;
            double eps = strtod(argv[2], &endptr1);
            double a = strtod(argv[3], &endptr2);
            double b = strtod(argv[4], &endptr3);
            double c = strtod(argv[5], &endptr4);

            if(*endptr1 != '\0' || *endptr2 != '\0' || *endptr3 != '\0' || *endptr4 != '\0'){
                fprintf(stderr, "ошибка: все аргументы должны быть вещественными числами\n");
                return 1;
            }
            status = HandlerFlagQ(eps, a, b, c);
            break;
        }

        default:
            fprintf(stderr, "неизвестный флаг %c\n", flag);
            return 1;
    }

    if(status != 0){
        return status;
    }
    return 0;
}


bool almost_equal(const double a, const double b, const double eps){
    return fabs(a - b) < eps;
}

bool almost_less(const double a, const double b, const double eps) {
    return (b - a) > eps;
}

int HandlerFlagM(const long long a, const long long b){
    if(a == 0 || b == 0){
        fprintf(stderr, "числа должны быть ненулевые\n");
        return 1;
    }

    if(a % b == 0){
        printf("%lld кратно %lld\n", a, b);
    }
    else if(a % b != 0){
        printf("%lld не кратно %lld\n", a, b);
    }
    return 0;
}

int HandlerFlagT(const double eps, const double a, const double b, const double c){
    if(eps <= 0){
        fprintf(stderr, "ошибка: eps должен быть больше 0\n");
        return 1;
    }

    if(a <= 0 || b <= 0 || c <= 0){
        fprintf(stderr, "ошибка: длины сторон должны быть положительные\n");
        return 1;
    }

    if(almost_equal(a * a + b * b, c * c, eps)||
       almost_equal(a * a + c * c, b * b, eps)||
       almost_equal(b * b + c * c, a * a, eps)){
        printf("%g %g %g могут быть сторонами прямоугольного треугольника\n", a, b, c);
       }
    else{
        printf("%g %g %g не могут быть сторонами прямоугольного треугольника\n", a, b, c);
    }
    return 0;
}

int is_duplicate(const double eps, const double x, const double y, const double z,
                const double previous[][3], const int count){
    for(int i = 0; i < count; i++){
        if(almost_equal(x, previous[i][0], eps) &&
           almost_equal(y, previous[i][1], eps) &&
           almost_equal(z, previous[i][2], eps)){
            return 1;
           }
    }
    return 0;
}

void solve_one(const double eps, const double a, const double b, const double c) {
    if (almost_equal(a, 0.0, eps)) {
        if (almost_equal(b, 0.0, eps)) {
            if (almost_equal(c, 0.0, eps)) {
                printf("бесконечно много решений\n");
            } else {
                printf("нет решений\n");
            }
        } else {
            double x = -c / b;
            printf("линейное, x = %g\n", x);
        }
        return;
    }

    double D = b * b - 4 * a * c;

    if (almost_less(D, 0.0, eps)) {
        printf("    дискриминант < 0, корней нет\n");
    } else if (almost_equal(D, 0.0, eps)) {
        double x = -b / (2 * a);
        printf("    D = 0, x = %g\n", x);
    } else {
        double sq = sqrt(D);
        double x1 = (-b + sq) / (2 * a);
        double x2 = (-b - sq) / (2 * a);
        printf("    D > 0, x1 = %g, x2 = %g\n", x1, x2);
    }
}

int HandlerFlagQ(const double eps, const double a, const double b, const double c) {
    if (eps <= 0) {
        fprintf(stderr, "ошибка: eps должен быть больше 0\n");
        return 1;
    }

    double printed[6][3];
    int count = 0;

    if (!is_duplicate(eps, a, b, c, printed, count)) {
        printf("Уравнение %g*x^2 + %g*x + %g = 0:\n", a, b, c);
        solve_one(eps, a, b, c);
        printed[count][0] = a; printed[count][1] = b; printed[count][2] = c;
        count++;
    }

    if (!is_duplicate(eps, a, c, b, printed, count)) {
        printf("Уравнение %g*x^2 + %g*x + %g = 0:\n", a, c, b);
        solve_one(eps, a, c, b);
        printed[count][0] = a; printed[count][1] = c; printed[count][2] = b;
        count++;
    }

    if (!is_duplicate(eps, b, a, c, printed, count)) {
        printf("Уравнение %g*x^2 + %g*x + %g = 0:\n", b, a, c);
        solve_one(eps, b, a, c);
        printed[count][0] = b; printed[count][1] = a; printed[count][2] = c;
        count++;
    }

    if (!is_duplicate(eps, b, c, a, printed, count)) {
        printf("Уравнение %g*x^2 + %g*x + %g = 0:\n", b, c, a);
        solve_one(eps, b, c, a);
        printed[count][0] = b; printed[count][1] = c; printed[count][2] = a;
        count++;
    }

    if (!is_duplicate(eps, c, a, b, printed, count)) {
        printf("Уравнение %g*x^2 + %g*x + %g = 0:\n", c, a, b);
        solve_one(eps, c, a, b);
        printed[count][0] = c; printed[count][1] = a; printed[count][2] = b;
        count++;
    }

    if (!is_duplicate(eps, c, b, a, printed, count)) {
        printf("Уравнение %g*x^2 + %g*x + %g = 0:\n", c, b, a);
        solve_one(eps, c, b, a);
        printed[count][0] = c; printed[count][1] = b; printed[count][2] = a;
        count++;
    }

    return 0;
}