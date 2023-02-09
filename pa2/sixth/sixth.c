#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<limits.h>

struct Vertex {
    char name[255];
    struct Vertex* next;
    int weight;
};

struct Distance{
    char name[255];
    int distance;
};

struct Mark{
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

void Reset(struct Mark *markedList[], int size) {
    for(int i = 0; i < size; i++) {
        markedList[i]->marked = false;
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

void minDist(struct Vertex* list[], struct Mark* markedList[], struct Distance* distanceList[], int size, char ans[]) {
    int min = INT_MAX;
    char buff[255];
    for(int i = 0; i < size; i++) {
        if(!isMarked(markedList, list[i]->name, size) && distanceList[i]->distance <= min) {
            min = distanceList[i]->distance;
            strcpy(buff, distanceList[i]->name);
        }
    }
    strcpy(ans, buff);
}

void Dijsktra(struct Vertex* list[], int size, char src[]) {
    // initialize marked and distance list
    struct Mark* markedList[size];
    struct Distance* distanceList[size];
    for(int i = 0; i < size; i++) {
        char buff[255];
        strcpy(buff, list[i]->name);
        // for the marked list
        struct Mark* tmp = malloc(sizeof(struct Mark));
        tmp->marked = false;
        strcpy(tmp->name, buff);
        markedList[i] = tmp;
        // for the distance list
        struct Distance* tmp2 = malloc(sizeof(struct Distance));
        if(strcmp(buff, src) == 0) {
            tmp2->distance = 0;
        }
        else {
            tmp2->distance = INT_MAX;
        }
        strcpy(tmp2->name, buff);
        distanceList[i] = tmp2;
    }

    for(int i = 0; i < size; i++) {
        char min[255];
        minDist(list, markedList, distanceList, size, min);
        Mark(markedList, min, size);
        // ptr at the adjacency list for the current min value
        struct Vertex* ptr = list[getI(distanceList, size, min)];
        while(ptr != NULL) {
            if(ptr != list[getI(distanceList, size, min)]) {
                if(!isMarked(markedList, ptr->name, size)) {
                    if(distanceList[getI(distanceList,size,min)]->distance != INT_MAX) {
                        if(distanceList[getI(distanceList,size,min)]->distance + weight(list, size, min, ptr->name) < distanceList[getI(distanceList,size,ptr->name)]->distance) {
                            distanceList[getI(distanceList,size,ptr->name)]->distance = distanceList[getI(distanceList,size,min)]->distance + weight(list, size, min, ptr->name);
                        }

                    }
                }
            }
            ptr = ptr->next;
        }
    }
    
    // Print out result and free markedList and distanceList
    for(int i = 0; i < size; i++) {
        if(distanceList[i]->distance == INT_MAX) {
            printf("%s %s\n", distanceList[i]->name, "INF");
        }
        else {
            printf("%s %d\n", distanceList[i]->name, distanceList[i]->distance);
        }
        free(markedList[i]);
        free(distanceList[i]);
    }
    printf("\n");

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
        fscanf(file, "%s", temp);
        // initialize the graph
        initialize(list, size, temp);
    }
    sort(list, size);

    // Add Adjacency
    while(!feof(file)) {
        char buff1[100];
        char buff2[100];
        char buff3[100];
        fscanf(file, "%s %s %s\n", buff1, buff2, buff3);
        addAdjacency(list, size, buff1, buff2, atoi(buff3));
    }
    fclose(file);
    
    // Querying
    file = fopen(argv[2], "r");
    while(!feof(file)) {
        char buff[100];
        fscanf(file, "%s\n", buff);
        Dijsktra(list, size, buff);
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