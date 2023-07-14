#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <syscall.h>

#define PAGE_SIZE 0x1000

int main(int argc, char **argv) {
    
    uint32_t* addr_virt = (uint32_t) malloc(PAGE_SIZE * sizeof(uint32_t));

    if(!addr_virt) {
        printf("Not enough space for allocation");
        return EXIT_FAILURE;
    } 

    uint32_t* addr_phys = sys_translate(addr_virt);


    printf("Adresse virtuelle : %p \n", addr_virt);
    printf("Adresse physique : %p \n", addr_phys);

    free(addr_virt);

    return EXIT_SUCCESS;
}
