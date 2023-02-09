#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

int power(int num, int exponent) {
    int product = num;
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

int IntBits(int num) {
    int sum = 1;
    while(power(2,sum) <= num) {
        sum++;
    }
    return sum;
}

void convInt(int num, char* str) {
    int numBits = IntBits(num);
    char* binary = malloc(numBits * sizeof(char) + 1);
    binary[numBits*sizeof(char)] = '\0';
    // initialize the binary number as string
   for(int i = 0; i < numBits; i++) {
        binary[i] = '0';
   }
   // If positive
    for(int i = 0; num != 0; i++) {
        int exp = (numBits-1) - i;
        if(power(2,exp) > num) {
            binary[i] = '0';
        }
        else {
            binary[i] = '1';
            num = num - power(2, exp);
        }
    }
    char* final = malloc(numBits * sizeof(char) + 2);
    final[numBits+1] = '\0';
    int cnt = 0;
    for(int i = 0; i <= numBits; i++) {
        if(i == 1) {
            final[i] = '.';
        }
        else {
            final[i] = binary[cnt];
            cnt++;
        }
    }
    strcpy(str, final);
    free(binary);
    free(final);
}

void convFract(double fract, int precision, char* str) {
    char* binary = malloc(precision * sizeof(char) + 1);
    binary[precision] = '\0';
    for(int j = 0; j < precision; j++) {
        binary[j] = '0';
    }


    int i = 1;
    while(i <= precision) {
        if(negPower((-1)*i) <= fract) {
            binary[i-1] = '1';
            fract = fract - negPower((-1)*i);
        }
        i++;
    }
    strcpy(str, binary);
    free(binary);
}

void Convert(double num, int precision) {
    int x = num;
    double fract = num - x;
    char* intStr = malloc((IntBits(x) + precision) * sizeof(char) * 10);
    char* fractStr = malloc((precision) * sizeof(char) * 10);
    convInt(x, intStr);
    convFract(fract, precision * 5, fractStr);
    strcat(intStr, fractStr);
    // Check if leading is 0.M
    if(intStr[0] == '0') {
        // find the negative exponent, as well as the index of the first instance of 1
        int cnt = 1;
        int index = 0;
        for(int i = 2; i < strlen(intStr); i++) {
            if(intStr[i] == '1') {
                index = i;
                break;
            }
            else {
                cnt++;
            }
        }
        // create new string to represent the denormalized number
        char* temp = malloc(precision * sizeof(char) + 3);
        temp[precision+2] = '\0';
        for(int i = 0; i < precision+2; i++) {
            if(i == 1) {
                temp[i] = '.';
            }
            else {
                temp[i] = intStr[index];
                index++;
            }
            
        }
        printf("%s ", temp);
        printf("%d\n", (-1)*cnt);
        free(temp);
    }
    else {
        intStr[precision+2] = '\0';
        printf("%s ", intStr);
        printf("%d\n", IntBits(x)-1);
    }
    free(intStr);
    free(fractStr);
}

int main(int argc, char* argv[]) {
    FILE * file;
    file = fopen(argv[1], "r");
    while(!feof(file)) {
        char buff1[255];
        char buff2[255];
        fscanf(file, "%s %s\n", buff1, buff2);
        Convert(atof(buff1), atoi(buff2));
    }
}