
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

#define N 5
#define MEALS 5

enum { THINKING, HUNGRY, EATING };

const char *state_name[] = {
    "THINKING", "HUNGRY", "EATING"
};

pthread_mutex_t forks[N];
pthread_mutex_t print_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t state_lock = PTHREAD_MUTEX_INITIALIZER;
sem_t room;

/* Fair Queue synchronization */
pthread_mutex_t fair_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t fair_cond = PTHREAD_COND_INITIALIZER;

int fork_in_use[N] = {0};
unsigned long next_ticket = 0;
unsigned long serving_ticket = 0;

int state[N];
int meals_eaten[N] = {0};

int strategy = 1;

/*
 Strategy 1: Resource Ordering
 Strategy 2: Waiter / N-1 Semaphore
 Strategy 3: Fair FIFO Queue
*/

void display_status(int id, const char *message)
{
    int current_state;

    pthread_mutex_lock(&state_lock);
    current_state = state[id];
    pthread_mutex_unlock(&state_lock);

    pthread_mutex_lock(&print_lock);

    printf("Philosopher %d: %-9s | %s\n",
           id + 1, state_name[current_state], message);
    fflush(stdout);

    pthread_mutex_unlock(&print_lock);
}

void think(int id)
{
    pthread_mutex_lock(&state_lock);
    state[id] = THINKING;
    pthread_mutex_unlock(&state_lock);

    display_status(id, "Thinking...");
    usleep(300000 + rand() % 500000);
}

void take_forks(int id)
{
    int left = id;
    int right = (id + 1) % N;

    pthread_mutex_lock(&state_lock);
    state[id] = HUNGRY;
    pthread_mutex_unlock(&state_lock);

    display_status(id, "Waiting for forks");

    if (strategy == 1) {
        /* Always acquire the lower-numbered fork first. */
        int first = left < right ? left : right;
        int second = left < right ? right : left;

        pthread_mutex_lock(&forks[first]);
        pthread_mutex_lock(&forks[second]);
    }
    else if (strategy == 2) {
        /* Allow at most N-1 philosophers to compete. */
        sem_wait(&room);

        pthread_mutex_lock(&forks[left]);
        pthread_mutex_lock(&forks[right]);
    }
    else {
        /*
         FIFO ticket queue.
         Only the philosopher whose ticket is being served
         can acquire forks. Adjacent philosophers cannot eat
         together because they share a fork.
        */
        pthread_mutex_lock(&fair_lock);

        unsigned long my_ticket = next_ticket++;

        while (my_ticket != serving_ticket ||
               fork_in_use[left] ||
               fork_in_use[right]) {
            pthread_cond_wait(&fair_cond, &fair_lock);
        }

        fork_in_use[left] = 1;
        fork_in_use[right] = 1;

        serving_ticket++;
        pthread_cond_broadcast(&fair_cond);

        pthread_mutex_unlock(&fair_lock);
    }

    pthread_mutex_lock(&state_lock);
    state[id] = EATING;
    pthread_mutex_unlock(&state_lock);
}

void put_forks(int id)
{
    int left = id;
    int right = (id + 1) % N;

    if (strategy == 1 || strategy == 2) {
        pthread_mutex_unlock(&forks[left]);
        pthread_mutex_unlock(&forks[right]);

        if (strategy == 2) {
            sem_post(&room);
        }
    }
    else {
        pthread_mutex_lock(&fair_lock);

        fork_in_use[left] = 0;
        fork_in_use[right] = 0;

        pthread_cond_broadcast(&fair_cond);
        pthread_mutex_unlock(&fair_lock);
    }

    meals_eaten[id]++;

    pthread_mutex_lock(&state_lock);
    state[id] = THINKING;
    pthread_mutex_unlock(&state_lock);

    display_status(id, "Finished eating; forks released");
}

void *philosopher(void *arg)
{
    int id = *(int *)arg;

    for (int meal = 0; meal < MEALS; meal++) {
        think(id);
        take_forks(id);

        display_status(id, "Eating...");
        usleep(300000 + rand() % 500000);

        put_forks(id);
    }

    pthread_mutex_lock(&print_lock);
    printf("Philosopher %d completed all meals.\n", id + 1);
    pthread_mutex_unlock(&print_lock);

    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc > 1) {
        strategy = atoi(argv[1]);
    }

    if (strategy < 1 || strategy > 3) {
        printf("Usage: %s [1|2|3]\n", argv[0]);
        printf("1 = Resource Ordering\n");
        printf("2 = Waiter / N-1 Semaphore\n");
        printf("3 = Fair FIFO Queue\n");
        return 1;
    }

    srand((unsigned int)time(NULL));

    const char *strategy_name =
        strategy == 1 ? "Resource Ordering" :
        strategy == 2 ? "Waiter / N-1 Semaphore" :
                        "Fair FIFO Queue";

    printf("\n========================================\n");
    printf(" DINING PHILOSOPHERS OS SIMULATOR\n");
    printf("========================================\n");
    printf("Strategy: %s\n", strategy_name);
    printf("Philosophers: %d | Meals each: %d\n\n", N, MEALS);

    for (int i = 0; i < N; i++) {
        pthread_mutex_init(&forks[i], NULL);
        state[i] = THINKING;
    }

    sem_init(&room, 0, N - 1);

    pthread_t threads[N];
    int ids[N];

    for (int i = 0; i < N; i++) {
        ids[i] = i;

        if (pthread_create(&threads[i], NULL,
                           philosopher, &ids[i]) != 0) {
            perror("pthread_create");
            return 1;
        }
    }

    for (int i = 0; i < N; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\n========================================\n");
    printf(" SIMULATION RESULTS\n");
    printf("========================================\n");

    int total = 0;

    for (int i = 0; i < N; i++) {
        printf("Philosopher %d: %d meals\n",
               i + 1, meals_eaten[i]);
        total += meals_eaten[i];
    }

    printf("Total meals: %d\n", total);
    printf("Expected meals: %d\n", N * MEALS);

    if (total == N * MEALS) {
        printf("RESULT: PASS - All meals completed.\n");
    } else {
        printf("RESULT: FAIL - Meal count mismatch.\n");
    }

    printf("Simulation completed.\n");

    sem_destroy(&room);

    for (int i = 0; i < N; i++) {
        pthread_mutex_destroy(&forks[i]);
    }

    pthread_mutex_destroy(&print_lock);
    pthread_mutex_destroy(&state_lock);
    pthread_mutex_destroy(&fair_lock);
    pthread_cond_destroy(&fair_cond);

    return 0;
}

