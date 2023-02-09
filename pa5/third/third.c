#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>
#include<string.h>
#include<math.h>

struct Node {
    char name[50];
    int* values;
    struct Node* next;
};

struct Gate {
    char name[50];
    char target[50];
    char** inputs;
    char** outputs;
    char** selectors;
    int n_inputs;
    int n_outputs;
    struct Gate* next;
};

FILE * file;
int num_compares;
int success_compares;
int numInputs;
int numOutputs;
int sizeOutputs;
int numMystery = 0;
char* valid_gates[] = {"NOT","AND", "OR", "XOR", "DECODER", "MULTIPLEXER"};
char* gate_options[] = {"AND", "OR", "XOR"};
// Linked list thatt will hold all of the "output" nodes
struct Node * oHead = NULL;
// Linked list that will hold all of the "input" nodes
struct Node * iHead = NULL;
// Linked list that will hold all of the "temp" nodes
struct Node * tHead = NULL;
// Linked list that will hold the Circuit
struct Gate * circuit = NULL;

int base[4][2] = 
    {{0, 0},
    {0, 1},
    {1, 1},
    {1, 0}};

int** grayCode(int size, int target, int** table) {
    
    if(table == NULL) {
        table = malloc(4 * sizeof(int*));
        for(int i = 0; i < 4; i++) {
            table[i] = malloc(2 * sizeof(int));
            for(int j = 0; j < 2; j++) {
                table[i][j] = base[i][j];
            }
        }
    }
    if(target == 2) {
        return table;
    }
    if(size == target) {
        return table;
    }

    // save the old table
    int rows = 1 << size;
    int columns = size;
    int tempTable[rows][columns];
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < columns; j++) {
            tempTable[i][j] = table[i][j];
        }
        free(table[i]);
    }
    free(table);

    size++;
    rows = 1 << size;
    columns = size;
    // resize the return table to the next size
    table = malloc(rows * sizeof(int*));
    for(int i = 0; i < rows; i++) {
        table[i] = malloc(columns * sizeof(int));
    }

    for(int i = 0; i < rows/2; i++) {
        for(int j = 1; j < columns; j++) {
            table[i][j] = tempTable[i][j-1];
        }
    }
    int cnt = rows/2;
    for(int i = (rows/2)-1; i >= 0; i--) {
        for(int j = 1; j < columns; j++) {
            table[cnt][j] = tempTable[i][j-1];
        }
        cnt++;
    }
    for(int i = 0; i < rows; i++) {
        if(i < rows/2) {
            table[i][0] = 0;
        }
        else {
            table[i][0] = 1;
        }
    }
    return(grayCode(size, target, table));
}

void addGate(struct Gate** head, struct Gate* gate) {
    gate->next = NULL;
    if(*head == NULL) {
        *head = gate;
    }
    else {
        struct Gate* ptr = *head;
        while(ptr->next != NULL) {
            ptr = ptr->next;
        }
        ptr->next = gate;
    }
}

void addLL(struct Node** head, struct Node* node) {
    node->next = NULL;
    if(*head == NULL) {
        *head = node;
    }
    else {
        struct Node* ptr = *head;
        while(ptr->next != NULL) {
            ptr = ptr->next;
        }
        ptr->next = node;
    }
}

int bin2dec(int bits, char bitstring[bits+1]) {
    int total = 0;
    int cnt = 0;
    for(int i = bits-1; i >= 0; i--) {
        if(bitstring[i] == '1') {
            total = total + (1 << (cnt));
        }
        cnt++;
    }
    return total;
}

void getBinary(int size, int num, char string[size+1]) {
    int total = num;
    char str[size+1];
    str[size] = '\0';
    for(int i = 0; i < size; i++) {
        str[i] = '0';
    }
    int cnt = size-1;
    for(int i = 0; i < size; i++) {
        if((1 << cnt) <= total && total > 0) {
            str[i] = '1';
            total -= (1<<cnt);
        }
        cnt--;
    }
    strcpy(string, str);

}

struct Node* findNode(struct Node** head,char tag[50]) {
    struct Node* ptr = *head;
    while(ptr != NULL) {
        if(strcmp(ptr->name, tag) == 0) {
            return ptr;
        }
        ptr = ptr->next;
    }
    return NULL;
}

void initiateMatrix() {
    int** table = grayCode(2, numInputs, NULL);
    struct Node* ptr = iHead;
    int col = 0;
    while(ptr != NULL && col < numInputs) {
        ptr->values = malloc(sizeOutputs * sizeof(int));
        for(int i = 0; i < sizeOutputs; i++) {
            ptr->values[i] = table[i][col];
        }
        ptr = ptr->next;
        col++;
    }
    for(int i = 0; i < (1 << numInputs); i++) {
        for(int j = 0; j < numInputs; j++) {
        }
        free(table[i]);
    }
    free(table);
}

void readInput() {
    char buff1[50];
    fscanf(file, "%s", buff1);
    numInputs = atoi(buff1);
    sizeOutputs = 1 << numInputs;
    for(int i = 0; i < numInputs; i++) {
        struct Node * temp = malloc(sizeof(struct Node));
        fscanf(file, "%s", temp->name);
        addLL(&iHead, temp);
    }
}

void readOutput() {
    char buff1[50];
    fscanf(file, "%s", buff1);
    numOutputs = atoi(buff1);
    //Array that holds Output struct
    for(int i = 0; i < numOutputs; i++) {
        struct Node * temp = malloc(sizeof(struct Node));
        fscanf(file, "%s", temp->name);
        addLL(&oHead, temp);
    }
}

