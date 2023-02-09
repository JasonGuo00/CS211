#include<stdio.h>
#include<stdlib.h>

int isPrime(char* string) {
    int num = atoi(string);
    int prime = 1;
    while(num > 0) {
        for(int i = 2; i <= (num/2); i++) {
            if(num % i == 0) {
                prime = 0;
            }
        }
        if(prime == 0) {
            printf("no\n");
            return 0;
        }
        num = num / 10;
    }
    printf("yes\n");
    return 0;
}

int isPrime(char* string);

int main(int argc, char* argv[]) {
    if(argc == 0) {
        return 0;
    }
    
    FILE * file;
    file = fopen(argv[1], "r");

    char line[100];
    fgets(line, 100, file);
    int count = atoi(line);

    for(int i = 0; i < count; i++) {
        fgets(line,100,file);
        isPrime(line);
    }
    
    fclose(file);

    return 0;

}

