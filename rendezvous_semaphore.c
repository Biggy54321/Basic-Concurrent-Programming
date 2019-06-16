/*!
 * \file rendezvous.c
 * \brief This program implements the rendezvous semaphore example, wherein a
 *        client thread requests the server thread to add two numbers
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define _THREAD_DEBUG_        1

/* Semaphore for requesting the addition */
sem_t g_sem_request;
/* Semaphore for signalling the completion of addition */
sem_t g_sem_served;

/* First number, Second number and the G_Sum */
int g_num1, g_num2, g_sum;

/*!
 * \brief This client thread initializes the two numbers and request the server
 *        thread to add the two global numbers and prints the result
 */
void *thread_client(void *_args) {
    /* Initialize the globals */
#if _THREAD_DEBUG_
    printf("Client - Initializing the global operands\n");
#endif
    g_num1 = 5;
    g_num2 = 10;

    /* Signal the server thread to add the two numbers */
#if _THREAD_DEBUG_
    printf("Client - Signalling the server to add the two globals\n");
#endif
    sem_post(&g_sem_request);


    /* Wait for the server thread to add the two numbers */
    sem_wait(&g_sem_served);
#if _THREAD_DEBUG_
    printf("Client - Server response received\n");
#endif

    /* Print the result */
#if _THREAD_DEBUG_ == 0
    printf("SUM = %d\n", g_sum);
#endif
}

/*!
 * \brief This thread adds the two globals upon being requested by the client
 */
void *thread_server(void *_args) {

    /* Wait till the client thread request the globals to be added */
    sem_wait(&g_sem_request);
#if _THREAD_DEBUG_
    printf("Server - Received the request to add two numbers\n");
#endif

    /* Add the two globals */
    g_sum = g_num1 + g_num2;

    /* Signal the client thread that the addition is over */
#if _THREAD_DEBUG_
    printf("Server - Signalling the client about the termination of the request\n");
#endif
    sem_post(&g_sem_served);
}

int main() {

    /* Initialize the semaphores */
    sem_init(&g_sem_request, 0, 0);
    sem_init(&g_sem_served, 0, 0);

    /* Create the thread ids */
    pthread_t th1, th2;
    pthread_create(&th2, NULL, thread_server, NULL);
    pthread_create(&th1, NULL, thread_client, NULL);

    /* Wait for the threads to finish */
    pthread_join(th1, NULL);
    pthread_join(th2, NULL);

    /* Free the semaphores as they may be dynamically allocated */
    sem_destroy(&g_sem_request);
    sem_destroy(&g_sem_served);

    return 0;
}
