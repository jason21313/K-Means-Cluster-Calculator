#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#ifndef CSV_READER_H
#define CSV_READER_H

typedef struct {
    char* key;
    int value;
}HashEntry;

typedef struct {
    HashEntry* entries;
    size_t size;
    size_t capacity;
}HashMap;

typedef struct {
    HashMap* map;
    char** keys;
} Info;

int get(HashMap* map, const char* key);

Info* read_file(const char* filename, int dim);

#endif
