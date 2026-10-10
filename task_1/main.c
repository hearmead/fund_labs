#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <string.h>



int HandlerFlagH(const long long x);
int HandlerFlagP(const long long x);
int HandlerFlagS(const long long x);
int HandlerFlagE(const long long x);
int HandlerFlagA(const long long x);
int HandlerFlagF(const long long x);
unsigned long long fact(const long long x);
bool isPrime(const long long x);


int main(int argc, char* argv[]){
    if(argc != 3){
        fprintf(stderr, "Неверное количество аргументов\n");
        fprintf(stderr, "использование: %s <число> <флаг>\n", argv[0]);
        return 1;
    }

    if(strlen(argv[2]) != 2 || (argv[2][0] != '-' &&
              argv[2][0] != '/')){
                fprintf(stderr, "флаг должен начинаться с '-' или '/' и содержать один симмвол\n");
                return 1;
    }
    
    
    char *endptr;
    long long  x = strtoll(argv[1], &endptr, 10);
    if(*endptr != '\0' || strlen(argv[1]) == 0){
        fprintf(stderr, "'%s' не является корректным числом\n", argv[1]);
        return 1;
    }

    char flag = argv[2][1];
    int status = 0;

    switch(flag){
        case 'h':
            status = HandlerFlagH(x);
            break;

        case 'p':
            status = HandlerFlagP(x);
            break;
        
        case 's':
            status = HandlerFlagS(x);
            break;
        
        case 'e':
            status = HandlerFlagE(x);
            break;
        case 'a':
            status = HandlerFlagA(x);
            break;
        
        case 'f':
            status = HandlerFlagF(x);
            break;
        default: 
            fprintf(stderr, "неизвестный флаг '%c'\n", flag);
            return 1;
    }

    if(status != 0){
        return status;
    }
    return 0;
}


int HandlerFlagH(const long long x){
    if(x == 0 || x < 0) {
        fprintf(stderr, "x должен быть положительный и не равен 0\n");
        return 1;
    }
    long long i = 1;
    int found = 0;

    while(i <= 100){
        if(i % x == 0) {
            printf("%lld\n", i);
            found = 1;
        }
        i++;
    }
    if(!found){
        fprintf(stderr, "натуральных чисел кратных %lld нет\n", x);
    }

    return 0;
}


int HandlerFlagP(const long long x){

    
    if(x < 0){
        fprintf(stderr, "x - не может быть отрицательным для флага -p\n");
        return 1;
    }
    else if(x <= 1){
        printf("%lld не является простым и не является составным\n", x);
        return 0;
    }
    else if(isPrime(x)){
        printf("%lld - простое\n", x);
    }
    else{
        printf("%lld - составное\n", x);
    }

    return 0;

}

bool isPrime(const long long x){

    if(x <= 1){ return false; }
    if(x == 2){return true;}
    if(x % 2 == 0){ return false; }
    
    for(int i = 3; i * i <= x; i+=2){
        if(x % i == 0){
            return false;
        }
    }
    return true;
}

int HandlerFlagS(const long long x){
    if(x < 0){
        fprintf(stderr, "ошибка: x должен быть положительным\n");
        return 1;
    }
    
    char buffer[32];
    sprintf(buffer, "%llX", x);

    for(int i = 0; buffer[i] != '\0'; i++){
        printf("%c ", buffer[i]);
    }
    printf("\n");
    return 0;
}


int HandlerFlagE(const long long x){
    if(x > 10 || x < 1){
        fprintf(stderr, "x должен быть в диапазоне от 1 до 10\n");
        return 1;
    }
    else{
        for(int base = 1; base <= 10; base++){
        printf("степени числа %d(от 1 до %lld): ", base, x);
        for(int power = 1; power <= x; power++){
            long long result = 1;
            for(int k = 0; k < power; k++){
                result *= base;
            }
            printf("%lld ", result);
        }
        printf("\n");
        }

    }
    return 0;
}


int HandlerFlagA(const long long x){
    long long result = 0;
    if(x <= 0){
        fprintf(stderr, "ошибка: x должен быть положительным и не равным 0\n");
        return 1;
    }
    else{
        for(int i = 1; i <= x; i++){
            result += i;
        }
    }
    printf("%lld\n", result);
    return 0;
}

int HandlerFlagF(const long long x){
    if(x < 0){
        fprintf(stderr, "ошибка: x должен быть положительный\n");
        return 1;
    }
    else if(x > 20){
        fprintf(stderr, "ошибка, x слишком велик\n");
        return 1;
    }
    unsigned long long fact_x = fact(x);
    printf("%llu\n", fact_x);
    return 0;
}

unsigned long long fact(const long long x){
    if(x == 0 || x == 1) {return 1;}
    else{
        return x * fact(x - 1);
    }
}