void readOutputValues() {
    struct Node* ptr = oHead;
    while(ptr != NULL) {
        //Read the output values
        char buff[50];
        fscanf(file, "%s", buff);
        ptr->values = malloc(sizeOutputs * sizeof(int));
        for(int i = 0; i < sizeOutputs; i++) {
            char t[5];
            fscanf(file, "%s", t);
            ptr->values[i] = t[0]-48;
        }
        ptr = ptr->next;
    }
}

// For OR and AND gates: removes from the circuit LL, and then frees the structure
void removeGate(struct Gate** head, struct Gate* gate) {
    struct Gate* ptr = *head;
    struct Gate* target = NULL;
    while(ptr != NULL) {
        if(strcmp(ptr->name, gate->name) == 0 && strcmp(ptr->inputs[0], gate->inputs[0]) == 0 && strcmp(ptr->inputs[1], gate->inputs[1]) == 0 && strcmp(ptr->outputs[0], gate->outputs[0]) == 0) {
            target = ptr;
        }
        ptr = ptr->next;
    }
    if(target == *head) {
        *head = target->next;
    }
    else {
        ptr = *head;
        while(ptr->next != target) {
            ptr = ptr->next;
        }
        ptr->next = target->next;
    }
    free(target->inputs[0]);
    free(target->inputs[1]);
    free(target->outputs[0]);
    free(target->inputs);
    free(target->outputs);
    free(target);
}

bool isMystery(char name[50]) {
    bool mystery = true;
    for(int i = 0; i < 6; i++) {
        if(strcmp(name, valid_gates[i]) == 0) {
            mystery = false;
            break;
        }
    }
    return mystery;
}

int findGrayIndex(int inputs, int selectors, char string[selectors+1]) {
    int** table = grayCode(2, selectors, NULL);
    char temp[selectors+1];
    temp[selectors] = '\0';
    for(int i = 0; i < inputs; i++) {
        for(int j = 0; j < selectors; j++) {
            if(table[i][j] == 0) {
                temp[j] = '0';
            }
            else {
                temp[j] = '1';
            }
        }
        if(strcmp(string, temp) == 0) {
            for(int x = 0; x < inputs; x++) {
                free(table[x]);
            }
            free(table);
            return i;
        }
    }
    for(int x = 0; x < inputs; x++) {
        free(table[x]);
    }
    free(table);
    return -1;

}

void AND(char in1[50], char in2[50], char outputName[50], bool compare) {
    struct Node* input1 = findNode(&iHead, in1);
    struct Node* input2 = findNode(&iHead, in2);
    if(input1 == NULL) {
        input1 = findNode(&tHead, in1);
    }
    if(input2 == NULL) {
        input2 = findNode(&tHead, in2);
    }
    struct Node* node = malloc(sizeof(struct Node));
    strcpy(node->name, outputName);
    node->values = malloc(sizeOutputs * sizeof(int));
    if(input1 == NULL) {
        for(int i = 0; i < sizeOutputs; i++) {
            node->values[i] = (in1[0]-48) & input2->values[i];
        }
    }
    else if(input2 == NULL) {
        for(int i = 0; i < sizeOutputs; i++) {
            node->values[i] = input1->values[i] & (in2[0]-48);
        }
    }
    else {
        for(int i = 0; i < sizeOutputs; i++) {
            node->values[i] = input1->values[i] & input2->values[i];
        }
    }
    if(!compare) {
        addLL(&tHead, node);
    }
    else {
        struct Node* out = findNode(&oHead, outputName);
        bool equivalent = true;
        for(int i = 0; i < sizeOutputs; i++) {
            if(node->values[i] != out->values[i]) {
                equivalent = false;
            }
        }
        if(equivalent) {
            success_compares++;
        }
        free(node->values);
        free(node);
    }
}

void OR(char in1[50], char in2[50], char outputName[50], bool compare) {
    struct Node* input1 = findNode(&iHead, in1);
    struct Node* input2 = findNode(&iHead, in2);
    if(input1 == NULL) {
        input1 = findNode(&tHead, in1);
    }
    if(input2 == NULL) {
        input2 = findNode(&tHead, in2);
    }
    struct Node* node = malloc(sizeof(struct Node));
    strcpy(node->name, outputName);
    node->values = malloc(sizeOutputs * sizeof(int));
    if(input1 == NULL) {
        for(int i = 0; i < sizeOutputs; i++) {
            node->values[i] = (in1[0]-48) | input2->values[i];
        }
    }
    else if(input2 == NULL) {
        for(int i = 0; i < sizeOutputs; i++) {
            node->values[i] = input1->values[i] | (in2[0]-48);
        }
    }
    else {
        for(int i = 0; i < sizeOutputs; i++) {
            node->values[i] = input1->values[i] | input2->values[i];
        }
    }
    if(!compare) {
        addLL(&tHead, node);
    }
    else {
        struct Node* out = findNode(&oHead, outputName);
        bool equivalent = true;
        for(int i = 0; i < sizeOutputs; i++) {
            if(node->values[i] != out->values[i]) {
                equivalent = false;
            }
        }
        if(equivalent) {
            success_compares++;
        }
        free(node->values);
        free(node);
    }
}

void XOR(char in1[50], char in2[50], char outputName[50], bool compare) {
    struct Node* input1 = findNode(&iHead, in1);
    struct Node* input2 = findNode(&iHead, in2);
    if(input1 == NULL) {
        input1 = findNode(&tHead, in1);
    }
    if(input2 == NULL) {
        input2 = findNode(&tHead, in2);
    }
    struct Node* node = malloc(sizeof(struct Node));
    strcpy(node->name, outputName);
    node->values = malloc(sizeOutputs * sizeof(int));
    if(input1 == NULL) {
        for(int i = 0; i < sizeOutputs; i++) {
            node->values[i] = (in1[0]-48) ^ input2->values[i];
        }
    }
    else if(input2 == NULL) {
        for(int i = 0; i < sizeOutputs; i++) {
            node->values[i] = input1->values[i] ^ (in2[0]-48);
        }
    }
    else {
        for(int i = 0; i < sizeOutputs; i++) {
            node->values[i] = input1->values[i] ^ input2->values[i];
        }
    }
    if(!compare) {
        addLL(&tHead, node);
    }
    else {
        struct Node* out = findNode(&oHead, outputName);
        bool equivalent = true;
        for(int i = 0; i < sizeOutputs; i++) {
            if(node->values[i] != out->values[i]) {
                equivalent = false;
            }
        }
        if(equivalent) {
            success_compares++;
        }
        free(node->values);
        free(node);
    }
}

