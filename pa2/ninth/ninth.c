#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<stdbool.h>
#include<limits.h>

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

void convIntOvr(int num, char* str, int numBits) {
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
    strcpy(str, binary);
    free(binary);
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

bool Round(char* intStr, char* mant, char* remaining) {
    char* v1 = malloc(strlen(mant) * sizeof(char)+3);
    char* v2 = malloc(strlen(mant) * sizeof(char)+3);
    bool flag1 = false;
    bool flag2 = false;
    // initialize v1
    v1[0] = '1';
    v1[1] = '.';
    v1[strlen(mant)+2] = '\0';
    for(int i = 2; i < strlen(mant)+2; i++) {
        v1[i] = mant[i-2];
    }

    // initialize v2
    strcpy(v2, v1);
    bool done = false;
    for(int i = strlen(v2)-1; i >= 1 && !done; i--) {
        if(i == 1) {
            // the rounding goes beyone the point - we need to round the entire thing form 1.M to 1.000...
            for(int j = 2; j < strlen(mant); j++) {
                v2[j] = '0';
                // if this occurs, that means the answer must have +1 to the exponent
                flag1 = true;
            }
        }
        else if(v2[i] == '0') {
            v2[i] = '1';
            done = true;
        }
        else {
            v2[i] = '0';
        }
    }
    // Get the midpoint
    char* midpoint = malloc(strlen(intStr) * sizeof(char) + 1);
    midpoint[strlen(intStr)] = '\0';
    for(int i = 0; i < strlen(intStr); i++) {
        if(i < strlen(v1)) {
            midpoint[i] = v1[i];
        }
        else if(i == strlen(v1)) {
            midpoint[i] = '1';
        }
        else {
            midpoint[i] = '0';
        }
        
    }

    // Compare the midpoint and the original 
    for(int i = 0; i < strlen(midpoint); i++) {
        if(midpoint[i] > intStr[i]) {
            break;
        }
        else if(midpoint[i] < intStr[i]) {
            for(int i = 2; i < strlen(mant) + 2; i++) {
                mant[i-2] = v2[i];
            }
            break;
        }
        else if(midpoint[i] == intStr[i] && (i == strlen(midpoint)-1)) {
            flag2 = true;
            if(v1[strlen(v1)-1] == '0') {
                break;
            }
            else {
                for(int i = 2; i < strlen(mant) + 2; i++) {
                    mant[i-2] = v2[i];
                }
                break;

            }
        }
    }
    // Done
    free(v2);
    free(v1);
    free(midpoint);
    if(flag1 && flag2) {
        return true;
    }
    else {
        return false;
    }
}

void Convert(double num, int expBits, int numBits) {
    bool negative = false;
    if(num < 0) {
        negative = true;
        num = -1 * num;
    }
    int x = num;
    double fract = num - x;
    char* intStr = malloc((IntBits(x) + numBits) * sizeof(char) * 10);
    convInt(x, intStr);
    // figure out how many bits necessary to represent
    int z = -1;
    while(negPower(z) > fract) {
        z--;
    }
    char* fractStr = malloc(((-1 * z) + (2 * numBits)) * sizeof(char) + 1);
    convFract(fract, (-1 * z) + (2 * numBits), fractStr);
    // Build full canonical representation
    strcat(intStr, fractStr);
    free(fractStr);

    // Exponent
    int exponent = 0;
    if(num == 0 || num >= 1 || num <= -1) {
        exponent = IntBits(x)-1;
    }


    char* mantissa = malloc(numBits * sizeof(char) + 1);
    char* remaining = malloc(numBits * sizeof(char) + 1);
    mantissa[numBits] = '\0';
    remaining[numBits] = '\0';
    // Mantissa for x > 1
    if(num >= 1 || num == 0 || num <= -1) {
        int cnt = 2;
        for(int i = 0; i < numBits; i++) {
            if(i < numBits) {
                mantissa[i] = intStr[cnt];
                cnt++; 
            }
        }
        for(int i = 0; (i < numBits) && (cnt < strlen(intStr)); i++) {
            remaining[i] = intStr[cnt];
            cnt++;
        }
    }
    // Mantissa for 0 < x < 1 or -1 < x < 0
    else {
        int cnt1 = 0;
        int cnt2 = 0;
        int cnt3 = 0;
        while(intStr[cnt1] != '1' && cnt1 < strlen(intStr)) {
            cnt1++;
        }
        // If denormalized
        // if(num < negPower(-1  * (power(2,expBits-1) - 2)))
        if((cnt1-1) > (power(2,expBits-1) - 2)) {
            for(int i = cnt1; i < strlen(intStr); i++) {
                if(cnt2 < numBits){
                mantissa[cnt2] = intStr[i];
                    cnt2++; 
                }
                else if(cnt2 >= numBits && cnt3 < numBits) {
                    remaining[cnt3] = intStr[i];
                    cnt3++;
                }
                
            }
            exponent = (-1)*(cnt1 -1);
        }
        // if normalized
        else {
            for(int i = cnt1+1; i < strlen(intStr); i++) {
                if(cnt2 < numBits){
                mantissa[cnt2] = intStr[i];
                    cnt2++; 
                }
                else if(cnt2 >= numBits && cnt3 < numBits) {
                    remaining[cnt3] = intStr[i];
                    cnt3++;
                }
                
            }
            exponent = (-1)*(cnt1 -1);
        }
    }
    // Performing rounding and adjustment

    if(Round(intStr, mantissa, remaining)) {
        exponent++;
    }

    if(negative) {
        printf("%d", 1);
    }
    else {
        printf("%d", 0);
    }
    int bias = power(2, expBits-1) - 1;
    char* expo = malloc(expBits * sizeof(char) + 1);
    convIntOvr(bias + exponent, expo, expBits);
    printf("%s", expo);
    printf("%s\n", mantissa);
    free(mantissa);
    free(expo);
    free(intStr);
    free(remaining);
}

// Need to do:

int main(int argc, char* argv[]) {
    FILE * file;
    file = fopen(argv[1], "r");
    while(!feof(file)) {
        char buff1[255];
        char buff2[255];
        char buff3[255];
        char buff4[255];
        fscanf(file, "%s %s %s %s\n", buff1, buff2, buff3, buff4);
        Convert(atof(buff1), atoi(buff3), atoi(buff4));
    }
}