#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct Node* queue = NULL;

struct Vertex {
    char name[255];
    struct Vertex* next;
};

struct Mark {
    char name[255];
    bool marked;
};

struct Node {
    char name[255];
    struct Node* next;
};

// Queue Methods
void enqueue(char val[], struct Node** queue) {
    // If linked list is empty
    if(*queue == NULL) {
        struct Node* temp = malloc(sizeof(struct Node));
        strcpy(temp->name, val);
        temp->next = NULL;
        *queue = temp;
    }
    else {
        // Insert at the end
        struct Node* ptr = *queue;
        // iterate until it reaches the end
        while(ptr->next != NULL) {
            ptr = ptr->next;
        }
        // Insert val at the very end
        struct Node *temp2 = malloc(sizeof(struct Node));
        strcpy(temp2->name, val);
        temp2->next = NULL;
        ptr->next = temp2;
    }
}

void pop(struct Node** head, char ans[]) {
    // Delete the head
    if(*head == NULL) {
        return;
    } 
    else{
        struct Node* temp = *head;
        strcpy(ans, temp->name);
        *head = (*head)->next;
        free(temp);
        return;
    }
}

bool isEmpty(struct Node** head) {
    if(*head == NULL) {
        return true;
    }
    return false;
}



// Graph Methods
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

void addAdjacency(struct Vertex* list[], int size, char v1[], char v2[]) {
    int i = 0;
    while(i < size && list[i] != NULL) {
        // if v1 is found, insert v2 as an adjacent vertex
        if(strcmp(list[i]->name, v1) == 0) {
            struct Vertex* ptr = list[i];
            // insert at front
            if(ptr->next != NULL && strcmp(ptr->next->name, v2) > 0) {
                struct Vertex* temp = malloc(sizeof(struct Vertex));
                strcpy(temp->name, v2);
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
                temp->next = NULL;
                ptr->next = temp;
            }
        }
        // if v2 is found, insert v1 as an adjacent vertex
        // if v1 is found, insert v2 as an adjacent vertex
        else if(strcmp(list[i]->name, v2) == 0) {
            struct Vertex* ptr = list[i];
            // insert at front
            if(ptr->next != NULL && strcmp(ptr->next->name, v1) > 0) {
                struct Vertex* temp = malloc(sizeof(struct Vertex));
                strcpy(temp->name, v1);
                temp->next = ptr->next;
                ptr->next = temp;
            }
            while(ptr->next != NULL) {
                // Handle Duplicates
                if(strcmp(ptr->name, v1) == 0) {
                    break;
                }
                // insert in middle
                if(ptr->next != NULL && strcmp(ptr->next->name, v1) > 0) {
                    struct Vertex* temp = malloc(sizeof(struct Vertex));
                    strcpy(temp->name, v1);
                    temp->next = ptr->next;
                    ptr->next = temp;
                    break;
                }
                ptr = ptr->next;
            }
            // insert at end
            if(strcmp(ptr->name, v1) != 0 && ptr->next == NULL) {
                struct Vertex* temp = malloc(sizeof(struct Vertex));
                strcpy(temp->name, v1);
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

// BFS
void BFS(char name[],struct Vertex* list[], struct Mark* markedList[], struct Node* queue, int size) {
    // Mark the current vertex as visited and enqueue it
    Mark(markedList, name, size);
    enqueue(name, &queue);
    while(!isEmpty(&queue)) {
        // Pop from the queue and print
        char target[255];
        pop(&queue, target);
        printf("%s", target);
        // Take the popped element, then find, mark, and enqueue adjacent elements
        for(int i = 0; i < size; i++) {
            if(strcmp(list[i]->name, target) == 0) {
                struct Vertex* ptr = list[i];
                while(ptr != NULL) {
                    if(ptr != list[i]) {
                        if(!isMarked(markedList, ptr->name, size)) {
                            Mark(markedList, ptr->name, size);
                            enqueue(ptr->name, &queue);
                        }
                    }
                    ptr = ptr->next;
                }
            }
        }
        if(!isEmpty(&queue)) {
            printf(" ");
        }
    }
    printf("\n");
}


// Main

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
        // initialize the marked array
        struct Mark* tmp = malloc(sizeof(struct Mark));
        tmp->marked = false;
        strcpy(tmp->name, temp);
        markedList[i] = tmp;
    }
    // Add Adjacency
    while(!feof(file)) {
        char buff1[100];
        char buff2[100];
        fscanf(file, "%s %s\n", buff1, buff2);
        addAdjacency(list, size, buff1, buff2);
    }
    fclose(file);

    //Querying
    file = fopen(argv[2], "r");
    while(!feof(file)) {
        char buff1[100];
        fscanf(file, "%s\n", buff1);
        BFS(buff1, list, markedList, queue, size);
        // Reset Marked List
        for(int i = 0; i < size; i++) {
            markedList[i]->marked = false;
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
        free(markedList[x]);
    }
    return 0;

}