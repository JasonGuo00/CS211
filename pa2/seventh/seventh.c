#include<stdio.h>
#include<stdlib.h>
#include<string.h>

long long power(int num, int exponent) {
    long long product = num;
    if(exponent == 0) {
        return 1;
    }
    if(exponent == 1) {
        return num;
    }
    for(int i = 2; i <= exponent; i++) {
        product = product * num;
    }
    return product;
}

int bitsNecessary(long long num) {
    int cnt = 1;
    while(num > power(2,cnt)-1) {
        cnt++;
    }
    return cnt;
}

void ConvertTwos(long long num, int numBits) {
    char* binary = malloc(numBits * sizeof(char) + 1);
    binary[numBits*sizeof(char)] = '\0';
    // printf("%d\n", num);
    // initialize the binary number as string
   for(int i = 0; i < numBits; i++) {
        binary[i] = '0';
   }
   // If positive
   if(num >= 0) {
        // larger than largest positive:
        if(bitsNecessary(num) + 1 > numBits) {
            for(int i = 1; i < numBits; i++) {
                if(i != 0) {
                    binary[i] = '1';
                }
            }
        }
        else {
            for(int i = 1; num != 0; i++) {
                int exp = (numBits-1) - i;
                if(power(2,exp) > num) {
                    binary[i] = '0';
                }
                else {
                    binary[i] = '1';
                    num = num - power(2, exp);
                }
            }
        } 
   }
   else {
        binary[0] = '1';
        // smaller than smallest negative: 2^n -1
        if(num < (power(2,numBits-1)) * (-1)) {
            binary[0] = '1';
        }
        else {
            int neg = (power(2,(numBits-1))) * (-1);
            for(int i = 1; neg != num; i++) {
                int exp = power(2, (numBits-1)-i);
                if(neg + exp <= num) {
                    binary[i] = '1';
                    neg += exp;
                }
                else {
                    binary[i] = '0';
                }
            }
        } 
   }
   printf("%s\n", binary) ;
   free(binary);
}

int main(int argc, char* argv[]) {
    FILE * file;
    file = fopen(argv[1], "r");
    while(!feof(file)) {
        char buff1[255];
        char buff2[255];
        fscanf(file, "%s %s\n", buff1, buff2);
        // printf("%lli\n", atoll(buff1));
        ConvertTwos(atoll(buff1), atoi(buff2));
    }
}