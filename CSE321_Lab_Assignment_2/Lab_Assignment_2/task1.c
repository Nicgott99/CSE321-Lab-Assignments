#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
    int n;
    int* fib_sequence;
    int search_count;
    int* search_indices;
    int* search_results;
} SharedData;

void* compute_fibonacci(void* arg) {
    SharedData* data = (SharedData*)arg;
    int n = data->n;
    
    data->fib_sequence = (int*)malloc((n + 1) * sizeof(int));
    if (data->fib_sequence == NULL) {
        perror("Failed to allocate memory for Fibonacci sequence");
        pthread_exit(NULL);
    }
    
    if (n >= 0) data->fib_sequence[0] = 0;
    if (n >= 1) data->fib_sequence[1] = 1;
    
    for (int i = 2; i <= n; i++) {
        data->fib_sequence[i] = data->fib_sequence[i-1] + data->fib_sequence[i-2];
    }
    
    pthread_exit(NULL);
}

void* search_fibonacci(void* arg) {
    SharedData* data = (SharedData*)arg;

    for (int i = 0; i < data->search_count; i++) {
        int search_index = data->search_indices[i];
        
        if (search_index < 0 || search_index > data->n) {
            data->search_results[i] = -1;
        } else {
            data->search_results[i] = data->fib_sequence[search_index];
        }
    }
    
    pthread_exit(NULL);
}

int main() {
    int n, search_count;
    pthread_t fib_thread, search_thread;
    
    printf("Enter the term of fibonacci sequence:\n");
    scanf("%d", &n);
    
    if (n < 0 || n > 40) {
        printf("Invalid input! n must be between 0 and 40.\n");
        return 1;
    }
    
    printf("How many numbers you are willing to search?:\n");
    scanf("%d", &search_count);
    
    if (search_count <= 0) {
        printf("Invalid number of searches!\n");
        return 1;
    }
    
    SharedData shared_data = {0};
    shared_data.n = n;
    shared_data.search_count = search_count;
    shared_data.search_indices = (int*)malloc(search_count * sizeof(int));
    shared_data.search_results = (int*)malloc(search_count * sizeof(int));
    
    if (shared_data.search_indices == NULL || shared_data.search_results == NULL) {
        perror("Failed to allocate memory for search data");
        return 1;
    }
    
    for (int i = 0; i < search_count; i++) {
        printf("Enter search %d:\n", i + 1);
        scanf("%d", &shared_data.search_indices[i]);
    }
    
    pthread_create(&fib_thread, NULL, compute_fibonacci, &shared_data);
    pthread_join(fib_thread, NULL);
    
    for (int i = 0; i <= n; i++) {
        printf("a[%d] = %d\n", i, shared_data.fib_sequence[i]);
    }
    
    pthread_create(&search_thread, NULL, search_fibonacci, &shared_data);
    pthread_join(search_thread, NULL);
    
    for (int i = 0; i < search_count; i++) {
        printf("result of search #%d = %d\n", i + 1, shared_data.search_results[i]);
    }
    
    free(shared_data.fib_sequence);
    free(shared_data.search_indices);
    free(shared_data.search_results);
    
    return 0;
}
