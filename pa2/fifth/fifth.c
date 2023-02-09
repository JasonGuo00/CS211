#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

struct Node *stack = NULL;

struct Vertex{
    char name[255];
    struct Vertex *next;
    int weight;
};

struct Mark{
    char name[255];
    bool marked;
};

struct Node{
    char name[255];
    struct Node *next;
};

struct Distance{
    char name[255];
    int distance;
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

// Stack Methods
void push(char val[], struct Node **stack){
    // If linked list is empty
    if (*stack == NULL){
        struct Node *temp = malloc(sizeof(struct Node));
        strcpy(temp->name, val);
        temp->next = NULL;
        *stack = temp;
    }
    else{
        // Insert at the head
        struct Node *ptr = *stack;
        // Insert val at the very head
        struct Node *temp2 = malloc(sizeof(struct Node));
        strcpy(temp2->name, val);
        temp2->next = ptr;
        *stack = temp2;
    }
}

void pop(struct Node **head, char ans[]){
    // Delete the head
    if (*head == NULL){
        return;
    }
    else{
        struct Node *temp = *head;
        strcpy(ans, temp->name);
        *head = (*head)->next;
        free(temp);
        return;
    }
}

bool isEmpty(struct Node **head){
    if (*head == NULL){
        return true;
    }
    return false;
}

// Inserts the vertex into the list (if it's not already there)
void initialize(struct Vertex *list[], int size, char v[]){
    for (int i = 0; i < size; i++){
        if (list[i] == NULL){
            struct Vertex *src = malloc(sizeof(struct Vertex));
            list[i] = src;
            strcpy(list[i]->name, v);
            list[i]->next = NULL;
            break;
        }
        else if (strcmp(list[i]->name, v) == 0){
            break;
        }
    }
}

void addAdjacency(struct Vertex *list[], int size, char v1[], char v2[], int wt){
    int i = 0;
    while (i < size && list[i] != NULL){
        // insert v2 at v1
        if (strcmp(list[i]->name, v1) == 0){
            struct Vertex *ptr = list[i];
            // insert at front
            if (ptr->next != NULL && strcmp(ptr->next->name, v2) > 0){
                struct Vertex *temp = malloc(sizeof(struct Vertex));
                strcpy(temp->name, v2);
                temp->weight = wt;
                temp->next = ptr->next;
                ptr->next = temp;
            }
            while (ptr->next != NULL){
                // Handle Duplicates
                if (strcmp(ptr->name, v2) == 0){
                    break;
                }
                // insert in middle
                if (ptr->next != NULL && strcmp(ptr->next->name, v2) > 0){
                    struct Vertex *temp = malloc(sizeof(struct Vertex));
                    strcpy(temp->name, v2);
                    temp->weight = wt;
                    temp->next = ptr->next;
                    ptr->next = temp;
                    break;
                }
                ptr = ptr->next;
            }
            // insert at end
            if (strcmp(ptr->name, v2) != 0 && ptr->next == NULL){
                struct Vertex *temp = malloc(sizeof(struct Vertex));
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

bool isMarked(struct Mark *markedList[], char target[], int size){
    for (int i = 0; i < size; i++){
        if (strcmp(markedList[i]->name, target) == 0){
            return (markedList[i]->marked);
        }
    }
    return false;
}

void Mark(struct Mark *markedList[], char target[], int size){
    for (int i = 0; i < size; i++){
        if (strcmp(markedList[i]->name, target) == 0){
            markedList[i]->marked = true;
        }
    }
}

void Unmark(struct Mark *markedList[], char target[], int size){
    for (int i = 0; i < size; i++){
        if (strcmp(markedList[i]->name, target) == 0){
            markedList[i]->marked = false;
        }
    }
}


void Reset(struct Mark *markedList[], int size) {
    for(int i = 0; i < size; i++) {
        markedList[i]->marked = false;
    }
}


// Topological Sort
void topological(struct Vertex *list[], struct Mark *markedList[], char target[], int size){
    Mark(markedList, target, size);
    for (int i = 0; i < size; i++){
        if (strcmp(list[i]->name, target) == 0){
            struct Vertex *ptr = list[i];
            while (ptr != NULL){
                if (ptr != list[i] && !isMarked(markedList, ptr->name, size)){
                    topological(list, markedList, ptr->name, size);
                }
                ptr = ptr->next;
            }
        }
    }
    push(target, &stack);
}

int getI(struct Distance *distanceList[], int size, char target[]){
    for (int i = 0; i < size; i++){
        if (strcmp(distanceList[i]->name, target) == 0){
            return i;
        }
    }
    return 0;
}

int weight(struct Vertex *list[], int size, char v[], char u[]){
    for (int i = 0; i < size; i++){
        if (strcmp(list[i]->name, v) == 0){
            struct Vertex *ptr = list[i];
            while (ptr != NULL){
                if (strcmp(ptr->name, u) == 0){
                    return ptr->weight;
                }
                ptr = ptr->next;
            }
        }
    }
    return 0;
}

void SSP(struct Vertex *list[], char src[], int size){
    struct Distance *distanceList[size];
    // Form the Distance array
    for (int i = 0; i < size; i++){
        struct Distance *temp = malloc(sizeof(struct Distance));
        distanceList[i] = temp;
        strcpy(distanceList[i]->name, list[i]->name);
        if (strcmp(list[i]->name, src) == 0){
            distanceList[i]->distance = 0;
        }
        else{
            distanceList[i]->distance = INT_MAX;
        }
    }
    // Query through the stack, assign shortest distance
    while (!isEmpty(&stack)){
        char v[255];
        pop(&stack, v);
        for (int i = 0; i < size; i++){
            if (strcmp(list[i]->name, v) == 0){
                struct Vertex *ptr = list[i];
                while (ptr != NULL){
                    if (ptr != list[i]){
                        int distv = distanceList[getI(distanceList, size, v)]->distance;
                        int distu = distanceList[getI(distanceList, size, ptr->name)]->distance;
                        int weight_vu = weight(list, size, v, ptr->name);
                        if (distu > distv + weight_vu){
                            if (distv != INT_MAX){
                                distanceList[getI(distanceList, size, ptr->name)]->distance = distv + weight_vu;
                            }
                        }
                    }
                    ptr = ptr->next;
                }
            }
        }
    }

    for (int j = 0; j < size; j++){
        if(distanceList[j]->distance == INT_MAX) {
            printf("%s %s\n", distanceList[j]->name, "INF");
        }
        else {
            printf("%s %d\n", distanceList[j]->name, distanceList[j]->distance);
        }
        free(distanceList[j]);
    }
    printf("\n");
}

bool detectCycle(struct Vertex* list[], struct Mark* recList[], struct Mark* markedList[], char target[], int size) {
    if(!isMarked(markedList, target, size)) {
        Mark(markedList, target, size);
        Mark(recList, target, size);

        for (int i = 0; i < size; i++){
            if (strcmp(list[i]->name, target) == 0){
                struct Vertex *ptr = list[i];
                while (ptr != NULL){
                    if(ptr != list[i]){
                        // recList ptr->name index
                        int index = 0;
                        for(int j = 0; j < size; j++) {
                            if(strcmp(list[j]->name, ptr->name) == 0) {
                                index = j;
                                break;
                            }
                        }
                        if (!isMarked(markedList, ptr->name, size) && detectCycle(list, recList, markedList, ptr->name, size)){
                            return true;
                        }
                        else if(recList[index]->marked) {
                            return true;
                        }
                    }
                    ptr = ptr->next;
                }
            }
        }
    }
    Unmark(recList, target, size);
    return false;
}

bool isCyclic(struct Vertex* list[], int size) {
    struct Mark* recList[size];
    struct Mark* markedList[size];
    for(int i = 0; i < size; i++) {
        struct Mark *tmp1 = malloc(sizeof(struct Mark));
        tmp1->marked = false;
        strcpy(tmp1->name, list[i]->name);
        recList[i] = tmp1;
        struct Mark *tmp2 = malloc(sizeof(struct Mark));
        tmp2->marked = false;
        strcpy(tmp2->name, list[i]->name);
        markedList[i] = tmp2;
    }

    for(int i = 0; i < size; i++) {
        if(!isMarked(markedList, list[i]->name, size) && detectCycle(list, recList, markedList, list[i]->name, size)) {
            for(int i = 0; i < size; i++) {
                free(markedList[i]);
                free(recList[i]);
            }
            return true;
        }
    }
    for(int i = 0; i < size; i++) {
        free(markedList[i]);
        free(recList[i]);
    }
    return false;
}

int main(int argc, char *argv[]){
    printf("\n");
    // Reading the First File
    if (argc == 0){
        return 0;
    }

    FILE *file;
    file = fopen(argv[1], "r");
    char temp[100];
    fscanf(file, "%s", temp);
    int size = atoi(temp);
    struct Mark *markedList[size];

    // Initiate Adjacency Matrix - NULL
    struct Vertex *list[size];
    for (int i = 0; i < size; i++){
        list[i] = NULL;
        fscanf(file, "%s", temp);
        // initialize the graph
        initialize(list, size, temp);
    }
    sort(list, size);
    for(int i = 0; i < size; i++) {
        // initialize the marked array
        struct Mark *tmp = malloc(sizeof(struct Mark));
        tmp->marked = false;
        strcpy(tmp->name, list[i]->name);
        markedList[i] = tmp;
    }
    // Add Adjacency
    while (!feof(file)){
        char buff1[100];
        char buff2[100];
        char buff3[100];
        fscanf(file, "%s %s %s\n", buff1, buff2, buff3);
        addAdjacency(list, size, buff1, buff2, atoi(buff3));
    }
    fclose(file);

    // Querying and Detect Cycle
    if(isCyclic(list, size)) {
        printf("Cycle\n");
    }
    else {
        file = fopen(argv[2], "r");
        while(!feof(file)) {
            char buff1[255];
            fscanf(file, "%s\n", buff1);
            // Topological Sort
            for (int z = 0; z < size; z++){
                if (!isMarked(markedList, list[z]->name, size)){
                    topological(list, markedList, list[z]->name, size);
                }
            }
            Reset(markedList, size);
            //SSP
            SSP(list, buff1, size);
        }
        fclose(file);
    }

    // Print adjacency list
    // for(int i = 0; i < size; i++) {
    //     struct Vertex* ptr = list[i];
    //     while(ptr != NULL) {
    //         printf("%s ", ptr->name);
    //         ptr = ptr->next;
    //     }
    //     printf("\n");
    // }
    
    // Free Memory
    for (int x = 0; x < size; x++){
        struct Vertex *ptr = list[x];
        while (ptr != NULL){
            struct Vertex *temp = ptr;
            ptr = ptr->next;
            free(temp);
        }
        free(markedList[x]);
    }
    return 0;
}