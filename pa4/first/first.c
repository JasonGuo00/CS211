#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<limits.h>
#include<stdbool.h>

struct Line {
    unsigned long tag;
    int age;
    int valid_bit;

};

int findHash(unsigned long address, int num_sets, int block_size, unsigned long* tag) {
    int n = log2(num_sets);
    int b = log2(block_size);
    (*tag) = address >> (b+n);
    return ((address >> b)&(num_sets-1));
}

int minimum(int num_sets, int association, int hash, struct Line cache[num_sets][association]) {
    int min = INT_MAX;
    int index = 0;
    for(int i = 0; i < association; i++) {
        if(cache[hash][i].age < min && cache[hash][i].valid_bit == 1) {
            index = i;
            min = cache[hash][i].age;
        }
    }
    return index;
}

void read_write(int num_sets, int association, int hash, struct Line cache[num_sets][association], unsigned long tag, int iteration) {
    
    bool flag = true;

    for(int i = 0; i < association; i++) {
        if(cache[hash][i].valid_bit == 0) {
            cache[hash][i].valid_bit = 1;
            cache[hash][i].tag = tag;
            cache[hash][i].age = iteration;
            flag = false;
            break;
        }
    }
    if(flag) {
        int index = minimum(num_sets, association, hash, cache);
        cache[hash][index].tag = tag;
        cache[hash][index].age = iteration;
    }
    
}

int main(int argc, char* argv[]) {
    if(argc == 0) {
        return 0;
    }
    // 5 Inputs: <cache size> <assoc:n> <cache policy> <block size> <trace file>
    char policy[50];
    strcpy(policy, argv[3]);

    // Read the association
    char temp1[50];
    char temp2[50];
    strcpy(temp1, argv[2]);
    for(int x = 6; x < strlen(temp1); x++) {
        temp2[x-6] = temp1[x];
        if(x == strlen(temp1)-1) {
            temp2[x-6+1] = '\0';
        }
    }

    //Create the cache
    int cache_size = atoi(argv[1]);
    int association = atoi(temp2);
    int block_size = atoi(argv[4]);
    int num_sets = cache_size / (association * block_size);
    struct Line cache[num_sets][association];
    for(int i = 0; i < num_sets; i++) {
        for(int j = 0; j < association; j++) {
            cache[i][j].valid_bit = 0;
        }
    }

    // Initiate the return variables
    int memread = 0;
    int memwrite = 0;
    int hit = 0;
    int miss = 0;

    // Read the text file
    FILE * file;
    file = fopen(argv[5], "r");
    unsigned long address;
    char mode[48];
    // To identify a cache line's age or when it was last accessed
    int iteration = 0;

    while(!feof(file)) {
        fscanf(file, "%s", mode);
        fscanf(file, "%lx\n", &address);
        iteration++;
        unsigned long tag;
        int hash = findHash(address, num_sets, block_size, &tag);

        // Read
        if(strcmp(mode, "R") == 0) {
            for(int i = 0; i < association; i++) {
                // Read cache hit
                if(cache[hash][i].tag == tag) {
                    hit++;
                    if(strcmp(policy, "lru") == 0) {
                        cache[hash][i].age = iteration;
                    }
                    break;
                }
                // Read cache miss
                else if(i == association-1) {
                    miss++;
                    memread++;
                    read_write(num_sets, association, hash, cache, tag, iteration);
                }
            }
        }

        // Write
        else {
            for(int i = 0; i < association; i++) {
                // Write hit
                if(cache[hash][i].tag == tag) {
                    hit++;
                    memwrite++;
                    if(strcmp(policy, "lru") == 0) {
                        cache[hash][i].age = iteration;
                    }
                    break;
                }
                // Write miss
                else if(i == association-1) {
                    miss++;
                    memread++;
                    memwrite++;
                    read_write(num_sets, association, hash, cache, tag, iteration);
                }
            }
        }
    }
    printf("memread:%d\n", memread);
    printf("memwrite:%d\n", memwrite);
    printf("cachehit:%d\n", hit);
    printf("cachemiss:%d\n", miss);
}