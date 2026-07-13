#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void my_sort(double *A, int n);
void fill(double *A, int n);

struct timespec start, end;

// Structure to store measurement data in memory
typedef struct {
    int size;
    double delay;
    double temp;
} Measurement;

// Function to read CPU temperature (compatible with Linux environments)
double get_cpu_temperature() {
    FILE *fp = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
    if (fp == NULL) {
        return -1.0; // Returns -1.0 if file is inaccessible (e.g., non-Linux platform)
    }
    long raw_temp = 0;
    if (fscanf(fp, "%ld", &raw_temp) != 1) {
        raw_temp = 0;
    }
    fclose(fp);
    return (double)raw_temp / 1000.0; // Convert from millidegrees Celsius to Celsius
}

/* ---------- Merge Sort ---------- */

void merge(double A[], int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    double *L = (double *)malloc(n1 * sizeof(double));
    double *R = (double *)malloc(n2 * sizeof(double));

    for (int i = 0; i < n1; i++)
        L[i] = A[left + i];

    for (int j = 0; j < n2; j++)
        R[j] = A[mid + 1 + j];

    int i = 0, j = 0, k = left;

    while (i < n1 && j < n2) {
        if (L[i] <= R[j])
            A[k++] = L[i++];
        else
            A[k++] = R[j++];
    }

    while (i < n1)
        A[k++] = L[i++];

    while (j < n2)
        A[k++] = R[j++];

    free(L);
    free(R);
}

void mergeSort(double A[], int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        mergeSort(A, left, mid);
        mergeSort(A, mid + 1, right);
        merge(A, left, mid, right);
    }
}

void my_sort(double *A, int n) {
    mergeSort(A, 0, n - 1);
}

/* ---------- Fill Array ---------- */

void fill(double *A, int n) {
    for (int i = 0; i < n; i++)
        A[i] = (double)rand() / RAND_MAX;
}

/* ---------- Main ---------- */

int main() {

    srand(time(NULL));

    int max = 4 * 1000 * 1000;
    int min = 1;
    int step = 20 * 1000;

    double *A = (double *)malloc(max * sizeof(double));

    if (A == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }

    // Calculate maximum possible entries to allocate exact memory buffer
    int max_iterations = (max - min) / step + 1;
    Measurement *storage = (Measurement *)malloc(max_iterations * sizeof(Measurement));
    if (storage == NULL) {
        printf("Memory allocation for measurements failed!\n");
        free(A);
        return 1;
    }

    int measurement_count = 0;

    for (int n = min; n < max; n += step) {

        fill(A, n);

        // Read CPU temperature right before the execution block to avoid skewed timing metrics
        double current_temp = get_cpu_temperature();

        clock_gettime(CLOCK_MONOTONIC, &start);
        my_sort(A, n);
        clock_gettime(CLOCK_MONOTONIC, &end);

        double tdiff = (end.tv_sec - start.tv_sec) + 1e-9 * (end.tv_nsec - start.tv_nsec);

        // Print the progress in terminal
        double progress = ((double)(n - min) / (max - min)) * 100.0;
        printf("progress %.1f%%, size %d, time %.6f sec, temp %.2f C\n", progress, n, tdiff, current_temp);

        // Save data points into the allocated memory array
        if (measurement_count < max_iterations) {
            storage[measurement_count].size = n;
            storage[measurement_count].delay = tdiff;
            storage[measurement_count].temp = current_temp;
            measurement_count++;
        }
    }

    // Write all stored metrics out to a CSV file
    FILE *csv_file = fopen("measurements.csv", "w");
    if (csv_file == NULL) {
        printf("Error: Could not open measurements.csv for writing.\n");
    } else {
        fprintf(csv_file, "Array Size,Delay (sec),CPU Temperature (C)\n");
        for (int i = 0; i < measurement_count; i++) {
            fprintf(csv_file, "%d,%.6f,%.2f\n", storage[i].size, storage[i].delay, storage[i].temp);
        }
        fclose(csv_file);
        printf("\nSuccessfully written data to measurements.csv\n");
    }

    // Clean up allocated arrays
    free(storage);
    free(A);

    return 0;
}