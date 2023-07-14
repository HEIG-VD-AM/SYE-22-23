#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syscall.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>


#define LETTERS_NB 26
#define A_UPPER_OFFSET 65
#define A_LOWER_OFFSET 97
#define CHILDREN_LIMIT 15

typedef struct  {
    /* Pointer to position in the text */
	char *text_pointer;
    /* Size of the text chuck to process */
	size_t size;
    /* Letters counters, used to send the results back */
	size_t counters[LETTERS_NB];
} count_param_t;


/**
 * @brief Threads function used to count the letters occurence in a text
 */
void *count_letters(void *args) {	

	// Local copy of the params
	count_param_t* local_params = (count_param_t*) args;

	size_t index;

	for(size_t i = 0; i < local_params->size; ++i) {

		// Calculate the case of the counter array to increment.
		// Conversion to lower case then substraction of the 'a' offset.
		index = tolower(*((local_params->text_pointer) + i)) - A_LOWER_OFFSET;

		if(index < LETTERS_NB)
			(local_params->counters[index])++;
	}

	return NULL;
}

/**
 * @brief Calculate the size of a file
 * 
 * @return The size of the file <fp>
 */
size_t fseek_filesize(FILE *fp) {
    long off;


    if (fseek(fp, 0, SEEK_END) == -1)
    {
        printf("failed to fseek\n");
        exit(EXIT_FAILURE);
    }

    off = ftell(fp);
    if (off == -1)
    {
        printf("failed to ftell\n");
        exit(EXIT_FAILURE);
    }

	fseek(fp, 0, SEEK_SET);

	return off;
}

/**
 * @brief Load a file content into a char buffer
 * 
 * @return A char *buffer with the content of the file <filename>
 */
char *buffer_from_file(char *filename) {

	char *buffer;
	size_t text_size = 0;
	FILE *f = fopen(filename, "r");

	if (f == NULL) {
        printf("failed to fopen %s\n", filename);
        exit(EXIT_FAILURE);
	}

	text_size = fseek_filesize(f);

	buffer = malloc(text_size+1);

	if (buffer == NULL) {
        printf("failed to malloc() the buffer\n");
        exit(EXIT_FAILURE);
	} 

	/* Read the whole content into buffer */
	fread(buffer, 1, text_size, f);

	fclose(f);

	/* Add the \0 at the end */
	buffer[text_size] = 0;

	return buffer;
}


/**
 * @brief Main function
 */
int main(int argc, char **argv)
{
	int ret; 				// Return value from thread creation

	int thread_num;			// Will contain the numnber of threads we want to launch

	count_param_t *params;	// Will contain the parameters passed to the threads
	pthread_t *threads;		// Array of pthread used to join them

	size_t chunk_size = 0; 	// Will contain the chunk size each thread will process 
	size_t leftovers = 0;  	// Will contain the lefotvers not covered by the chunks
	size_t text_size;		// Will contain the text size
	char *text;				// Will contain the text itself as (char *) buffer

	size_t result_counters[LETTERS_NB] = { 0 }; // Used to compile the final count result

	char *filename;			// Will contain the filename of the file we want to count the characters


	/* Arguments checks */
	if(argc != 3) {
		printf("Usage : threads <noThreads> <filename>\n");
		exit(-1);
	}

	/* TO COMPLETE */

	// Reading thread number
	sscanf(argv[1], "%d", &thread_num);

	// Set max thread to 15
    thread_num = thread_num > CHILDREN_LIMIT ? CHILDREN_LIMIT : thread_num;

	// Reading filname
	filename = argv[2];
	/* End argument check */


	/* Load text from file */
	text = buffer_from_file(filename);
	text_size = strlen(text);

	/* Text split preparation */

	/* TO COMPLETE */
	chunk_size = text_size / thread_num;
	leftovers = text_size % thread_num;

	// Allocation of count_param struct for each thread
	params = (count_param_t*) malloc(thread_num * sizeof(count_param_t));

	// Error if allocation didn't achieve
	if (params == NULL) {
      printf("Unable to allocate memory :(\n");
      exit(EXIT_FAILURE);
    }

	/* End text split preparation */

	printf("Now counting on a text of size %d, with %d threads each counting %d char\n", text_size, thread_num, chunk_size);
	
	/* Threads creation and parameters setting */

	// Allocation of the number of threads
	threads = (pthread_t*) malloc(thread_num * sizeof(pthread_t));

	// Error if allocation didn't achieve
	if (threads == NULL) {
      printf("Unable to allocate memory :(\n");
      exit(EXIT_FAILURE);
    }

	for(size_t i = 0; i < thread_num; ++i) {
		
		// Setting chunk size and text start for each thread
		params[i] = (count_param_t) {

			.text_pointer = text + i * chunk_size,
		
			.size = chunk_size + ((i == thread_num - 1) ? leftovers : 0),

			.counters = {0}
		};

		// Creating threads and calling function with their count_param struct
		ret = pthread_create(&threads[i], NULL, count_letters, (void*) &params[i]);
	
		// Managing thread creation error
		if(ret) {
			printf("ERROR : return code from pthread_create() is %d\n", ret);
			exit(EXIT_FAILURE);
		}
	}

	/* End threads creation and parameters setting */


	/* Thread join and result gathering */

	/* TO COMPLETE */
	for(size_t i = 0; i < thread_num; ++i) {
		
		pthread_join(threads[i], NULL);
	}
	
	// Merging results
	for(size_t i = 0; i < LETTERS_NB; ++i) {

		for(size_t j = 0; j < thread_num; ++j) {

			result_counters[i] += params[j].counters[i];
		}
	}
	/* End thread join and result gathering */
	

	/* Display the results */
	for (int i = 0; i < LETTERS_NB; ++i) {
		printf("Number of %c: %u\n", (char)(i+A_LOWER_OFFSET), result_counters[i]);
	}
	
	/* Free the memory */
	free(text);

	/* MAYBE TO COMPLETE? */
	free(params);
	free(threads);

	/* End free the memory */

	return EXIT_SUCCESS;
}
