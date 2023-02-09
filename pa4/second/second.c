#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<limits.h>
#include<stdbool.h>

struct Line {
    unsigned long tag;
    unsigned long address;
    int age;
    int last_used;
    int valid_bit;
};
int findHash(unsigned long address, int num_sets, int block_size, unsigned long* tag) {
    int n = log2(num_sets);
    int b = log2(block_size);
    (*tag) = address >> (b+n);
    return ((address >> b)&(num_sets-1));
}

int minimum_age(int num_sets, int association, int hash, struct Line cache[num_sets][association]) {
    int min = INT_MAX;
    int index = 0;
    for(int i = 0; i < association; i++) {
        if(cache[hash][i].valid_bit == 1 && cache[hash][i].age >= 0 && cache[hash][i].age < min) {
            index = i;
            min = cache[hash][i].age;
        }
    }
    return index;
}

int minimum_lastused(int num_sets, int association, int hash, struct Line cache[num_sets][association]) {
    int min = INT_MAX;
    int index = 0;
    for(int i = 0; i < association; i++) {
        if(cache[hash][i].valid_bit == 1 && cache[hash][i].last_used >= 0 && cache[hash][i].last_used < min) {
            index = i;
            min = cache[hash][i].last_used;
        }
    }
    return index;
}

void insert_L2(unsigned long address, int association2, int num_sets2, char policy2[50], int block_size, int iteration, struct Line cache2[num_sets2][association2]) {
    unsigned long tag;
    int hash = findHash(address, num_sets2, block_size, &tag);
    bool flag = true;
    for(int i = 0; i < association2; i++) {
        // Place into empty line
        if(cache2[hash][i].valid_bit == 0) {
            cache2[hash][i].valid_bit = 1;
            cache2[hash][i].tag = tag;
            cache2[hash][i].address = address;
            cache2[hash][i].age = iteration;
            cache2[hash][i].last_used = iteration;
            flag = false;
            break;
        }
    }
    // Replace
    if(flag) {
        int index;
        if(strcmp(policy2, "fifo") == 0) {
            index = minimum_age(num_sets2, association2, hash, cache2);
        }
        else {
            index = minimum_lastused(num_sets2, association2, hash, cache2);
        }
        cache2[hash][index].tag = tag;
        cache2[hash][index].address = address;
        cache2[hash][index].age = iteration;
        cache2[hash][index].last_used = iteration;
    }
}

