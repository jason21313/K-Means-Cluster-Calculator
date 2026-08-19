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

void insert(HashMap* map, const char* key, int* values) {
    unsigned int index  = hash(key, map->capacity);
    while (map->entries[index].key !=NULL){
        if(strcmp(map->entries[index].key, key) == 0) {
            map->entries[index].values = values;
            return;
        }
        index = (index + 1) % map->capacity;
    }

    map->entries[index].key = strdup(key);
    map->entries[index].values = values;
    map->size++;
}

int* get(HashMap* map, const char* key){
    unsigned int index = hash(key, map->capacity);
    unsigned int start_index = index;
    while(map->entries[index].key != NULL){
        if(strcmp(map->entries[index].key, key) == 0){
            return map->entries[index].values;
        }
        index = (index + 1) % map->capacity;

        if(index == start_index){
            break; // We've looped back to the start
        }
    }
    return NULL; // Key not found
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
    map->dim=dim;

    FILE *file = fopen(filename, "r");
    char buffer[1024] = {0};
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    int tracker = 0;
    int tracker_d=0;
    const char *key;
    int val_x;
    int val_y;
    int val_z;
    while(fgets(buffer, sizeof(buffer), file)!=NULL) {
        buffer[strcspn(buffer, "\n")] = 0; // Remove newline character
        char* token = strtok(buffer, ",");
        while(token != NULL) {
            if(dim==1){
                if(tracker%2==0){
                key = token;
                keys[tracker/2]=strdup(key);
                printf("Key: %s\n", key);
                token = strtok(NULL, ",");
                tracker++;
            }else{
                int* values = calloc(dim,sizeof(int));
                values[0] = atoi(token);
                printf("Value: %d\n", values[0]);
                insert(map, keys[(tracker-1)/2], values);
                printf("Inserted: %s -> %d\n", key, get(map, key)[0]);
                tracker++;
                token = strtok(NULL, ",");
            }
            }else if(dim==2){
                if(tracker_d==0){
                    key = token;
                    keys[(tracker)/3]=strdup(key);
                    printf("Key: %s\n", key);
                    token = strtok(NULL, ",");
                    tracker++;
                    tracker_d++;
                }else if(tracker_d==1){
                    val_x = atoi(token);
                    token = strtok(NULL, ",");
                    tracker++;
                    tracker_d++;
                }else{
                    int* values = calloc(dim,sizeof(int));
                    values[0]=val_x;
                    values[1] = atoi(token);
                    printf("Value: (%d,%d)\n", values[0],values[1]);
                    insert(map, keys[(tracker-2)/3], values);
                    printf("Inserted: %s -> (%d,%d)\n", key, get(map, key)[0],get(map,key)[1]);
                    tracker++;
                    tracker_d=0;
                    token = strtok(NULL, ",");
                }
            }else if(dim==3){
                if(tracker_d==0){
                    key = token;
                    keys[(tracker)/4]=strdup(key);
                    printf("Key: %s\n", key);
                    token = strtok(NULL, ",");
                    tracker++;
                    tracker_d++;
                }else if(tracker_d==1){
                    val_x = atoi(token);
                    token = strtok(NULL, ",");
                    tracker++;
                    tracker_d++;
                }else if(tracker_d==2){
                    val_y = atoi(token);
                    token = strtok(NULL, ",");
                    tracker++;
                    tracker_d++;
                }else{
                    int* values = calloc(dim,sizeof(int));
                    values[0]=val_x;
                    values[1]=val_y;
                    values[2] = atoi(token);
                    printf("Value: (%d,%d,%d)\n", values[0],values[1],values[2]);
                    insert(map, keys[(tracker-3)/4], values);
                    printf("Inserted: %s -> (%d,%d,%d)\n", key, get(map, key)[0],get(map,key)[1],get(map,key)[2]);
                    tracker++;
                    tracker_d=0;
                    token = strtok(NULL, ",");
                }
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
    Info* info1 = read_file("test.csv",1);
    printf("\n");
    Info* info2 = read_file("test2.csv",2);
    printf("\n");
    Info* info3 = read_file("test3.csv",3);
    return 1;
}
