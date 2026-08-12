# include <stdio.h>
# include <stdlib.h>
# include <time.h>
# include <math.h>
# include <stddef.h>
# include <stdbool.h>

int dat[] = {6, 8, 3, 9, 1};

int* sort(int* arr, int size) {
    // Implementation for sorting
    int* sorted_arr = malloc(size * sizeof(int));
    for(int i = 0; i < size; i++) {
        sorted_arr[i] = arr[i];
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


int* run_d(int*data, int k, int size){
    srand(time(NULL));
    int* cen_locations = malloc(k * sizeof(int));
    int* cen_sizes = malloc(k * sizeof(int));
    int** cen_arrs = malloc(k * sizeof(int*));
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


    //loop through each data point
    for(int i=0; i<size; i++){
        //create array to track distances from each centroid
        int* point_distances = malloc(k * sizeof(int));
        //calculate distance from each centroid
        for(int j=0; j<k; j++){
            point_distances[j] = abs(data[i]-cen_locations[j]);
        }
        //sort to find the closest centroid
        int* sorted_distances = sort(point_distances, k);
        for(int l=0; l<k; l++){
            if(point_distances[l] == sorted_distances[0]){
                cen_arrs[l] = realloc(cen_arrs[l], (cen_sizes[l] + 1) * sizeof(int));
                cen_arrs[l][cen_sizes[l]] = data[i];
                cen_sizes[l]++;
            }
        }
    }

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

int main() {
    printf("Enter the number of clusters (k): ");
    int k = 3;
    scanf("%d", &k);
    int size = sizeof(dat) / sizeof(dat[0]);
    if(k <= 0 || k > size) {
        printf("Invalid number of clusters. Please enter a value between 1 and %d.\n", size);
        return 1;
    }else if(k==size){
        printf("Variance = 0 since k = Size of data\n");
        return 0;
    }else{
        int *best = NULL;
        int *sorted_data = sort(dat, size);
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
    return 0;
}