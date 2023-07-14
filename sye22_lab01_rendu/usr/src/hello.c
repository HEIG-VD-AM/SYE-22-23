

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {

    char* hello = "Hello World !\n";

	printf("SO3 - Simple application\n");
    printf("%s", hello);
    printf("Adresse : %p\n", hello);

    size_t taille = strlen(hello);

    
    for(size_t i = 0; i < taille - 1; i++) {

        ++*((char*)(0x8000 + i * sizeof(char)));
    }
    
    printf("%s", hello);
    
    return 0;
}
