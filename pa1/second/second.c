#include<stdio.h>
#include<stdlib.h>
#include<string.h>

struct Node {
    int data;
    struct Node *next;
};

struct Node *head = NULL;
size_t size = sizeof(struct Node);

void print() {
    if(head == NULL) {
        printf("EMPTY");
    }
    struct Node *ptr = head;
    while(ptr != NULL) {
        printf("%d", ptr->data);
        if(ptr->next != NULL) {
            printf(" ");
        }
        ptr = ptr->next;
    }
    printf("\n");
}

void insert(int val) {
    // If head == null AKA empty list
    if(head == NULL) {
        struct Node* temp = malloc(size);
        temp->data = val;
        temp->next = NULL;
        head = temp;
    }
    // If val goes before head -> make it the new head
    else if(head->data > val) {
        struct Node* temp = malloc(size);
        temp->data = val;
        temp->next = head;
        head = temp;
    }
    // Insert val between nodes
    else {
        struct Node* ptr = head;
        struct Node* prev = NULL;
        while(ptr != NULL) {
            if(val < ptr->data) {
                struct Node *temp = malloc(size);
                temp->data = val;
                temp->next = ptr;
                prev->next = temp;
                break;
            }
            else if(val == ptr->data) {
                break;
            }
            else {
                prev = ptr;
                ptr = ptr->next;
                // Insert val at the very end
                if(ptr == NULL) {
                    struct Node *temp2 = malloc(size);
                    temp2->data = val;
                    temp2->next = NULL;
                    prev->next = temp2;
                }
            }
        }
    }
    print();
}

void delete(int val) {
    // Delete the head
    if(head == NULL) {
        print();
        return;
    } 
    else if(head->data == val) {
        struct Node *temp = head;
        head = head->next;
        free(temp);
    }
    // Delete in between or last
    else {
        struct Node *ptr = head;
        struct Node *prev = NULL;
        while(ptr != NULL) {
            if(val == ptr->data) {
                prev->next = ptr->next;
                free(ptr);
                break;
            }
            else {
                prev = ptr;
                ptr = ptr->next;
            }
        }
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
            char buff2[50];
            fscanf(file, "%s %s\n", buff1, buff2);
            if(strcmp(buff1, "INSERT") == 0) {
                insert(atoi(buff2));
            }
            else{
                delete(atoi(buff2));
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
