#include<stdio.h>
#include<stdlib.h>
#include<string.h>

struct Vertex {
    char name[255];
    struct Vertex* next;
    int weight;
};

// Inserts the vertex into the list (if it's not already there)
void initialize(struct Vertex* list[], int size, char v[]) {
    for(int i = 0; i < size; i++) {
        if(list[i] == NULL) {
            struct Vertex* src = malloc(sizeof(struct Vertex));
            list[i] = src;
            strcpy(list[i]->name, v);
            list[i]->next = NULL;
            break;
        }
        else if(strcmp(list[i]->name, v) == 0) {
            break;
        }
    }
}

void addAdjacency(struct Vertex* list[], int size, char v1[], char v2[], int wt) {
    int i = 0;
    while(i < size && list[i] != NULL) {
        // insert v2 at v1
        if(strcmp(list[i]->name, v1) == 0) {
            struct Vertex* ptr = list[i];
            // insert at front
            if(ptr->next != NULL && strcmp(ptr->next->name, v2) > 0) {
                struct Vertex* temp = malloc(sizeof(struct Vertex));
                strcpy(temp->name, v2);
                temp->weight = wt;
                temp->next = ptr->next;
                ptr->next = temp;
            }
            while(ptr->next != NULL) {
                // Handle Duplicates
                if(strcmp(ptr->name, v2) == 0) {
                    break;
                }
                // insert in middle
                if(ptr->next != NULL && strcmp(ptr->next->name, v2) > 0) {
                    struct Vertex* temp = malloc(sizeof(struct Vertex));
                    strcpy(temp->name, v2);
                    temp->weight = wt;
                    temp->next = ptr->next;
                    ptr->next = temp;
                    break;
                }
                ptr = ptr->next;
            }
            // insert at end
            if(strcmp(ptr->name, v2) != 0 && ptr->next == NULL) {
                struct Vertex* temp = malloc(sizeof(struct Vertex));
                strcpy(temp->name, v2);
                temp->weight = wt;
                temp->next = NULL;
                ptr->next = temp;
            }
        }
        i++;
    }
}

void adjacency(struct Vertex* list[], int size, char v[]) {
    for(int i = 0; i < size; i++) {
        if(list[i] != NULL && strcmp(list[i]->name, v) == 0) {
            struct Vertex* ptr = list[i];
            while(ptr != NULL) {
                if(ptr != list[i]) {
                    printf("%s" , ptr->name);
                    if(ptr->next != NULL) {
                        printf(" ");
                    }
                }
                ptr = ptr->next;
            }
            printf("\n");
            break;
        }
    }
}

void indegree(struct Vertex* list[], int size, char v[]) {
    int sum = 0;
    for(int i = 0; i < size; i++) {
        if(list[i] != NULL && strcmp(list[i]->name, v) != 0) {
            struct Vertex* ptr = list[i];
            while(ptr != NULL) {
                if(strcmp(ptr->name, v) == 0) {
                    sum++;
                }
                ptr = ptr->next;
            }
        }
    }
    printf("%d\n", sum);
}

void outdegree(struct Vertex* list[], int size, char v[]) {
    int sum = 0;
    for(int i = 0; i < size; i++) {
        if(list[i] != NULL && strcmp(list[i]->name, v) == 0) {
            struct Vertex* ptr = list[i];
            while(ptr != NULL) {
                sum++;
                ptr = ptr->next;
            }
            break;
        }
    }
    printf("%d\n", sum - 1);
}

void print(struct Vertex* list[], int size) {
    for(int i = 0; i < size; i++) {
        if(list[i] != NULL) {
            struct Vertex* ptr = list[i];
            while(ptr != NULL) {
                printf("%s", ptr->name);
                if(ptr->next != NULL) {
                    printf(" ");
                }
                ptr = ptr->next;
            }
            printf("\n");
        }
    }
}


int main(int argc, char* argv[]) {
    // Reading the First File
    if(argc == 0) {
        return 0;
    }
    
    FILE * file;
    file = fopen(argv[1], "r");
    char temp[100];
    fscanf(file, "%s", temp);
    int size = atoi(temp);

    // Initiate Adjacency Matrix - NULL
    struct Vertex* list[size];
    for(int i = 0; i < size; i++) {
        list[i] = NULL;
    }
    for(int i = 0; i < size; i++) {
        char buff[255];
        fscanf(file, "%s\n", buff);
        initialize(list, size, buff);
    }
    // Add Adjacency
    while(!feof(file)) {
        char buff1[100];
        char buff2[100];
        char buff3[100];
        fscanf(file, "%s %s %s\n", buff1, buff2, buff3);
        addAdjacency(list, size, buff1, buff2, atoi(buff3));
    }
    fclose(file);

    //Querying
    file = fopen(argv[2], "r");
    while(!feof(file)) {
        char buff1[100];
        char buff2[100];
        fscanf(file, "%s %s\n", buff1, buff2);
        if(strcmp(buff1, "o") == 0) {
            outdegree(list, size, buff2);
        }
        else if (strcmp(buff1, "i") == 0) {
            indegree(list, size, buff2);
        }
        else {
            adjacency(list, size, buff2);
        }
        
    }
    fclose(file);

    
    // Free Memory
    for(int x = 0; x < size; x++) {
        struct Vertex* ptr = list[x];
        while(ptr != NULL) {
            struct Vertex* temp = ptr;
            ptr = ptr->next;
            free(temp);
        }
    }
    return 0;

}