void NOT(char in[50], char outputName[50], bool compare) {
    struct Node* input = findNode(&iHead, in);
    if(input == NULL) {
        input = findNode(&tHead, in);
    }
    struct Node* node = malloc(sizeof(struct Node));
    strcpy(node->name, outputName);
    node->values = malloc(sizeOutputs * sizeof(int));
    for(int i = 0; i < sizeOutputs; i++) {
        node->values[i] = !(input->values[i]);
    }
    if(!compare) {
        addLL(&tHead, node);
    }
    else {
        struct Node* out = findNode(&oHead, outputName);
        bool equivalent = true;
        for(int i = 0; i < sizeOutputs; i++) {
            if(node->values[i] != out->values[i]) {
                equivalent = false;
            }
        }
        if(equivalent) {
            success_compares++;
        }
        free(node->values);
        free(node);
    }
}

void DECODER(int num_Inputs, int num_Outputs, char** inputs, char** outputs, bool compare) {
    int** table = grayCode(2, num_Inputs, NULL);
    int table2[sizeOutputs][num_Inputs];
    for(int i = 0; i < num_Inputs; i++) {
        struct Node* ptr = findNode(&iHead, inputs[i]);
        if(ptr == NULL) {
            ptr = findNode(&tHead, inputs[i]);
        }
        for(int j = 0; j < sizeOutputs; j++) {
            table2[j][i] = ptr->values[j];
        }
    }
    for(int i = 0; i < num_Outputs; i++ ) {
        char string[num_Inputs+1];
        string[num_Inputs] = '\0';
        for(int j = 0; j < num_Inputs; j++) {
            if(table[i][j] == 0) {
                string[j] = '0';
            }
            else {
                string[j] = '1';
            }
        }
        // make the output
        struct Node* temp = malloc(sizeof(struct Node));
        strcpy(temp->name, outputs[i]);
        temp->values = malloc(sizeOutputs * sizeof(int));
        for(int x = 0; x < sizeOutputs; x++) {
            int val = 1;
            for(int y = 0; y < num_Inputs; y++) {
                if(string[y] == '0') {
                    val = val & !table2[x][y];
                }
                else {
                    val = val & table2[x][y];
                }
            }
            temp->values[x] = val;
        }
        if(!compare) {
            addLL(&tHead, temp);
        }
        else {
            struct Node* o = findNode(&oHead, outputs[i]);
            bool equivalent = true;
            for(int i = 0; i < sizeOutputs; i++) {
                if(temp->values[i] != o->values[i]) {
                    equivalent = false;
                }
            }
            if(equivalent) {
                success_compares++;
            }
            free(temp->values);
            free(temp);
        }
    }
    for(int i = 0; i < (1 << num_Inputs); i++) {
        free(table[i]);
    }
    free(table);
}

void MUX(int num_Inputs, int num_Selectors, char** inputs, char** var_selectors, char name_output[50], bool compare) {
    bool variables = false;
    if((findNode(&iHead, inputs[0]) != NULL) || (findNode(&tHead, inputs[0]) != NULL)) {
        variables = true;
    }
    int table[sizeOutputs][num_Selectors];
    int table2[sizeOutputs][num_Inputs];
    if(variables) {
        for(int i = 0; i < num_Inputs; i++) {
            struct Node* ptr = findNode(&iHead, inputs[i]);
            if(ptr == NULL) {
                ptr = findNode(&tHead, inputs[i]);
            }
            for(int j = 0; j < sizeOutputs; j++) {
                table2[j][i] = ptr->values[j];
            }
        }
    }

    for(int i = 0; i < num_Selectors; i++) {
        struct Node* ptr = findNode(&iHead, var_selectors[i]);
        if(ptr == NULL) {
            ptr = findNode(&tHead, var_selectors[i]);
        }
        for(int j = 0; j < sizeOutputs; j++) {
            table[j][i] = ptr->values[j];
        }
    }

    struct Node* temp = malloc(sizeof(struct Node));
    strcpy(temp->name, name_output);
    temp->values = malloc(sizeOutputs * sizeof(int));
    for(int i = 0; i < sizeOutputs; i++ ) {
        char string[num_Selectors+1];
        string[num_Selectors] = '\0';
        for(int j = 0; j < num_Selectors; j++) {
            if(table[i][j] == 0) {
                string[j] = '0';
            }
            else {
                string[j] = '1';
            }
        }
        int x = findGrayIndex(num_Inputs, num_Selectors, string);
        if(variables) {
            temp->values[i] = table2[i][x];
        }
        else {
            temp->values[i] = atoi(inputs[x]);
        }
    }
    if(!compare) {
        addLL(&tHead, temp);
    }
    else {
        struct Node* o = findNode(&oHead, name_output);
        bool equivalent = true;
        for(int i = 0; i < sizeOutputs; i++) {
            if(temp->values[i] != o->values[i]) {
                equivalent = false;
            }
        }
        if(equivalent) {
            success_compares++;
        }
        free(temp->values);
        free(temp);
    }
}

