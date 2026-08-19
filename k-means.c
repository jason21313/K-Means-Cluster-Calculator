# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <time.h>
# include <math.h>
# include <stddef.h>
# include <stdbool.h>
# include "csv_reader.h"

/*
 * Function to sort an array of integers in ascending order.
*/
int* sort_arr(int* arr, int size){
    int* sorted_arr = malloc(size * sizeof(int));
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

int get_min(int* data,int size){
    int min;
    for(int i=0;i<size;i++){
        if(i==0){
            min=data[i];
        }else if(data[i]<min){
            min=data[i];
        }
    }
    return min;
}
int get_max(int* data, int size){
    int max;
    for(int i=0;i<size;i++){
        if(i==0){
            max=data[i];
        }else if(data[i]>max){
            max=data[i];
        }
    }
    return max;
}

float* run_kmeans(int k,int size, int dim,int** data){
    int** cen_locations = malloc(k * sizeof(int*));
    int*** cen_arrs = malloc(k * sizeof(int**));
    for(int i=0;i<k;i++){
        cen_locations[i]=malloc(dim*sizeof(int));
        cen_arrs[i]=calloc(dim,sizeof(int*));
    }
    int* cen_sizes = calloc(k , sizeof(int));

    for(int i = 0; i < k; i++) {
        if(i==0){
            for(int d=0;d<dim;d++){
                int min = get_min(data[d],size);
                int max = get_max(data[d],size);
                cen_locations[i][d] = (rand() % (max-min+1)) + min;
            }
        }else{
            bool unique = false;
            while(!unique) {
                for(int d=0;d<dim;d++){
                    int min = get_min(data[d],size);
                    int max = get_max(data[d],size);
                    cen_locations[i][d] = (rand() % (max-min+1)) + min;
                }
                for(int j = 0; j < i; j++) {
                    for(int d=0;d<dim;d++){
                        if(cen_locations[i][d]==cen_locations[j][d]){
                            unique=false;
                        }else{
                            unique=true;
                        }
                    }
                }
            }
        }
    }

    for(int i=0; i<size; i++){
        //create array to track distances from each centroid
        int* point_distances = malloc(k * sizeof(float));
        //calculate distance from each centroid
        for(int j=0; j<k; j++){
            int temp_distance = 0;
            for(int d=0;d<dim;d++){
                temp_distance+=pow(data[d][i]-cen_locations[j][d],2);
            }
            point_distances[j]= abs(sqrt(temp_distance));
        }
        //sort to find the closest centroid
        int* sorted_distances = sort_arr(point_distances,k);
        //insert into the array of the closest centroid
        for(int l=0; l<k; l++){
            if(point_distances[l] == sorted_distances[0]){
                int** temp = calloc(dim,sizeof(int*));
                for(int d=0;d<dim;d++){
                    temp[d]=realloc(cen_arrs[l][d],(cen_sizes[l]+1)*sizeof(int));
                    if(temp[d]==NULL){
                        free(cen_sizes);
                        for(int i=0;i<k;i++){
                            free(cen_locations[i]);
                            for(int d=0;d<dim;d++){
                                free(cen_arrs[i][d]);
                            }
                            free(cen_arrs[i]);
                        }
                        for(int j=0;j<=d;j++){
                            free(temp[j]);
                        }
                        free(temp);
                        free(cen_locations);
                        free(cen_arrs);
                        printf("error");
                        return NULL;
                    }
                }
                for(int d=0;d<dim;d++){
                    cen_arrs[l][d]=temp[d];
                    cen_arrs[l][d][cen_sizes[l]]=data[d][i];
                }
                cen_sizes[l]++;
            }
        }
        free(point_distances);
        free(sorted_distances);
    }

    float* return_val = malloc((dim*k+1) * sizeof(float));
    float** final_locations = calloc(k,sizeof(float*));
    for(int i=0;i<k;i++){
        final_locations[i]=calloc(dim,sizeof(float));
        float* new_locations=calloc(dim,sizeof(float));
        for(int j=0;j<cen_sizes[i];j++){
            for(int d=0;d<dim;d++){
                new_locations[d]+=cen_arrs[i][d][j];
            }
        }
        for(int d=0;d<dim;d++){
            final_locations[i][d]=new_locations[d]/cen_sizes[i];
            return_val[dim*i+(d+1)]=final_locations[i][d];
        }
        free(new_locations);
    }

    float* variances = calloc(dim,sizeof(float));
    float total_variance = 0;
    for(int i=0;i<k;i++){
        float* temp_variances = calloc(dim,sizeof(float));
        for(int j=0;j<cen_sizes[i];j++){
            for(int d=0;d<dim;d++){
                temp_variances[d]+=pow(cen_arrs[i][d][j]-final_locations[i][d],2);
            }
        }
        for(int d=0; d<dim;d++){
            variances[d]+=temp_variances[d];
        }
        free(temp_variances);
    }
    for(int d=0;d<dim;d++){
        total_variance+=variances[d];
    }
    return_val[0] = total_variance;

    for(int d=0;d<dim;d++){
        free(final_locations[d]);
    }
    free(final_locations);
    free(cen_sizes);
    for(int i=0;i<k;i++){
        free(cen_locations[i]);
        for(int d=0;d<dim;d++){
            free(cen_arrs[i][d]);
        }
        free(cen_arrs[i]);
    }
    free(cen_locations);
    free(cen_arrs);
    free(variances);

    return return_val;
}

int main(){
    printf("Enter a filename: ");
    char* filename = malloc(50 *sizeof(char));
    fgets(filename,50,stdin);
    filename[strlen(filename)-1]='\0';

    printf("Enter the Dimensions of Your Data (1, 2 or 3): ");
    int dim = 0;
    scanf("%d",&dim);
    if(dim <= 0 || dim > 3) {
        printf("Invalid Number of Dimensions. Please enter a value between 1 and 3.\n");
    }

    Info* info = read_file(filename,dim);
    HashMap* dataMap = info->map;
    char** keys = info->keys;
    int size = dataMap->size;
    int** data = calloc(dim,sizeof(int*));
    for(int d=0;d<dim;d++){
        data[d]=calloc(size,sizeof(int));
    }
    for(int i = 0; i < size; i++) {
        int* point = get(dataMap,keys[i]);
        if(point==NULL){
            data=NULL;
            break;
        }
        for(int d=0;d<dim;d++){
            data[d][i]=point[d];
        }
    }
    if(data==NULL){
        for(int i=0;i<size;i++){
            for(int d=0;d<dim;d++){
                free(info->map->entries->key);
                free(info->map->entries->values);
            }
            free(info->map->entries);
            free(info->keys[i]);
        }
        for(int d=0;d<dim;d++){
            free(data[d]);
        }
        free(data);
        free(info->map);
        free(dataMap);
        free(info->keys);
        free(keys);
        free(info);
        return 1;
    }

    printf("Enter the number of clusters (k): ");
    int k = 3;
    scanf("%d", &k);

    bool zscore = false;

    if(k <= 0 || k > size) {
        printf("Invalid number of clusters. Please enter a value between 1 and %d.\n", size);
    }else if(k==size){
        printf("Variance = 0 since k = Size of data\n");
    }else{
        float* best = NULL;
        printf("Finding best Clustering for data with %d clusters\n", k);
        for(int i=0;i<10;i++){
            float* result = run_kmeans(k,size,dim,data);
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
        if(best[0]==-1){
            printf("One of the clusters has no data points assigned to it. Please try again.\n");
        }else{
            printf("\nResult: Variance = %f", best[0]);
            for(int i = 1; i <= k; i++) {
                printf(", Centroid%d = (", i);
                for(int d=0;d<dim;d++){
                    printf("%f",best[dim*(i-1)+(d+1)]);
                    if(d!=dim-1){
                        printf(",");
                    }
                }
                printf(")");
            }
            printf("\n");
        }
        free(best);
    }
    free(info->map->entries->key);
    free(info->map->entries->values);
    free(info->map->entries);
    for(int i=0;i<size;i++){
        free(info->keys[i]);
    }
    for(int d=0;d<dim;d++){
        free(data[d]);
    }
    free(data);
    free(info->map);
    free(info->keys);
    free(info);
}