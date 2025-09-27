#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

#define NUM_STUDENTS 10
#define WAITING_CHAIRS 3

sem_t chairs;
sem_t st_available;
sem_t student_ready;
pthread_mutex_t mutex;

int waiting_students = 0;
int served_students = 0;
int num_left = 0;  

void* student(void* arg) {
    int id = *(int*)arg;
    usleep(rand() % 2000000);
    
    if (sem_trywait(&chairs) == 0) {
        pthread_mutex_lock(&mutex);
        waiting_students++;
        printf("Student %d started waiting (Waiting: %d)\n", id, waiting_students);
        pthread_mutex_unlock(&mutex);
        
        sem_post(&student_ready);
        sem_wait(&st_available);
        
        printf("Student %d getting consultation\n", id);
        sleep(1);
        
        pthread_mutex_lock(&mutex);
        served_students++;
        printf("Student %d finished. Served: %d, Left: %d\n", 
              id, served_students, num_left);
        pthread_mutex_unlock(&mutex);
        
        sem_post(&chairs);
    } else {
        pthread_mutex_lock(&mutex);
        num_left++;
        printf("No chairs! Student %d leaving (Total left: %d)\n", id, num_left);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void* st(void* arg) {
    while (served_students + num_left < NUM_STUDENTS) {
        sem_wait(&student_ready);
        
        pthread_mutex_lock(&mutex);
        if (waiting_students > 0) {
            waiting_students--;
            printf("ST starts. Waiting: %d\n", waiting_students);
            pthread_mutex_unlock(&mutex);
            sem_post(&st_available);
        } else {
            pthread_mutex_unlock(&mutex);
        }
    }
    return NULL;
}

int main() {
    pthread_t students[NUM_STUDENTS], tutor;
    int ids[NUM_STUDENTS];
    
    sem_init(&chairs, 0, WAITING_CHAIRS);
    sem_init(&st_available, 0, 0);
    sem_init(&student_ready, 0, 0);
    pthread_mutex_init(&mutex, NULL);
    srand(time(NULL));
    
    pthread_create(&tutor, NULL, st, NULL);
    for (int i = 0; i < NUM_STUDENTS; i++) {
        ids[i] = i;
        pthread_create(&students[i], NULL, student, &ids[i]);
    }
    
    for (int i = 0; i < NUM_STUDENTS; i++) {
        pthread_join(students[i], NULL);
    }
    
    sem_destroy(&chairs);
    sem_destroy(&st_available);
    sem_destroy(&student_ready);
    pthread_mutex_destroy(&mutex);
    
    printf("Final: %d served, %d left\n", served_students, num_left);
    return 0;
}