void clearTemp() {
    struct Node* ptr = tHead;
    while(ptr != NULL) {
        struct Node* temp = ptr;
        if(temp->values != NULL) {
            free(temp->values);
        }
        ptr = ptr->next;
        free(temp);
    }
    tHead = NULL;
}

void newMakeGate() {
    // Name of the gate
    struct Gate* gate = malloc(sizeof(struct Gate));
    char gateName[50];
    fscanf(file, "%s", gateName);
    strcpy(gate->name, gateName);
    bool mystery = isMystery(gateName);
    if(mystery) {
        numMystery++;
        strcpy(gate->target, "NULL");
        char buff[50];
        fscanf(file, "%s", buff);
        int num_mystery_inputs = atoi(buff);
        char in_list[num_mystery_inputs][50];
        char out_list[num_mystery_inputs][50];
        int ins = 0;
        int temps = 0;
        int outs = 0;
        // x > 1 outputs, DECODER
        // 1 input, 1 output: NOT Gate
        // 2 inputs, 1 output: AND, OR, XOR, MUX
        for(int i = 0; i < num_mystery_inputs; i++) {
            if(i == num_mystery_inputs - 1) {
                fscanf(file, "%s\n", buff);
            }
            else {
                fscanf(file, "%s", buff);
            }
            if(findNode(&iHead, buff) != NULL || findNode(&tHead, buff) != NULL || buff[0] == '0' || buff[0] == '1') {
                strcpy(in_list[ins], buff);
                ins++;
            }
            else if(findNode(&oHead, buff) != NULL) {
                strcpy(out_list[outs], buff);
                outs++;
            }
            else {
                strcpy(out_list[temps], buff);
                temps++;
            }
        }
        gate->n_inputs = ins;
        gate->n_outputs = outs + temps;
        gate->inputs = malloc(gate->n_inputs * sizeof(char*));
        gate->outputs = malloc(gate->n_outputs * sizeof(char*));
        for(int i = 0; i < ins; i++) {
            gate->inputs[i] = malloc(50 * sizeof(char));
            strcpy(gate->inputs[i], in_list[i]);
        }
        for(int i = 0; i < outs + temps; i++) {
            gate->outputs[i] = malloc(50 * sizeof(char));
            strcpy(gate->outputs[i], out_list[i]);
            if(findNode(&oHead, gate->outputs[i]) == NULL) {
                struct Node* tmp = malloc(sizeof(struct Node));
                tmp->values = NULL;
                strcpy(tmp->name,gate->outputs[i]);
                addLL(&tHead, tmp);
            }
        }
    }
    else {
        if(strcmp(gateName, "AND") == 0 || strcmp(gateName, "OR") == 0 || strcmp(gateName, "XOR") == 0) {
            gate->inputs = malloc(2 * sizeof(char*));
            for(int i = 0; i < 2; i++) {
                gate->inputs[i] = malloc(50 * sizeof(int));
            }
            gate->outputs = malloc(sizeof(char*));
            gate->outputs[0] = malloc(50 * sizeof(char));
            gate->n_inputs = 2;
            gate->n_outputs = 1;
            fscanf(file, "%s %s %s\n", gate->inputs[0], gate->inputs[1], gate->outputs[0]);
            if(findNode(&oHead, gate->outputs[0]) == NULL) {
                struct Node* tmp = malloc(sizeof(struct Node));
                tmp->values = NULL;
                strcpy(tmp->name,gate->outputs[0]);
                addLL(&tHead, tmp);
            }
        }
        else if(strcmp(gateName, "NOT") == 0) {
            gate->inputs = malloc(sizeof(char*));
            gate->inputs[0] = malloc(50 * sizeof(char));
            gate->outputs = malloc(sizeof(char*));
            gate->outputs[0] = malloc(50 * sizeof(char));
            gate->n_inputs = 1;
            gate->n_outputs = 1;
            fscanf(file, "%s %s\n", gate->inputs[0], gate->outputs[0]);
            if(findNode(&oHead, gate->outputs[0]) == NULL) {
                struct Node* tmp = malloc(sizeof(struct Node));
                tmp->values = NULL;
                strcpy(tmp->name,gate->outputs[0]);
                addLL(&tHead, tmp);
            }
        }
        else if(strcmp(gateName, "DECODER") == 0) {
            char buff[50];
            fscanf(file, "%s", buff);
            gate->n_inputs = atoi(buff);
            gate->n_outputs = 1 << gate->n_inputs;
            gate->inputs = malloc(gate->n_inputs * sizeof(char*));
            gate->outputs = malloc(gate->n_outputs * sizeof(char*));
            for(int i = 0; i < gate->n_inputs; i++) {
                gate->inputs[i] = malloc(50 * sizeof(char));
                fscanf(file, "%s", gate->inputs[i]);
            }
            for(int i = 0; i < gate->n_outputs; i++) {
                gate->outputs[i] = malloc(50 * sizeof(char));
                if(i == gate->n_outputs-1) {
                    fscanf(file, "%s\n", gate->outputs[i]);
                }
                else {
                    fscanf(file, "%s", gate->outputs[i]);
                }
                if(findNode(&oHead, gate->outputs[i]) == NULL) {
                    struct Node* tmp = malloc(sizeof(struct Node));
                    tmp->values = NULL;
                    strcpy(tmp->name,gate->outputs[i]);
                    addLL(&tHead, tmp);
                }
            }
        }
        else if(strcmp(gateName, "MULTIPLEXER") == 0) {
            char buff[50];
            fscanf(file, "%s", buff);
            gate->n_inputs = atoi(buff);
            gate->n_outputs = 1;
            gate->inputs = malloc(gate->n_inputs * sizeof(char*));
            int num_selectors = log2(gate->n_inputs);
            gate->selectors = malloc(num_selectors * sizeof(char*));
            gate->outputs = malloc(sizeof(char*));
            gate->outputs[0] = malloc(50 * sizeof(char));
            for(int i = 0; i < gate->n_inputs; i++) {
                gate->inputs[i] = malloc(50 * sizeof(char));
                fscanf(file, "%s", gate->inputs[i]);
            }
            for(int i = 0; i < num_selectors; i++) {
                gate->selectors[i] = malloc(50 * sizeof(char));
                fscanf(file, "%s", gate->selectors[i]);
            }
            fscanf(file, "%s\n", gate->outputs[0]);
            if(findNode(&oHead, gate->outputs[0]) == NULL) {
                struct Node* tmp = malloc(sizeof(struct Node));
                tmp->values = NULL;
                strcpy(tmp->name,gate->outputs[0]);
                addLL(&tHead, tmp);
            }
        }
    }
    addGate(&circuit, gate);
}

