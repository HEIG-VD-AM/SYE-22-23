#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define CHILDREN_LIMIT 15
#define CHILDREN_LIMIT_DIGITS 2

int main(int argc, char **argv) {
    
    
    char currentChildId[CHILDREN_LIMIT_DIGITS + 1]; // "+ 1" pour \0
    pid_t childrenPid[CHILDREN_LIMIT];

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <threads> <mode>\n", argv[0]);
        return EXIT_FAILURE;
    }

    size_t number;  
    sscanf(argv[1], "%zu", &number);
    number = number > CHILDREN_LIMIT ? CHILDREN_LIMIT : number;

    for(size_t i = 0; i < number; ++i) {
    
        childrenPid[i] = fork();

        if(childrenPid[i] == 0) {
            
            sprintf(currentChildId, "%zu", i); 
            execl("./count.elf", "./count.elf", currentChildId, NULL);
        }
        else if(childrenPid[i] > 0 && strcmp(argv[2], "s") == 0) {
            waitpid(childrenPid[i], NULL, 0);  
        }
    }

    if(strcmp(argv[2], "p") == 0) {
        for(size_t i = 0; i < number; ++i) {
            waitpid(childrenPid[i], NULL, 0);  
        }
    }

    return EXIT_SUCCESS;
}

