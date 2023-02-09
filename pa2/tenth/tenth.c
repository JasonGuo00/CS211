#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

double power(int num, int exponent) {
    double product = num;
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

double negPower(int exponent){
    return ((1.0) / (power(2, (-1) * exponent)));
}

void hex2bin(char* hex, int numBits, char* bin) {
    for(int i = 2; i < strlen(hex); i++) {
        switch(hex[i]) {
            case '0':
                strcat(bin, "0000\0");
                break;
            case '1':
                strcat(bin, "0001\0");
                break;
            case '2':
                strcat(bin, "0010\0");
                break;
            case '3':
                strcat(bin, "0011\0");
                break;
            case '4':
                strcat(bin, "0100\0");
                break;
            case '5':
                strcat(bin, "0101\0");
                break;
            case '6':
                strcat(bin, "0110\0");
                break;
            case '7':
                strcat(bin, "0111\0");
                break;
            case '8':
                strcat(bin, "1000\0");
                break;
            case '9':
                strcat(bin, "1001\0");
                break;
            case 'A':
            case 'a':
                strcat(bin, "1010\0");
                break;
            case 'B':
            case 'b':
                strcat(bin, "1011\0");
                break;
            case 'C':
            case 'c':
                strcat(bin, "1100\0");
                break;
            case 'D':
            case 'd':
                strcat(bin, "1101\0");
                break;
            case 'E':
            case 'e':
                strcat(bin, "1110\0");
                break;
            case 'F':
            case 'f':
                strcat(bin, "1111\0");
                break;
        }
    }
}

int bin2int(char* binary) {
    int cnt = 0;
    int sum = 0;
    for(int i = strlen(binary)-1; i >= 0; i--) {
        if(binary[i] == '1') {
            sum += power(2, cnt);
        }
        cnt++;
    }
    return sum;
}

double bin2dec(char* binary) {
    int cnt = 1;
    double sum = 0;
    for(int i = 0; i < strlen(binary); i++) {
        if(binary[i] == '1') {
            sum += negPower(-1 * cnt);
        }
        cnt++;
    }
    return sum;
}

void Convert(char* hex, int numBits, int expBits, int fractBits, int precision) {
    // Take the hexadecimal and convert it into a Binary to get our IEEE-754
    char* bin = malloc(((strlen(hex) - 2)*4) * sizeof(char) + 1);
    bin[0] = '\0';
    hex2bin(hex, ((strlen(hex) - 2)*4), bin);

    // Find the offset
    int offset = 0;
    if((strlen(hex)-2)*4 > numBits) {
        offset = (strlen(hex)-2)*4 - numBits;
    }

    // Grab the value of the signed bit
    char sign = bin[offset];
    offset++;
    // Grab the exponent binary
    char* exp = malloc(expBits * sizeof(char) + 1);
    exp[expBits] = '\0';
    int cnt = 0;
    for(int i = offset; i < strlen(bin) && cnt < expBits; i++) {
        exp[cnt] = bin[i];
        cnt++;
    }
    offset += expBits;
    // Grab the fraction binary
    char* fract = malloc(fractBits * sizeof(char) + 1);
    fract[fractBits] = '\0';
    int cnt2 = 0;
    for(int i = offset; i < strlen(bin) && cnt2 < fractBits; i++) {
        fract[cnt2] = bin[i];
        cnt2++;
    }

    // Convert exponent binary to a number
   int exponent = bin2int(exp);
   int bias = power(2, expBits-1) - 1;
   exponent = exponent - bias;

   // Convert Mantissa to Decimal
   double decimal = bin2dec(fract);


   // Formula
   double answer;
   if(exponent < 0) {
        if(exponent + bias == 0) {
            // if denormalized
            answer = decimal * (negPower(1-bias));
        }
        else {
            answer = (1.0 + decimal) * (negPower(exponent));
        }
        
   }    
   else {
        answer = (1.0 + decimal) * (power(2, exponent));
   }
   
   if(sign == '1') {
        answer = -1 * answer;
   }

    printf("%.*f\n", precision, answer);
    free(bin);
    free(exp);
    free(fract);
}

int main(int argc, char* argv[]) {
    FILE * file;
    file = fopen(argv[1], "r");
    while(!feof(file)) {
        char buff1[255];
        char buff2[255];
        char buff3[255];
        char buff4[255];
        char buff5[255];
        fscanf(file, "%s %s %s %s %s\n", buff1, buff2, buff3, buff4, buff5);
        Convert(buff4, atoi(buff1), atoi(buff2), atoi(buff3), atoi(buff5));
    }
}