struct Gate** assumeMystery(struct Gate** list, int* num) {
    struct Gate* ptr = circuit;
    int cnt = 0;
    int n_mystery = numMystery;
    while(ptr != NULL) {
        bool mystery = isMystery(ptr->name);
        if(mystery) {
            list[cnt] = ptr;
            cnt++;
            if(ptr->n_outputs > 1) {
               strcpy(ptr->target, "DECODER");
               n_mystery--;
            }
            else if(ptr->n_inputs == 1 && ptr->n_outputs == 1) {
                strcpy(ptr->target, "NOT");
                n_mystery--;
            }
            else if(ptr->n_inputs > 2 && ptr->n_outputs == 1) {
                int numSelectors = log2(ptr->n_inputs);
                int n_inputs = ptr->n_inputs - numSelectors;
                ptr->selectors = malloc(numSelectors * sizeof(char*));
                int cnt = 0;
                for(int i = ptr->n_inputs - numSelectors; i < ptr->n_inputs; i++) {
                    ptr->selectors[cnt] = malloc(50 * sizeof(char));
                    strcpy(ptr->selectors[cnt], ptr->inputs[i]);
                    free(ptr->inputs[i]);
                    cnt++;
                }
                strcpy(ptr->target, "MULTIPLEXER");
                ptr->n_inputs = n_inputs;
                n_mystery--;
                // for(int i = 0; i < ptr->n_inputs; i++) {
                //     printf("%s ", ptr->inputs[i]);
                // }
                // printf("\n");
                // for(int i = 0; i < numSelectors; i++) {
                //     printf("%s ", ptr->selectors[i]);
                // }
                // printf("\n");
            }
        }
        ptr = ptr->next;
    }
    *num = n_mystery;
    if(n_mystery == 0) {
        return NULL;
    }
    else {
        struct Gate** m_list_final = malloc(n_mystery * sizeof(struct Gate*));
        int c = 0;
        for(int i = 0; i < numMystery; i++) {
            if(strcmp(list[i]->target, "NULL") == 0) {
                m_list_final[c] = list[i];
                c++;
            }
        }
        return(m_list_final);
    }
}

void runCircuit() {
    num_compares = 0;
    success_compares = 0;
    clearTemp();
    struct Gate* ptr = circuit;
    while(ptr != NULL) {
        bool compare = false;
        if(findNode(&oHead, ptr->outputs[0]) != NULL) {
            compare = true;
            num_compares = num_compares + ptr->n_outputs;
        }

        if(strcmp(ptr->name, "AND") == 0 || strcmp(ptr->target, "AND") == 0) {
            AND(ptr->inputs[0], ptr->inputs[1], ptr->outputs[0], compare);
        }
        else if(strcmp(ptr->name, "OR") == 0 || strcmp(ptr->target, "OR") == 0) {
            OR(ptr->inputs[0], ptr->inputs[1], ptr->outputs[0], compare);
        }
        else if(strcmp(ptr->name, "XOR") == 0 || strcmp(ptr->target, "XOR") == 0) {
            XOR(ptr->inputs[0], ptr->inputs[1], ptr->outputs[0], compare);
        }
        else if(strcmp(ptr->name, "NOT") == 0 || strcmp(ptr->target, "NOT") == 0) {
            NOT(ptr->inputs[0], ptr->outputs[0], compare);
        }
        else if(strcmp(ptr->name, "DECODER") == 0 || strcmp(ptr->target, "DECODER") == 0) {
            DECODER(ptr->n_inputs, ptr->n_outputs, ptr->inputs, ptr->outputs, compare);
        }
        else if(strcmp(ptr->name, "MULTIPLEXER") == 0 || strcmp(ptr->target, "MULTIPLEXER") == 0) {
            int numSelectors = log2(ptr->n_inputs);
            MUX(ptr->n_inputs, numSelectors, ptr->inputs, ptr->selectors, ptr->outputs[0], compare);
        }
        ptr = ptr->next;
    }
    // printf("Number of Comparisons: %d Number of Successful Comparisons: %d\n", num_compares, success_compares);
}

bool solveMystery(int n_mystery, struct Gate** gate) {
    char handler[n_mystery+1];
    char end[n_mystery+1];
    handler[n_mystery] = '\0';
    end[n_mystery] = '\0';
    for(int i = 0; i < n_mystery; i++) {
        handler[i] = '0';
        end[i] = '2';
        strcpy(gate[i]->target, gate_options[0]);
    }
    runCircuit();
    bool completed_circuit = false;
    if(num_compares == success_compares) {
        completed_circuit = true;
    }

    while(strcmp(handler, end) != 0 && !completed_circuit) {
        int tracker = n_mystery - 1;
        while(handler[tracker] == '2') {
            handler[tracker] = '0';
            strcpy(gate[tracker]->target, gate_options[handler[tracker]-48]);
            tracker--;
        }
        handler[tracker] = handler[tracker]+1;
        int temp = handler[tracker]-48;
        strcpy(gate[tracker]->target, gate_options[temp]);
        for(int x = 0; x < n_mystery; x++) {
            // printf("%s:%s ", gate[x]->name, gate[x]->target);
        }
        // printf("\n");
        runCircuit();
        if(num_compares <= success_compares) {
            completed_circuit = true;
        }
    }

    if(completed_circuit) {
        return true;
    }
    else {
        return false;
    }
}

