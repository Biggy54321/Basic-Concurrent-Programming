#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_OF_CUSTOMERS 10
#define MIN_NUM_OF_CONES 1
#define MAX_NUM_OF_CONES 4

/* Structure for communication between the clerks and the manager */
struct VerifyRequest {
    /* Whether a cone has passed the verification or not */
    int passed;
    /* Semaphore used by clerk to signal manager for verification */
    sem_t request;
    /* Semaphore used by manager to signal clerk about termination of process */
    sem_t done;
    /* Semaphore used by clerk to lock the access of manager to other clerks */
    sem_t lock;
} verify;

/* Structure for communication between the customer and the cashier */
struct CashRequest {
    /* Token number */
    int token;
    /* Semaphore used by customer to signal the cashier to get a bill */
    sem_t bill[NUM_OF_CUSTOMERS];
    /* Semaphore to prevent the access to use token for another customers */
    sem_t lock;
} cash;

/**
 * \brief This function simulates the action of verifying the cone by the
 *        manager.
 * \param void
 * \retval 0 or 1 depending on whether the cone has passed the test or not.
 */
static int verify_cone() {
    sleep(0.0001);
    return (rand() % 2);
}

/**
 * \brief This thread simulates the action of manager, i.e. waiting for the
 *        clerk to signal a request and then finding the result and signalling
 *        the clerk about the termination of verification.
 * \param _args The total number of cones that the manager has to verify before
 *              terminating.
 * \retval void
 */
void *manager_th(void *_args) {
    long total_num_of_cones = (long)_args;
    long cones_approved = 0;
    int result;

    /* Keep on verifying the cones till all are approved */
    while (cones_approved < total_num_of_cones) {
        /* Wait till a clerk signals for verification */
        sem_wait(&verify.request);

        /* Verify the cone */
        result = verify_cone();
        verify.passed = result;

        /* Increment the approved cone count */
        if (result) {
            cones_approved++;
        }

        /* Signal the clerk about the termination of verification */
        sem_post(&verify.done);
    }
}

/**
 * \brief This function simulates the action of making a cone by a clerk.
 * \param void
 * \retval void
 */
static void make_cone() {
    sleep(0.0001);
}

/**
 * \brief This thread simulates the actions of a clerk, i.e. making a cone,
 *        checking if manager is available, if yes then verifying the cone and
 *        keep on making the cone till the cone is approved by the manager.
 * \param void
 * \retval void
 */
void *clerk_th(void *_args) {

    int result = 0;

    /* Keep on making a cone till the cone is verified successfully */
    while (!result) {
        /* Make a single cone for the customer */
        make_cone();

        /* Wait till the clerk can access the manager for verification */
        sem_wait(&verify.lock);

        /* Signal the manager for verification */
        sem_post(&verify.request);

        /* Wait till the cone is being verified */
        sem_wait(&verify.done);

        /* Check the result of verification */
        result = verify.passed;

        /* Signal that the manager is free for other clerks */
        sem_post(&verify.lock);
    }
}

/**
 * \brief This function simulates the action of the customer of searching for
 *        the ice cream from all the provided varieties.
 * \param void
 * \retval void
 */
void browse() {
    sleep(0.0002);
}

/**
 * \brief This thread simulates the actions performed by a customer i.e.,
 *        browsing for the required ice creams and spawning the clerks to get
 *        their ice creams. As well as it takes a token for getting a bill from
 *        the cashier.
 * \param _args Number of cones required by the customer.
 * \retval void
 */
void *customer_th(void *_args) {
    long num_of_cones = (long)_args;
    long i;
    int token;

    pthread_t clerk[MAX_NUM_OF_CONES];

    /* Check for the different ice creams */
    browse();

    /* Create a thread representing each clerk being requested to make a cone */
    for (i = 0; i < num_of_cones; i++) {
        pthread_create(&clerk[i], NULL, clerk_th, NULL);
    }

    /* Wait for the clerks to make the cones */
    for (i = 0; i < num_of_cones; i++) {
        pthread_join(clerk[i], NULL);
    }

    /* Wait till the customer gets the chance to take a token */
    sem_wait(&cash.lock);

    /* Grab a token */
    token = cash.token;
    cash.token++;

    /* Signal that the token is free to be grabbed by other customers */
    sem_post(&cash.lock);

    /* Signal that the current customer thread is ready with a token for a bill */
    sem_post(&cash.bill[token]);

}

/**
 * \brief This function simulates the action of giving a bill to a customer by
 *        the cashier
 * \param void
 * \retval void
 */
static void give_bill() {
    sleep(0.0001);
}

/**
 * \brief This thread simulates the actions of the cashier which is to process
 *        the request of the customers to get their bill only in first in first
 *        out manner.
 * \param void
 * \retval void
 */
void *cashier_th(void *_args) {
    int i = 0;

    /* Serve the customer in FIFO manner */
    for (i = 0; i < NUM_OF_CUSTOMERS; i++) {
        /* Wait till the customer with the lowest token number asks for a bill */
        sem_wait(&cash.bill[i]);

        /* Give the requesting customer a bill */
        give_bill();
    }
}

/**
 * \brief This function initializes all the semaphores to the required values.
 * \param void
 * \retval void
 */
void initialize_semaphores() {
    int i;

    /* Initialize the request semaphore to zero */
    sem_init(&verify.request, 0, 0);
    /* Initialize the verification done semaphore to zero */
    sem_init(&verify.done, 0, 0);
    /* Initialize the semaphore to access the manager to one*/
    sem_init(&verify.lock, 0, 1);
    /* Initialize the cashier access semaphores to zero */
    for (i = 0; i < NUM_OF_CUSTOMERS; i++) {
        sem_init(&cash.bill[i], 0, 0);
    }
    /* Initialize the semaphore to access the token to one */
    sem_init(&cash.lock, 0, 1);
    cash.token = 0;
}

/**
 * \brief This function frees all the semaphores to the required values.
 * \param void
 * \retval void
 */
void destroy_semaphores() {
    int i;

    sem_destroy(&verify.request);
    sem_destroy(&verify.done);
    sem_destroy(&verify.lock);
    sem_destroy(&cash.lock);
    for (i = 0; i < NUM_OF_CUSTOMERS; i++) {
        sem_destroy(&cash.bill[i]);
    }
}

int main() {

    /* Initialize the semaphores */
    initialize_semaphores();

    long total_num_of_cones = 0, i, num_of_cones;

    pthread_t customer[NUM_OF_CUSTOMERS], manager, cashier;

    /* Create the customer threads and total number of cones for everyone */
    for (i = 0; i < NUM_OF_CUSTOMERS; i++) {
        /* Get random number of cones between 1 to 4 */
        num_of_cones = ((rand() % 4) + 1);
        /* Update the total number of cones */
        total_num_of_cones += num_of_cones;
        /* Create a customer thread with argument as the number of cones*/
        pthread_create(&customer[i], NULL, customer_th, (void *)num_of_cones);
    }

    /* Create the manager thread, with argument as total number of cones */
    pthread_create(&manager, NULL, manager_th, (void *)total_num_of_cones);

    /* Create the cashier thread with no arguments */
    pthread_create(&cashier, NULL, cashier_th, NULL);

    /* Wait for the manager and the cashier to get their job done */
    pthread_join(manager, NULL);
    pthread_join(cashier, NULL);
    for (i = 0; i < NUM_OF_CUSTOMERS; i++) {
        pthread_join(customer[i], NULL);
    }

    /* Free the semaphores */
    destroy_semaphores();

    return 0;
}
