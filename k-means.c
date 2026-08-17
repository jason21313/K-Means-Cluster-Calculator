# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <time.h>
# include <math.h>
# include <stddef.h>
# include <stdbool.h>
# include "csv_reader.h"


//example data set
int dat[] = {6, 8, 3, 9, 1};

/*
 * Function to sort an array of integers in ascending order.
*/
int* sort_arr(int* arr, int size){
    int* sorted_arr = malloc(sizeof(int));
    for(int i = 0; i < size; i++) {
        sorted_arr[i]=arr[i];
    }
    for(int i = 0; i < size - 1; i++) {
        for(int j = 0; j < size - i - 1; j++) {
            if(sorted_arr[j] > sorted_arr[j + 1]) {
                int temp = sorted_arr[j];
                sorted_arr[j] = sorted_arr[j + 1];
                sorted_arr[j + 1] = temp;
            }
        }
    }
    return sorted_arr;
}

int* sort_map(HashMap* map, char** keys){
    int* sorted_data = malloc(sizeof(int));
    for(int i = 0; i < map->size; i++) {
        printf("%s\n",keys[i]);
        sorted_data[i]=get(map,keys[i]);
    }
    for(int i = 0; i < map->size - 1; i++) {
        for(int j = 0; j < map->size - i - 1; j++) {
            if(sorted_data[j] > sorted_data[j + 1]) {
                int temp = sorted_data[j];
                sorted_data[j] = sorted_data[j + 1];
                sorted_data[j + 1] = temp;
            }
        }
    }
    return sorted_data;
}

/*
 * Function to calculate the variance of an array of integers.
 * @param arr: Pointer to the array for which to calculate variance.
 * @param size: The number of elements in the array.
 * @return: The calculated variance.
 */
int calc_variance(int *arr, int size) {
    int e = 0;
    int e2 = 0;
    for(int i = 0; i < size; i++) {
        e += arr[i];
        e2 += arr[i] * arr[i];
    }
    int variance = (e2 / size) - (e * e / (size * size));
    return variance;
}

/*
 * Function to run the k-means algorithm.
 * @param data: Pointer to the array of data points.
 * @param k: The number of clusters.
 * @param size: The number of elements in the data array.
 * @return: Pointer to the array containing the variance and centroid locations.
 */
int* run_d(int*data, int k, int size){
    srand(time(NULL));
    //creates arrays to hold the centroid locations, sizes, and data points assigned to each centroid
    int* cen_locations = malloc(k * sizeof(int));
    int* cen_sizes = malloc(k * sizeof(int));
    int** cen_arrs = malloc(k * sizeof(int*));
    //ensure that the centroid locations are unique
    for(int i = 0; i < k; i++) {
        if(i==0){
            cen_locations[i] = (rand() % (data[size-1]-data[0]+1)) + data[0];
        }else{
            bool unique = false;
            while(!unique) {
                cen_locations[i] = (rand() % (data[size-1]-data[0]+1)) + data[0];
                for(int j = 0; j < i; j++) {
                    if(cen_locations[i] == cen_locations[j]) {
                        unique = false;
                    } else {
                        unique = true;
                    }
                }
            }
        }
        cen_sizes[i] = 0;
        cen_arrs[i] = NULL;
    }

    //loop through each data point to find the closest centroid and assign the data point to that centroid's array
    for(int i=0; i<size; i++){
        //create array to track distances from each centroid
        int* point_distances = malloc(k * sizeof(int));
        //calculate distance from each centroid
        for(int j=0; j<k; j++){
            point_distances[j] = abs(data[i]-cen_locations[j]);
        }
        //sort to find the closest centroid
        int* sorted_distances = sort_arr(data,size);
        //insert into the array of the closest centroid
        for(int l=0; l<k; l++){
            if(point_distances[l] == sorted_distances[0]){
                cen_arrs[l] = realloc(cen_arrs[l], (cen_sizes[l] + 1) * sizeof(int));
                cen_arrs[l][cen_sizes[l]] = data[i];
                cen_sizes[l]++;
            }
        }
    }

    //calculate the variance of each centroid's array and return the total variance and centroid locations
    int variance = 0;
    int* return_val = malloc((k+1) * sizeof(int));
    for(int i=0; i<k; i++){
        if(cen_sizes[i] == 0){
            return_val[0] = -1;
            return return_val;
        }else{
            variance += calc_variance(cen_arrs[i], cen_sizes[i]);
            return_val[i+1] = cen_locations[i];
        }

    }
    return_val[0] = variance;

    free(cen_locations);
    free(cen_sizes);
    free(cen_arrs);

    return return_val;
}

/*
 * Main function to execute the k-means clustering algorithm.
 */
int main() {
    printf("Enter a filename: ");
    char* filename = malloc(50 *sizeof(char));
    fgets(filename,50,stdin);
    filename[strlen(filename)-1]='\0';

    Info* info = read_file(filename, 1);
    HashMap* dataMap = info->map;
    char** keys = info->keys;

    int* sorted_data = sort_map(dataMap,keys);
    for(int i = 0;i<dataMap->size;i++){
        printf("%d\n",sorted_data[i]);
    }


    printf("Enter the number of clusters (k): ");
    int k = 3;
    scanf("%d", &k);
    int size = dataMap->size;
    printf("%d\n",size);

    if(k <= 0 || k > size) {
        printf("Invalid number of clusters. Please enter a value between 1 and %d.\n", size);
        return 1;
    }else if(k==size){
        printf("Variance = 0 since k = Size of data\n");
        return 0;
    }else{
        int *best = NULL;
        // int *sorted_data = NULL;
        printf("Finding best Clustering for data with %d clusters\n", k);
        for(int i = 0; i < 1; i++) {
            int *result = run_d(sorted_data, k, size);
            if(i==0){
                best = result;
            }else if (result[0] != -1) {
                if(result[0] < best[0]){
                    free(best);
                    best = result;
                }else{
                    free(result);
                }
            }
        }
        if(best[0] == -1){
            printf("One of the clusters has no data points assigned to it. Please try again.\n");
            free(best);
            return 1;
        }else{
            printf("\nResult: Variance = %d", best[0]);
            for(int i = 1; i <= k; i++) {
                printf(", Centroid%d = %d", i, best[i]);
            }
            printf("\n");
            return 0;
        }
    }

    free(info);
    free(keys);
    free(filename);
    free(dataMap->entries);
    free(dataMap);

    return 0;
}