void insert_L1(unsigned long address, int association1, int association2, int num_sets1, int num_sets2, char policy1[50], char policy2[50], int block_size, int iteration, struct Line cache1[num_sets1][association1], struct Line cache2[num_sets2][association2]) {
    unsigned long tag;
    int hash = findHash(address, num_sets1, block_size, &tag);
    bool flag = true;
    for(int i = 0; i < association1; i++) {
        // Place into empty line
        if(cache1[hash][i].valid_bit == 0) {
            cache1[hash][i].valid_bit = 1;
            cache1[hash][i].tag = tag;
            cache1[hash][i].address = address;
            cache1[hash][i].last_used = iteration;
            cache1[hash][i].age = iteration;
            flag = false;
            break;
        }
    }
    // Replace following policy
    if(flag) {
        int index;
        if(strcmp(policy1, "fifo") == 0) {
            index = minimum_age(num_sets1, association1, hash, cache1);
        }
        else {
            index = minimum_lastused(num_sets1, association1, hash, cache1);
        }
        //temporarily store old line's values and insert into L2 cache
        unsigned long temp_address = cache1[hash][index].address;
        insert_L2(temp_address, association2, num_sets2, policy2, block_size, iteration, cache2);
        //replace
        cache1[hash][index].tag = tag;
        cache1[hash][index].address = address;
        cache1[hash][index].last_used = iteration;
        cache1[hash][index].age = iteration;
    }
}
int main(int argc, char* argv[]) {
    if(argc == 0) {
        return 0;
    }
    // 5 Inputs: <l1 cache size> <assoc:n> <cache policy> <block size> <l2 cache size> <assoc:n> <cache policy> <trace file>
    char policy1[50];
    char policy2[50];
    strcpy(policy1, argv[3]);
    strcpy(policy2, argv[7]);

    // Read the associations
    char temp1[50];
    char temp2[50];
    strcpy(temp1, argv[2]);
    for(int x = 6; x < strlen(temp1); x++) {
        temp2[x-6] = temp1[x];
        if(x == strlen(temp1)-1) {
            temp2[x-6+1] = '\0';
        }
    }
    int association1 = atoi(temp2);
    strcpy(temp1, argv[6]);
    for(int x = 6; x < strlen(temp1); x++) {
        temp2[x-6] = temp1[x];
        if(x == strlen(temp1)-1) {
            temp2[x-6+1] = '\0';
        }
    }
    int association2 = atoi(temp2);

    //Create the l1 cache
    int cache_size1 = atoi(argv[1]);
    int block_size = atoi(argv[4]);
    int num_sets1 = cache_size1 / (association1 * block_size);
    struct Line cache1[num_sets1][association1];
    for(int i = 0; i < num_sets1; i++) {
        for(int j = 0; j < association1; j++) {
            cache1[i][j].valid_bit = 0;
            cache1[i][j].age = -1;
            cache1[i][j].last_used = -1;
            cache1[i][j].address = 0;
            cache1[i][j].tag = 0;
        }
    }

    //Create the l2 cache
    int cache_size2 = atoi(argv[5]);
    int num_sets2 = cache_size2 / (association2 * block_size);
    struct Line cache2[num_sets2][association2];
    for(int i = 0; i < num_sets2; i++) {
        for(int j = 0; j < association2; j++) {
            cache2[i][j].valid_bit = 0;
            cache2[i][j].age = -1;
            cache2[i][j].last_used = -1;
            cache2[i][j].address = 0;
            cache2[i][j].tag = 0;
        }
    }

    // Initiate the return variables
    int memread = 0;
    int memwrite = 0;
    int l1hit = 0;
    int l1miss = 0;
    int l2hit = 0;
    int l2miss = 0;

    // Read the text file
    FILE * file;
    file = fopen(argv[8], "r");
    unsigned long address;
    char mode[48];
    // To identify a cache line's age or when it was last accessed
    int iteration = 0;

    while(!feof(file)) {
        fscanf(file, "%s", mode);
        fscanf(file, "%lx\n", &address);
        iteration++;
        unsigned long tag1;
        int hashl1 = findHash(address, num_sets1, block_size, &tag1);
        unsigned long tag2;
        int hashl2 = findHash(address, num_sets2, block_size, &tag2);
        // printf("Hash1: %d Hash2: %d\n", hashl1, hashl2);

        //Check for hit in L1
        bool miss1 = true;
        for(int i = 0; i < association1; i++) {
            if(cache1[hashl1][i].valid_bit == 1 && cache1[hashl1][i].tag == tag1){
                l1hit++;
                cache1[hashl1][i].last_used = iteration;
                // if(strcmp(policy1, "lru") == 0) {
                //     cache1[hashl1][i].last_used = iteration;
                // }
                if(strcmp(mode, "W") == 0) {
                    memwrite++;
                }
                miss1 = false;
                break;
            }
        }
        //Check L2 if L1 Misses
        bool miss2 = true;
        if(miss1) {
            l1miss++;
            for(int i = 0; i < association2; i++) {
                if(cache2[hashl2][i].valid_bit == 1 && cache2[hashl2][i].tag == tag2) {
                    // printf("Iteration: %d L2 Hit\n", iteration);
                    l2hit++;
                    if(strcmp(mode, "W") == 0) {
                        memwrite++;
                    }
                    miss2 = false;
                    cache2[hashl2][i].valid_bit = 0;
                    cache2[hashl2][i].age = -1;
                    cache2[hashl2][i].last_used = -1;
                    cache2[hashl2][i].address = 0;
                    cache2[hashl2][i].tag = 0;
                    insert_L1(address, association1, association2, num_sets1, num_sets2, policy1, policy2, block_size, iteration, cache1, cache2);
                    break;
                }
            }
        }
        if(miss1 && miss2) {
            if(strcmp(mode, "W") == 0) {
                memwrite++;
            }
            memread++;
            l2miss++;
            insert_L1(address, association1, association2, num_sets1, num_sets2, policy1, policy2, block_size, iteration, cache1, cache2);
        }
        
    }

    // printf("Cache 1: \n");
    // for(int i = 0; i < num_sets1; i++) {
    //     printf("Set %d\n", i);
    //     for(int j = 0; j < association1; j++) {
    //         printf("Block %d Address: %lx Age: %d LRU: %d\n", j, cache1[i][j].tag, cache1[i][j].age, cache1[i][j].last_used);
    //     } 
    // }
    // printf("\n\n");
    // printf("Cache 2: \n");
    // for(int i = 0; i < num_sets2; i++) {
    //     printf("Set %d\n", i);
    //     for(int j = 0; j < association2; j++) {
    //         printf("Block %d Address: %lx Age: %d LRU: %d\n", j, cache2[i][j].tag, cache2[i][j].age, cache2[i][j].last_used);
    //     }
    // }

    printf("memread:%d\n", memread);
    printf("memwrite:%d\n", memwrite);
    printf("l1cachehit:%d\n", l1hit);
    printf("l1cachemiss:%d\n", l1miss);
    printf("l2cachehit:%d\n", l2hit);
    printf("l2cachemiss:%d\n", l2miss);
}