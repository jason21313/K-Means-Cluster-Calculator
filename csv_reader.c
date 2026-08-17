#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "csv_reader.h"

unsigned int hash(const char* key, size_t capacity) {
    unsigned int hash_val = 5381;
    int c;
    while ((c = *key++)) {
        hash_val = ((hash_val << 5) + hash_val) + c; /* hash * 33 + c */
    }
    return hash_val % capacity;
}

void insert(HashMap* map, const char* key, int value) {
    unsigned int index  = hash(key, map->capacity);
    while (map->entries[index].key !=NULL){
        if(strcmp(map->entries[index].key, key) == 0) {
            map->entries[index].value = value;
            return;
        }
        index = (index + 1) % map->capacity;
    }

    map->entries[index].key = strdup(key);
    map->entries[index].value = value;
    map->size++;
}

int get(HashMap* map, const char* key){
    unsigned int index = hash(key, map->capacity);
    unsigned int start_index = index;
    while(map->entries[index].key != NULL){
        if(strcmp(map->entries[index].key, key) == 0){
            return map->entries[index].value;
        }
        index = (index + 1) % map->capacity;

        if(index == start_index){
            break; // We've looped back to the start
        }
    }
    return -1; // Key not found
}

int count_rows(const char* filename){
    int rows=0;
    FILE *file = fopen(filename,"r");
    char buffer[1024]={0};
    while(fgets(buffer,sizeof(buffer),file)!=NULL){
        rows++;
    }
    fclose(file);
    return rows;
}

Info* read_file(const char* filename, int dim) {
    HashMap* map = malloc(sizeof(HashMap));
    int rows = count_rows(filename);
    if(rows<=0){
        return NULL;
    }
    char** keys = malloc(sizeof(char*)*rows);
    map->capacity = 100; // Initial capacity
    map->size = 0;
    map->entries = calloc(map->capacity, sizeof(HashEntry));

    FILE *file = fopen(filename, "r");
    char buffer[1024] = {0};
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    int tracker = 0;
    const char *key;
    while(fgets(buffer, sizeof(buffer), file)!=NULL) {
        buffer[strcspn(buffer, "\n")] = 0; // Remove newline character
        char* token = strtok(buffer, ",");
        while(token != NULL) {
            if(tracker%2==0){
                key = token;
                keys[tracker/2]=strdup(key);
                printf("Key: %s\n", key);
                token = strtok(NULL, ",");
                tracker++;
            }else{
                int value = atoi(token);
                printf("Value: %d\n", value);
                insert(map, keys[(tracker-1)/2], value);
                printf("Inserted: %s -> %d\n", key, get(map, key));
                tracker++;
                token = strtok(NULL, ",");
            }
        }
    }
    fclose(file);
    Info* info = malloc(sizeof(Info));
    info->keys=keys;
    info->map=map;
    return info;
}

int main_csv(){
    printf("1");
    return 1;
}