struct Gate** OR_Gates(int* num) {
    struct Gate* ptr = circuit;
    int numOR = 0;
    while(ptr != NULL) {
        if(strcmp(ptr->name, "OR") == 0 || strcmp(ptr->target, "OR") == 0) {
            numOR++;
        }
        ptr = ptr->next;
    }
    *num = numOR;
    struct Gate** list = malloc(numOR * sizeof(struct Gate*));
    ptr = circuit;
    int cnt = 0;
    while(ptr != NULL) {
        if(strcmp(ptr->name, "OR") == 0 || strcmp(ptr->target, "OR") == 0) {
            list[cnt] = ptr;
            cnt++;
        }
        ptr = ptr->next;
    }
    return list;
}

struct Gate** AND_Gates(int* num) {
    struct Gate* ptr = circuit;
    int numAND = 0;
    while(ptr != NULL) {
        if(strcmp(ptr->name, "AND") == 0 || strcmp(ptr->target, "AND") == 0) {
            numAND++;
        }
        ptr = ptr->next;
    }
    *num = numAND;
    struct Gate** list = malloc(numAND * sizeof(struct Gate*));
    ptr = circuit;
    int cnt = 0;
    while(ptr != NULL) {
        if(strcmp(ptr->name, "AND") == 0 || strcmp(ptr->target, "AND") == 0) {
            list[cnt] = ptr;
            cnt++;
        }
        ptr = ptr->next;
    }
    return list;
}

bool compareORInputs(struct Gate* gate1, struct Gate* gate2) {
    int x = 0;
    int y = 0;
    bool match1 = false;
    for(int i = 0; i < 2; i++) {
        for(int j = 0; j < 2; j++) {
            if(strcmp(gate1->inputs[i], gate2->inputs[j]) == 0) {
                x = i;
                y = j;
                match1 = true;
            }
        }
    }
    // x = index of the identical input in gate1
    // y = index of the identical input in gate 2
    bool match2 = false;
    if(match1) {
        y = !y;
        if(strcmp(gate1->outputs[0], gate2->inputs[y]) == 0) {
            match2 = true;
        }
    }

    // Gate 1 and Gate 2 can be minimized
    // move gate1->inputs[!x] to gate2->inputs[!y]
    // delete gate1 from the circuit
    if(match2) {
        x = !x;
        strcpy(gate2->inputs[y], gate1->inputs[x]);
        if(strcmp(gate2->inputs[0], gate2->inputs[1]) > 0) {
            char temp[50];
            strcpy(temp, gate2->inputs[0]);
            strcpy(gate2->inputs[0], gate2->inputs[1]);
            strcpy(gate2->inputs[1], temp);
        }
        removeGate(&circuit, gate1);
        return true;
    }

    return false;
}

bool compareDistAND(struct Gate* gate1, struct Gate* gate2, struct Gate* gate3) {
    int x = 0;
    int y = 0;
    bool match1 = false;
    for(int i = 0; i < 2; i++) {
        for(int j = 0; j < 2; j++) {
            if(strcmp(gate1->inputs[i], gate2->inputs[j]) == 0) {
                x = i;
                y = j;
                match1 = true;
            }
        }
    }
    // x = index of the identical input in gate1
    // y = index of the identical input in gate 2
    bool match2 = false;
    int z = 0;
    if(match1) {
        if(strcmp(gate3->inputs[0], gate1->outputs[0]) == 0 && strcmp(gate3->inputs[1], gate2->outputs[0]) == 0) {
            match2 = true;
        }
        else if(strcmp(gate3->inputs[0], gate2->outputs[0]) == 0 && strcmp(gate3->inputs[1], gate1->outputs[0]) == 0) {
            match2 = true;
        }
    }

    if(match2) {
        x = !x;
        if(strcmp(gate1->outputs[0], gate3->inputs[1]) == 0) {
            z = 1;
        }
        char temp2[50];
        strcpy(temp2, gate2->inputs[y]);

        // Merge the two AND gates
        strcpy(gate2->name, "OR");
        strcpy(gate2->target, "OR");
        strcpy(gate2->inputs[y], gate1->inputs[x]);
        if(strcmp(gate2->inputs[0], gate2->inputs[1]) > 0) {
            char temp[50];
            strcpy(temp, gate2->inputs[0]);
            strcpy(gate2->inputs[0], gate2->inputs[1]);
            strcpy(gate2->inputs[1], temp);
        }

        // Change OR gate to AND, replace one of the inputs
        strcpy(gate3->name, "AND");
        strcpy(gate3->target, "AND");
        strcpy(gate3->inputs[z], temp2);
        if(strcmp(gate3->inputs[0], gate3->inputs[1]) > 0) {
            char temp[50];
            strcpy(temp, gate3->inputs[0]);
            strcpy(gate3->inputs[0], gate3->inputs[1]);
            strcpy(gate3->inputs[1], temp);
        }

        removeGate(&circuit, gate1);
        return true;
    }

    return false;
}

