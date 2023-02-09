#include<stdio.h>
#include<stdlib.h>

// n has to be an odd number
void magicSquare(int n, int matrix[n][n]) {

    int x = 0;
    int y = n/2;

    //Shifting: either up and right or down
    //Shift down: only if matrix[x-1][y+1] != 0
    //Up and right: if row == -1, change to n-1 ; if column > n-1, change to 0
    //down: if row > n-1, change to 0

    for(int i = 1; i <= (n*n); i++) {
        if(i == 1) {
            matrix[x][y] = 1;
        }
        else {
            int new_x = x-1;
            int new_y = y+1;
            //handles border collisions
            if(x-1 == -1) {
                new_x = n-1;
            }
            if(y+1 > n-1) {
                new_y = 0;
            }
            //shift up and right
            if(matrix[new_x][new_y] == 0) {
                matrix[new_x][new_y] = i;
                x = new_x;
                y = new_y;
            }
            //shift down if up-right is occupied
            else {
                new_x = x+1;
                if(new_x > n-1) {
                    new_x = 0;
                }
                matrix[new_x][y] = i;
                x = new_x;
            }
            
        }
    }

    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            printf("%d", matrix[i][j]);
            if(j+1 < n) {
                printf("\t");
            }
        }
        printf("\n");
    }

}

int main(int argc, char* argv[]) {

    if(argc > 1) {
        int n = atoi(argv[1]);
        if(n % 2 == 0) {
            printf("error\n");
            return 0;
        }
        int matrix[n][n];
        for(int i = 0; i < n; i++) {
            for(int j = 0; j < n; j++) {
                matrix[i][j] = 0;
            }
        }
        magicSquare(n, matrix);
    }
    else {
        printf("error\n");
        return 0;
    }
    


    
}