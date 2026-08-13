#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


typedef struct {
    char* key;
    int value;
} HashEntry;

typedef struct {
    HashEntry* entries;
    size_t size;
    size_t capacity;
} HashMap;

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



int* read_file(const char* filename, int dim) {
    if(dim==1){
        //create a dict with id and int
    }else if(dim==2){
        //create a dict with id and tuple of 2 ints
    }else if(dim==3){
        //create a dict with id and tuple of 3 ints
    }else{
        return NULL;
    }
    HashMap* map = malloc(sizeof(HashMap));
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
            if(tracker==0){
                key = token;
                printf("Key: %s\n", key);
                token = strtok(NULL, ",");
                tracker++;
            }else{
                int value = atoi(token);
                printf("Value: %d\n", value);
                insert(map, key, value);
                printf("Inserted: %s -> %d\n", key, get(map, key));
                tracker=0;
                token = strtok(NULL, ",");
            }
        }
    }
    free(map->entries);
    free(map);
    fclose(file);
    return NULL;
}

int main() {
    const char* filename = "test.csv";
    int dim = 1;
    read_file(filename, dim);
    return 0;
}