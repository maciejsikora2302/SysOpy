#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <math.h>
#include <time.h>

int max_delay = 3;
int max_cut_time = 3;

int K;
int customer_served_counter = 0;
int        free_seats;
bool       barber_ready = true;
bool       customer_ready = false;
bool       barber_busy = false;
pthread_t  current_customer;
int curr_customer_id;

// mutex
pthread_mutex_t room_mutex;
pthread_mutex_t barber_mutex;
pthread_mutex_t customer_mutex;

// mutex cond
pthread_cond_t room_cond_var;
pthread_cond_t barber_cond_var;
pthread_cond_t customer_cond_var;

void error_msg(const char* msg) {
    printf("Error: %s\n", msg);
    exit(EXIT_FAILURE);
}

void* customer_th(void* arg) {
    pthread_t thread = pthread_self();
    int* id_pointer = (int*) arg;
    int  id = *id_pointer;

    while (1) {
        pthread_mutex_lock(&room_mutex);
        if (free_seats > 0 || !barber_busy) { 
            if (!barber_busy) {
               printf("Klient: budzę golibrodę - %lx (%d)\n", thread, id);
               barber_busy = true;
            } else {
                printf("Klient: poczekalnia | Wolne miejsca: %d - %lx (%d)\n", free_seats - 1, thread, id);
            }
            free_seats -= 1;
            // Notify barber
            pthread_cond_broadcast(&room_cond_var);
            pthread_mutex_unlock(&room_mutex);
           
            // Wait for barber to fetch customer
            pthread_mutex_lock(&barber_mutex);
            while (barber_ready) {
                pthread_cond_wait(&barber_cond_var, &barber_mutex);
            }
            
            barber_ready = true;
            pthread_cond_broadcast(&barber_cond_var); 
            pthread_mutex_unlock(&barber_mutex);

            pthread_mutex_lock(&customer_mutex);
            current_customer = thread;
            curr_customer_id = id;
            customer_ready = true;
            pthread_cond_broadcast(&customer_cond_var);
            pthread_mutex_unlock(&customer_mutex);

            // Wait until barber finishes cutting this customers's hair
            pthread_mutex_lock(&customer_mutex);
            while (customer_ready) {
                pthread_cond_wait(&customer_cond_var, &customer_mutex);
            }
            customer_ready = true;
            // Leave after 
            printf("Klient: golibroda skończył mnie golić, opuszczam salon - %lx (%d)\n", thread, id);
            pthread_cond_broadcast(&customer_cond_var);
            pthread_mutex_unlock(&customer_mutex);

            return NULL;
        } else {
            printf("Klient: nie ma miejsca w poczekalni, opuszczam salon - %lx (%d)\n", thread, id);
            pthread_mutex_unlock(&room_mutex);
            sleep(1 + rand() % max_delay);
        }
    }
    
    return NULL;
}

void* barber_th(void* arg) {
    while (1) {
        // sleep until customer comes in
        pthread_mutex_lock(&room_mutex);
        while (free_seats == K) {
            // If there is no one waiting then go to sleep.
            printf("Golibroda: Czas na drzemkę. zzzzzz...\n");
            barber_busy = false;
            pthread_cond_wait(&room_cond_var, &room_mutex);
        }

        barber_busy = true;
        free_seats += 1;

        // notify customers that barber is ready
        pthread_mutex_lock(&barber_mutex);
        barber_ready = false;
        pthread_cond_broadcast(&barber_cond_var); 
        pthread_mutex_unlock(&barber_mutex);

        // fetch customer from wating room
        pthread_mutex_lock(&customer_mutex);
        while (!customer_ready) {
            pthread_cond_wait(&customer_cond_var, &customer_mutex);
        }

        // Finally cut hair of this poor customer
        printf("Golibroda: %d klientów oczekuje, pracuję obecnie nad klientem -> %lx (%d)\n", K - free_seats, current_customer, curr_customer_id);
        pthread_mutex_unlock(&room_mutex);
        sleep((rand() % max_cut_time) + 1);
        customer_served_counter += 1;

        // Once finished notify customer that he's done ;)
        customer_ready = false;
        printf("Golibroda: skończyłem pracę nad klientem %lx (%d) czekam aż wyjdzie z salonu\n", current_customer, curr_customer_id);
        pthread_cond_broadcast(&customer_cond_var); 
        pthread_mutex_unlock(&customer_mutex);

        // Wait for customer to leave 
        pthread_mutex_lock(&customer_mutex);
        while (!customer_ready) {
            pthread_cond_wait(&customer_cond_var, &customer_mutex);
        }
        customer_ready = false;
        pthread_mutex_unlock(&customer_mutex);
    }

    return NULL;
}


int main(int charc, char* argv[]) {
    if (charc < 3)
        error_msg("Not enuagh arguments!");

    K = atoi(argv[1]);
    int N = atoi(argv[2]);

    srand(time(NULL));
    pthread_t* threads = malloc(sizeof(pthread_t) * (N + 1));
    printf("===================================================\n");
    printf("SleepingBarberProblem: \n\t- %d miejsc w poczekalni\n\t- %d klientów  \n", K, N);
    printf("===================================================\n");

    // Create barber thread
    int i = 0;
    pthread_create(&threads[i++], NULL, barber_th, NULL);

    // Spawn threads with random delay
    int* waitFor = malloc(sizeof(int) * N);
    int maxWait = 0;
    for(i = 0; i < N; i++) { 
        waitFor[i] = rand() % max_delay;
        if (maxWait < waitFor[i])
            maxWait = waitFor[i];
    }

    free_seats = K;

    pthread_mutex_init(&room_mutex, NULL);
    pthread_mutex_init(&barber_mutex, NULL);
    pthread_mutex_init(&customer_mutex, NULL);

    pthread_cond_init(&barber_cond_var, NULL);
    pthread_cond_init(&room_cond_var, NULL);
    pthread_cond_init(&customer_cond_var, NULL);

    // Create customers threads
    int execTime = 0;
    while (execTime <= maxWait) {
        for(i = 0; i < N; i++) { 
            if (execTime == waitFor[i]) {
                int* id = malloc(sizeof(int));
                *id = i;
                pthread_create(&threads[i + 1], NULL, customer_th, (void*) id);
            }                
        }
        sleep(1);
        execTime += 1;
    }

    // Wait for barber to serve all customers
    i = N + 1;
    void* threadReturnValue;
    while (i-- > 1) { 
        pthread_join(threads[i], &threadReturnValue);
    }
    printf("===================================================\n");
    printf("SleepingBarberProblem: wszyscy klienci obsłóżeni.\n");
    printf("===================================================\n");

    // Clean up 
    pthread_mutex_destroy(&barber_mutex);
    pthread_mutex_destroy(&customer_mutex);

    pthread_cond_destroy(&barber_cond_var);
    pthread_cond_destroy(&customer_cond_var);

    free(waitFor);
    free(threads);

    return 0;
}