bool compareDistOR(struct Gate* gate1, struct Gate* gate2, struct Gate* gate3) {
    int x = 0;
    int y = 0;
    bool match1 = false;
    for(int i = 0; i < 2; i++) {
        for(int j = 0; j < 2; j++) {
            if(strcmp(gate1->inputs[i], gate2->inputs[j]) == 0) {
                x = i;
                y = j;
                match1 = true;
            }
        }
    }
    // x = index of the identical input in gate1
    // y = index of the identical input in gate 2
    bool match2 = false;
    int z = 0;
    if(match1) {
        if(strcmp(gate3->inputs[0], gate1->outputs[0]) == 0 && strcmp(gate3->inputs[1], gate2->outputs[0]) == 0) {
            match2 = true;
        }
        else if(strcmp(gate3->inputs[0], gate2->outputs[0]) == 0 && strcmp(gate3->inputs[1], gate1->outputs[0]) == 0) {
            match2 = true;
        }
    }

    if(match2) {
        x = !x;
        if(strcmp(gate1->outputs[0], gate3->inputs[1]) == 0) {
            z = 1;
        }
        char temp2[50];
        strcpy(temp2, gate2->inputs[y]);

        // Merge the two AND gates
        strcpy(gate2->name, "AND");
        strcpy(gate2->target, "AND");
        strcpy(gate2->inputs[y], gate1->inputs[x]);
        if(strcmp(gate2->inputs[0], gate2->inputs[1]) > 0) {
            char temp[50];
            strcpy(temp, gate2->inputs[0]);
            strcpy(gate2->inputs[0], gate2->inputs[1]);
            strcpy(gate2->inputs[1], temp);
        }

        // Change OR gate to AND, replace one of the inputs
        strcpy(gate3->name, "OR");
        strcpy(gate3->target, "OR");
        strcpy(gate3->inputs[z], temp2);
        if(strcmp(gate3->inputs[0], gate3->inputs[1]) > 0) {
            char temp[50];
            strcpy(temp, gate3->inputs[0]);
            strcpy(gate3->inputs[0], gate3->inputs[1]);
            strcpy(gate3->inputs[1], temp);
        }

        removeGate(&circuit, gate1);
        return true;
    }

    return false;
}

void minimizeAssoc() {
    int numOR = 0;
    struct Gate** list = OR_Gates(&numOR);
    bool compare = false;
    for(int i = 0; i < numOR; i++) {
        for(int j = i; j < numOR; j++) {
            if(j != i) {
                compare = compareORInputs(list[i], list[j]);
            }
            if(compare) {
                break;
            }
        }
        if(compare) {
            break;
        }
    }
    free(list);
    if(compare) {
        minimizeAssoc();
    }
}

void minimizeDisAND() {
    int numOR = 0;
    int numAND = 0;
    struct Gate** list = OR_Gates(&numOR);
    struct Gate** list2 = AND_Gates(&numAND);
    bool compare = false;
    for(int i = 0; i < numAND; i++) {
        for(int j = i; j < numAND; j++) {
            for(int k = 0; k < numOR; k++) {
                if(j != i) {
                    compare = compareDistAND(list2[i], list2[j], list[k]);
                }
                if(compare) {
                    break;
                }
            }
            if(compare) {
                break;
            }
        }
        if(compare) {
            break;
        }
    }
    free(list);
    free(list2);
    if(compare) {
        minimizeDisAND();
    }
}

void minimizeDisOR() {
    int numOR = 0;
    int numAND = 0;
    struct Gate** list = OR_Gates(&numOR);
    struct Gate** list2 = AND_Gates(&numAND);
    bool compare = false;
    for(int i = 0; i < numOR; i++) {
        for(int j = i; j < numOR; j++) {
            for(int k = 0; k < numAND; k++) {
                if(j != i) {
                    compare = compareDistOR(list[i], list[j], list2[k]);
                }
                if(compare) {
                    break;
                }
            }
            if(compare) {
                break;
            }
        }
        if(compare) {
            break;
        }
    }
    free(list);
    free(list2);
    if(compare) {
        minimizeDisOR();
    }
}

