/*!
 * \file reader_writer_problem.c
 * \brief This program implements the reader writer problem in concurrent
 *       programming. The program using semaphores ensures that the writer
 *       fills in the empty slots of the buffer and the reader can only read
 *       if the slots in the buffer are filled by the writer and thus making
 *       a slot empty.
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

/* Define the Buffer attributes */
#define BUFFER_SIZE           8
#define NUM_OF_DATA_ELEMENTS  16
#define _THREAD_DEBUG_        0

/* The global buffer */
int g_buffer[BUFFER_SIZE];

/* Semaphore indicating the empty slots or the slots which have been read */
sem_t g_empty_slots;
/* Semaphore indication the slots which are written with data but not yet read */
sem_t g_filled_slots;

/*!
 * \brief This thread reads the predefined number of data elements from the
 *        global buffer and prints the square of each read number. It reads
 *        only if there are filled slots and also ensures the slots are made
 *        empty after reading.
 */
void *thread_read(void *_args) {
    int buffer_i, read_num;

    /* Read the data elements from the buffer */
    for (buffer_i = 0; buffer_i < NUM_OF_DATA_ELEMENTS; buffer_i++) {
        /* Wait till atleast the current slot is filled by the writer */
        sem_wait(&g_filled_slots);

#if _THREAD_DEBUG_
        printf("%d data element read\n", buffer_i + 1);
#endif

        /* Read the number from the buffer */
        read_num = g_buffer[buffer_i % BUFFER_SIZE];

        /* Signal the current slot being emptied as it is read */
        sem_post(&g_empty_slots);

        /* Process the data (Printing the square of the read number) */
#if _THREAD_DEBUG_ == 0
        printf("%d\n", read_num * read_num);
#endif
    }
}

/*!
 * \brief This thread writes the predefined number of data elements in the global
 *        global array only if it finds atleast one empty slot in the buffer and
 *        also marks the slot as filled after writing
 */
void *thread_write(void *_args) {
    int buffer_i;

    /* Write the data elements to the buffer */
    for (buffer_i = 0; buffer_i < NUM_OF_DATA_ELEMENTS; buffer_i++) {
        /* Wait till there is atleast one empty slot to write into */
        sem_wait(&g_empty_slots);

#if _THREAD_DEBUG_
        printf("%d data element written\n", buffer_i + 1);
#endif

        /* Fill the slot with a number (which is current index + 1) */
        g_buffer[buffer_i % BUFFER_SIZE] = buffer_i + 1;

        /* Signal the slot being filled with some data */
        sem_post(&g_filled_slots);
    }
}

int main() {

    /* Initialize the semaphores */
    sem_init(&g_empty_slots, 0, BUFFER_SIZE);
    sem_init(&g_filled_slots, 0, 0);

    /* Create the reader and the writer threads */
    pthread_t read_th, write_th;
    pthread_create(&read_th, NULL, thread_read, NULL);
    pthread_create(&write_th, NULL, thread_write, NULL);

    /* Wait for the two threads to complete */
    pthread_join(read_th, NULL);
    pthread_join(write_th, NULL);

    /* Free the semaphores */
    sem_destroy(&g_empty_slots);
    sem_destroy(&g_filled_slots);

    return 0;
}
