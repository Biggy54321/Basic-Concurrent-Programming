/*!
 * \file diner_philosopher_prob.c
 * \brief This program implements the diner philosopher's problem where there
 *        are 'n' philosophers and each philosopher thinks and eats for 'm' such
 *        days. Each philosopher requires two forks to eat. Hence the following
 *        solution provides a solution to the above problem without resulting a
 *        deadlock condition.
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_OF_PHILOSOPHERS 5
#define NUM_OF_DAYS         3
#define _THREAD_DEBUG_      0

/* Semaphore representing whether a philosopher can grab a fork to eat */
sem_t can_eat;
/* Semaphore representing whether a fork is already grabbed or not */
sem_t grab_fork[NUM_OF_PHILOSOPHERS];

/*!
 * \brief This function adds a little delay to represent the time for which
 *        a philosopher thinks
 * \param id The philosopher id or number
 * \param day The day number for which the philosopher is thinking
 */
void think(int id, int day) {
#if _THREAD_DEBUG_ == 0
    printf("Philosopher %d is thinking on day %d\n", id + 1, day + 1);
#endif
    sleep(0.0001);
}
/*!
 * \brief This function adds a little delay to represent the time for which
 *        a philosopher eats
 * \param id The philosopher id or number
 * \param day The day number for which the philosopher is eating
 */
void eat(int id, int day) {
#if _THREAD_DEBUG_ == 0
    printf("Philosopher %d is eating   on day %d\n", id + 1, day + 1);
#endif
    sleep(0.0003);
}

/*!
 * \brief This thread simulates the life of a philosopher, satisfying the
 *        conditions required while eating in case of multiple philosophers
 *        trying to eat.
 * \param _args (Type casted to a long integer) The philosopher id or number
 */
void *philosophers_life(void *_args) {
    int day;
    long philo_id;

    /* Get the philosopher's id */
    philo_id = (long)_args;

    /* Each philosopher thinks and eats for three days in total */
    for (day = 0; day < NUM_OF_DAYS; day++) {
        /* think for a while */
        think(philo_id, day);

        /* Wait till you can grab the forks to eat */
        sem_wait(&can_eat);
#if _THREAD_DEBUG_
        printf("%ld entered\n", philo_id);
#endif

        /* Wait till you can get access to two adjacent forks */
        sem_wait(&grab_fork[philo_id]);
#if _THREAD_DEBUG_
        printf("%ld grabbed 1st fork\n", philo_id);
#endif

        sem_wait(&grab_fork[(philo_id + 1) % NUM_OF_PHILOSOPHERS]);
#if _THREAD_DEBUG_
        printf("%ld grabbed 2nd fork\n", philo_id);
#endif

        /* eat for a while */
        eat(philo_id, day);

        /* Signal the release of the forks used for eating */
#if _THREAD_DEBUG_
        printf("%ld released 1st fork\n", philo_id);
#endif
        sem_post(&grab_fork[philo_id]);

#if _THREAD_DEBUG_
        printf("%ld released 2nd fork\n", philo_id);
#endif
        sem_post(&grab_fork[(philo_id + 1) % NUM_OF_PHILOSOPHERS]);


        /* Signal the release of access to grab the forks */
#if _THREAD_DEBUG_
        printf("%ld leaved\n", philo_id);
#endif
        sem_post(&can_eat);
    }
}

int main() {

    /* Iterator */
    long i;

    /* Declare a thread structure array for each philosopher */
    pthread_t ph[NUM_OF_PHILOSOPHERS];

    /* Initialize the semaphores */
    sem_init(&can_eat, 0, NUM_OF_PHILOSOPHERS - 1);
    for (i = 0; i < NUM_OF_PHILOSOPHERS; i++) {
        sem_init(&grab_fork[i], 0, 1);
    }

    /* Create a thread for each philosopher */
    for (i = 0; i < NUM_OF_PHILOSOPHERS; i++) {
        pthread_create(&ph[i], NULL, philosophers_life, (void *)i);
    }

    /* Wait for each philosopher thread to complete */
    for (i = 0; i < NUM_OF_PHILOSOPHERS; i++) {
        pthread_join(ph[i], NULL);
    }

    /* Free the semaphores */
    sem_destroy(&can_eat);
    for (i = 0; i < NUM_OF_PHILOSOPHERS; i++) {
        sem_destroy(&grab_fork[i]);
    }
    return 0;
}
