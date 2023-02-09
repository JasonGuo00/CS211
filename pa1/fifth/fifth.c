#include<stdlib.h>
#include<stdio.h>



int determinant(int n, int** matrix) {
    if(n == 1) {
        return matrix[0][0];
    }
    if(n == 2) {
        // ad - bc
        return (matrix[0][0]*matrix[1][1]) - (matrix[0][1]*matrix[1][0]);
    }
    //Recursively calculate the determinant
    int det = 0;
    for(int i = 1; i <= n; i++) {
        // Create sub matrix for each iteration
        int** sub = (int**)malloc((n-1) * sizeof(int*));
        for(int z = 0; z < n-1; z++) {
            sub[z] = (int*)malloc((n-1) * sizeof(int));
        }
        for(int x = 0; x < n-1; x++) {
            int yz = 0;
            for(int y = 0; y < n-1; y++) {
                if(yz == i-1) {
                    yz++;
                }
                // problem! sub[y] is going out of bounds
                sub[x][y] = matrix[x+1][yz];
                yz++;
            }
        }
        //Calculate det via recursion
        if(i % 2 == 0) {
            det += -1 * matrix[0][i-1] * determinant(n-1,sub);
        }
        else {
            det += matrix[0][i-1] * determinant(n-1,sub);
        }
        for(int q = 0; q < n-1; q++) {
            free(sub[q]);
        }
        free(sub);
        
    }
    return det;
}

int main(int argc, char* argv[]) {
    if(argc == 0) {
        return 0;
    }
    
    FILE * file;
    
    if((file = fopen(argv[1], "r"))) {
        // Scan for n, the size of the nxn matrix
        char buff1[100];
        char buff2[255];
        fscanf(file, "%s", buff1);
        int n = atoi(buff1);
        // Allocate matrix memory
        int** matrix = (int**)malloc((n) * sizeof(int*));
        for(int z = 0; z < n; z++) {
            matrix[z] = (int*)malloc((n) * sizeof(int));
        }
        // Fill matrix
        for(int x = 0; x < n; x++) {
            for(int y = 0; y < n; y++) {
                fscanf(file,"%s",buff2);
                matrix[x][y] = atoi(buff2);
            }
        }

        int ans = determinant(n, matrix);
        for(int i = 0; i < n; i++) {
            free(matrix[i]);
        }
        free(matrix);
        printf("%d\n", ans);

        fclose(file);

        return 0;

    }
    else {
        printf("error\n");
        return 0;
    }   
}

// int main() {
//     int** matrix = (int**)malloc((3) * sizeof(int*));
//     for(int z = 0; z < 3; z++) {
//         matrix[z] = (int*)malloc((3) * sizeof(int));
//     }
//     matrix[0][0] = 2;
//     matrix[0][1] = 7;
//     matrix[0][2] = 6;
//     matrix[1][0] = 9;
//     matrix[1][1] = 5;
//     matrix[1][2] = 1;
//     matrix[2][0] = 4;
//     matrix[2][1] = 3;
//     matrix[2][2] = 8;
//     int ans = determinant(3, matrix);
//     for(int i = 0; i < 3; i++) {
//         free(matrix[i]);
//     }
//     free(matrix);
//     printf("%d\n", ans);
// }