int main(int argc, char* argv[]) {
    file = fopen(argv[1], "r");

    //Reading the INputs and Outputs
    int input = 1;
    char buff1[50];
    fscanf(file, "%s", buff1);
    if(strcmp(buff1, "INPUTVAR") == 0) {
        readInput();
        fscanf(file, "%s", buff1);
        if(strcmp(buff1, "OUTPUTVAR") == 0) {
            readOutput();
        }
    }
    else if(strcmp(buff1, "OUTPUTVAR") == 0) {
        input = 0;
        readOutput();
        fscanf(file, "%s", buff1);
        if(strcmp(buff1, "INPUTVAR") == 0) {
            readInput();
        }
    }
    fscanf(file, "%s", buff1);
    if(strcmp(buff1, "OUTPUTVAL") == 0) {
        readOutputValues();
    }
    // Initiate input matrix (the truth table of the inputs, no output)
    initiateMatrix();

    // Scan the entire file and make gate structures
    while(!feof(file)) {
        newMakeGate();
    }

    // Full Circuit is now created, run through and solve for the missing gates
    struct Gate** mystery_list = malloc(numMystery * sizeof(struct Node*));
    int n_mystery;
    struct Gate** mystery_list_final = assumeMystery(mystery_list, &n_mystery);
    clearTemp();
    bool invalid = false;
    if(n_mystery > 0) {
        bool solved = solveMystery(n_mystery, mystery_list_final);
        if(!solved) {
            printf("INVALID\n");
            invalid = true;
        }
        // if(solved) {
        //     for(int i = 0; i < numMystery; i++) {
        //         printf("%s %s\n", mystery_list[i]->name, mystery_list[i]->target);
        //     }
        // }
        // else {
        //     printf("INVALID\n");
        // }
    }
    else {
        runCircuit();
        if(num_compares > success_compares) {
            printf("INVALID\n");
            invalid = true;
        }
        // if(num_compares <= success_compares) {
        //     for(int i = 0; i < numMystery; i++) {
        //         printf("%s %s\n", mystery_list[i]->name, mystery_list[i]->target);
        //     }
        // }
        // else {
        //     printf("INVALID\n");
        // }
    }

    // printf("Mystery List: ");
    // for(int i = 0; i < numMystery; i++) {
    //     printf("%s ", mystery_list[i]->name);
    // }
    // printf("\n");
    free(mystery_list);

    // printf("Mystery List Cut Down: ");
    // for(int i = 0; i < n_mystery; i++) {
    //     printf("%s ", mystery_list_final[i]->name);
    // }
    // printf("\n");
    free(mystery_list_final);

    if(!invalid) {
        //CIRCUIT MINIMIZATION HERE
        if(input == 1) {
            printf("INPUTVAR %d ", numInputs);
            struct Node* ptr = iHead;
            while(ptr != NULL) {
                if(ptr->next == NULL) {
                    printf("%s\n", ptr->name);
                }
                else {
                   printf("%s ", ptr->name); 
                }
                ptr = ptr->next;
            }
            printf("OUTPUTVAR %d ", numOutputs);
            ptr = oHead;
            while(ptr != NULL) {
                if(ptr->next == NULL) {
                    printf("%s\n", ptr->name);
                }
                else {
                   printf("%s ", ptr->name); 
                }
                ptr = ptr->next;
            }
            printf("OUTPUTVAL\n");
            ptr = oHead;
            while(ptr != NULL) {
                printf("%s ", ptr->name);
                for(int i = 0; i < sizeOutputs; i++) {
                    if(i == sizeOutputs - 1) {
                        printf("%d\n", ptr->values[i]);
                    }
                    else {
                       printf("%d ", ptr->values[i]); 
                    }
                }
                ptr = ptr->next;
            }
        }
        else {
            printf("OUTPUTVAR %d ", numOutputs);
            struct Node* ptr = oHead;
            while(ptr != NULL) {
                if(ptr->next == NULL) {
                    printf("%s\n", ptr->name);
                }
                else {
                   printf("%s ", ptr->name); 
                }
                ptr = ptr->next;
            }
            printf("INPUTVAR %d ", numInputs);
            ptr = iHead;
            while(ptr != NULL) {
                if(ptr->next == NULL) {
                    printf("%s\n", ptr->name);
                }
                else {
                   printf("%s ", ptr->name); 
                }
                ptr = ptr->next;
            }
            printf("OUTPUTVAL\n");
            ptr = oHead;
            while(ptr != NULL) {
                printf("%s ", ptr->name);
                for(int i = 0; i < sizeOutputs; i++) {
                    if(i == sizeOutputs - 1) {
                        printf("%d\n", ptr->values[i]);
                    }
                    else {
                       printf("%d ", ptr->values[i]); 
                    }
                }
                ptr = ptr->next;
            }
        }
        // Actual minimization here:
        minimizeAssoc();
        minimizeDisAND();
        minimizeDisOR();
    }
    

    // Printing for Debugging Purposes

    

    struct Gate* ptr2 = circuit;
    // printf("-------\n");
    // printf("Gates:\n");
    while(ptr2 != NULL) {
        bool mux = false;
        if(strcmp(ptr2->name, "MULTIPLEXER") == 0 || strcmp(ptr2->target, "MULTIPLEXER") == 0) {
            mux = true;
        }
        if(isMystery(ptr2->name)) {
            printf("%s ", ptr2->target);
        }
        else {
            printf("%s ", ptr2->name);
        }
        if(mux || strcmp(ptr2->name, "DECODER") == 0 || strcmp(ptr2->target, "DECODER") == 0) {
            printf("%d ", ptr2->n_inputs);
        }
        for(int i = 0; i < ptr2->n_inputs; i++) {
            printf("%s ", ptr2->inputs[i]);
            free(ptr2->inputs[i]);
        }
        if(mux) {
            for(int i = 0; i < (int)log2(ptr2->n_inputs); i++) {
                printf("%s ", ptr2->selectors[i]);
                free(ptr2->selectors[i]);
            }
        }
        for(int i = 0; i < ptr2->n_outputs; i++) {
            if(i == ptr2->n_outputs - 1) {
                printf("%s", ptr2->outputs[i]);
            }
            else {
                printf("%s ", ptr2->outputs[i]);
            }
            free(ptr2->outputs[i]);
        }
        printf("\n");
        struct Gate* temp = ptr2;
        ptr2 = ptr2->next;
        free(temp->inputs);
        free(temp->outputs);
        if(mux) {
            free(temp->selectors);
        }
    }
    // printf("-------\n");
    struct Node* ptr = iHead;
    // free the linked lists
    while(ptr != NULL) {
        // printf("%s ", ptr->name);
        // for(int i = 0; i < sizeOutputs; i++) {
        //     printf("%d ", ptr->values[i]);
        // }
        // printf("\n");
        struct Node* temp = ptr;
        ptr = ptr->next;
        free(temp->values);
        free(temp);
    }
    ptr = oHead;
    while(ptr != NULL) {
        // printf("%s ", ptr->name);
        // for(int i = 0; i < sizeOutputs; i++) {
        //     printf("%d ", ptr->values[i]);
        // }
        // printf("\n");
        struct Node* temp = ptr;
        ptr = ptr->next;
        free(temp->values);
        free(temp);
    }
    ptr = tHead;
    while(ptr != NULL) {
        // printf("%s ", ptr->name);
        // for(int i = 0; i < sizeOutputs; i++) {
        //     printf("%d ", ptr->values[i]);
        // }
        // printf("\n");
        struct Node* temp = ptr;
        ptr = ptr->next;
        free(temp->values);
        free(temp);
    }
}