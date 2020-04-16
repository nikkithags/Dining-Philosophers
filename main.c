#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<windows.h>

#define TOTAL_MEALS_ALLOWED 4

//The philosopher starts in THINKING state, then it is HUNGRY,
//and then if the neighboring philosophers are not eating then
//it goes into EATING state
#define THINKING 0
#define EATING 1
#define HUNGRY 2
#define r 2 //r is the highest in the range for random number
#define l 0 //l is the lowest in the range for random number

pthread_mutex_t lock; //lock to keep CR safe while obtaining and returning chopsticks
int totalPhilosophers;
int *state; //THINKING,HUNGRY,EATING
int *count; //total number of times each philosopher eats
int meal = 0;
pthread_t *philosopher; //threads for each philosopher

//returning chopstick after eating ie. changing the state of the philosopher
//Also checking if the meals reached their limit and exiting the program.
void return_chopsticks(int philID)
{
    pthread_mutex_lock(&lock);
    state[philID] = THINKING;
    printf("Philosopher %d finish eating\n", philID+1);
    count[philID]++;
    meal++;
    if(meal == TOTAL_MEALS_ALLOWED)
    {
        printf("\n\n\n");
        for (int i=0;i<totalPhilosophers;i++)
        {
            printf("[Philosopher %d = %d meals] ",i+1,count[i]);
            printf("\n");
        }
        printf("\nMAXIMUM MEALS REACHED\nDINNER OVER\n");
        exit(0);
    }
    pthread_mutex_unlock(&lock);
}

//checking the neighbors and change state to EATING in this function
void check_chopsticks(int philID)
{
    printf("Philosopher %d looking for chopsticks\n", philID+1);
    if (state[philID] == HUNGRY && (state[(philID + 1) % totalPhilosophers] != EATING && state[(philID + totalPhilosophers - 1) % totalPhilosophers] != EATING))
    {
        state[philID] = EATING;
        int eating = (rand() % (r - l + 1));
        printf("Philosopher %d is eating for %d seconds\n", philID+1, eating);
        Sleep(eating*1000);
        printf("\n");
    }
}

//The philosopher is hungry and checks for chopstick availability in this function
void obtain_chopsticks(int philID)
{
    pthread_mutex_lock(&lock);
    state[philID] = HUNGRY;
    srand(time(0) + philID); //if philID is not added then same seed will be taken for every philosopher
    int hungry = (rand() % (r - l + 1));
    printf("Philosopher %d is hungry for %d seconds\n", philID+1, hungry);
    Sleep(hungry*1000);
    check_chopsticks(philID);
    pthread_mutex_unlock(&lock);
}

//The function initialised to the pthreads. Obtaining and returning of chopsticks are done here.
void *startDinner(int *phil_ID)
{
    while(1){
        int philID = *phil_ID;
        srand(time(0) + philID); //if philID is not added then same seed will be taken for every philosopher
        int think = (rand() % (r - l + 1)); //max allowed is 3
        printf("Philosopher %d is thinking for %d seconds\n", philID+1, think);
        Sleep(think*1000);
        obtain_chopsticks(philID);
        return_chopsticks(philID);
    }
}

int main()
{
    printf("Enter number of philosophers :");
    scanf("%d",&totalPhilosophers);
    state = malloc(sizeof(int*)*totalPhilosophers);
    count = malloc(sizeof(int*)*totalPhilosophers);
    philosopher = malloc(sizeof(pthread_t*)*totalPhilosophers);
    int id[totalPhilosophers];
    pthread_mutex_init(&lock, NULL);
    for (int i = 0; i < totalPhilosophers; i++)
    {
        id[i] = i;
        state[i] = THINKING;
        count[i] = 0;
        pthread_create(&philosopher[i], NULL, &startDinner, &id[i]);
    }
    for (int i = 0; i < totalPhilosophers; i++)
    {
        pthread_join(philosopher[i], NULL);
    }
}
