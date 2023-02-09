#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

struct Vertex {
    char name[255];
    struct Vertex* next;
    int weight;
};

struct Mark {
    char name[255];
    bool marked;
};

void sort(struct Vertex* list[], int size) {
    bool sorted = false;
    while(!sorted) {
        sorted = true;
        for(int i = 0; i < size-1; i++) {
            if(strcmp(list[i]->name, list[i+1]->name) > 0) {
                // swap
                struct Vertex* tmp = list[i+1];
                list[i+1] = list[i];
                list[i] = tmp;
                sorted = false;
            }
        }
    }
    
}

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

// Marked List Methods

bool isMarked(struct Mark* markedList[], char target[], int size) {
    for(int i = 0; i < size; i++) {
        if(strcmp(markedList[i]->name, target) == 0) {
            return(markedList[i]->marked);
        }
    }
    return false;
}

void Mark(struct Mark* markedList[], char target[], int size) {
    for(int i = 0; i < size; i++) {
        if(strcmp(markedList[i]->name, target) == 0) {
            markedList[i]->marked = true;
        }
    }
}

void DFS(struct Vertex* list[], struct Mark* markedList[], char target[], int size) {
    Mark(markedList, target, size);
    bool check = false;
    for(int x = 0; x < size; x++) {
        if(!markedList[x]->marked) {
            check = true;
        }
    }
    printf("%s", target);
    if(check) {
        printf(" ");
    }
    for(int i = 0; i < size; i++) {
        if(strcmp(list[i]->name, target) == 0) {
            struct Vertex* ptr = list[i];
            while(ptr != NULL) {
                if(ptr != list[i] && !isMarked(markedList, ptr->name, size)) {
                    DFS(list, markedList, ptr->name, size);
                }
                ptr = ptr->next;
            }
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
    struct Mark* markedList[size];

    // Initiate Adjacency Matrix - NULL
    struct Vertex* list[size];
    for(int i = 0; i < size; i++) {
        list[i] = NULL;
        fscanf(file, "%s", temp);
        // initialize the graph
        initialize(list, size, temp);
    }
    sort(list, size);
    for(int i = 0; i < size; i++) {
        // initialize the marked array
        struct Mark* tmp = malloc(sizeof(struct Mark));
        tmp->marked = false;
        strcpy(tmp->name, list[i]->name);
        markedList[i] = tmp;
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

    // DFS
    for(int z = 0; z < size; z++) {
        if(!isMarked(markedList, list[z]->name, size)) {
            DFS(list, markedList, list[z]->name, size);
        }
    }
    printf("\n");
    
    // Free Memory
    for(int x = 0; x < size; x++) {
        struct Vertex* ptr = list[x];
        while(ptr != NULL) {
            struct Vertex* temp = ptr;
            ptr = ptr->next;
            free(temp);
        }
        free(markedList[x]);
    }
    return 0;

}