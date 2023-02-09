#include<stdio.h>
#include<stdlib.h>
#include<string.h>

struct Node {
    int data;
    struct Node* next;
};

struct Node* head = NULL;
size_t size = sizeof(struct Node);

void print() {
    if(head == NULL) {
        printf("EMPTY");
    }
    struct Node* ptr = head;
    while(ptr != NULL) {
        printf("%d", ptr->data);
        if(ptr->next != NULL) {
            printf(" ");
        }
        ptr = ptr->next;
    }
    printf("\n");
}

void push(int val) {
    // If head == null AKA empty list
    if(head == NULL) {
        struct Node* temp = (struct Node*) malloc(size);
        temp->data = val;
        temp->next = NULL;
        head = temp;
    }
    // Push new link as the head
    else {
        struct Node* temp = (struct Node*) malloc(size);
        temp->data = val;
        temp->next = head;
        head = temp;
    }
    print();
}

void enqueue(int val) {
    // If linked list is empty
    if(head == NULL) {
        struct Node* temp = (struct Node*) malloc(size);
        temp->data = val;
        temp->next = NULL;
        head = temp;
    }
    else {
        // Insert at the end
        struct Node* ptr = head;
        // iterate until it reaches the end
        while(ptr->next != NULL) {
            ptr = ptr->next;
        }
        // Insert val at the very end
        struct Node *temp2 = (struct Node*) malloc(size);
        temp2->data = val;
        temp2->next = NULL;
        ptr->next = temp2;
    }
    print();
}

void pop() {
    // Delete the head
    if(head == NULL) {
        print();
        return;
    } 
    else{
        struct Node* temp = head;
        head = head->next;
        free(temp);
    }
    print();
}

int main(int argc, char* argv[]) {
    if(argc == 0) {
        return 0;
    }
    
    FILE * file;
    
    if((file = fopen(argv[1], "r"))) {
        while(!feof(file)) {
            char buff1[50];
            fscanf(file, "%s\n", buff1);
            if(strcmp(buff1, "PUSH") == 0) {
                char buff2[50];
                fscanf(file, "%s", buff2);
                push(atoi(buff2));
            }
            else if(strcmp(buff1, "ENQUEUE") == 0) {
                char buff2[50];
                fscanf(file, "%s", buff2);
                enqueue(atoi(buff2));
            }
            else {
                pop();
            }
        }

        fclose(file);

        return 0;

    }
    else {
        printf("error\n");
        return 0;
    }

    

    
}