#include <ctype.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <memreplace_gui/memreplace_gui.h>

#define RESET        	2
#define DELTA        	2
#define EXIT_MEMRPLC 	(-2)

#define PAGE_NB_MASK    0x00F0
#define REF_BIT_MASK    0x0004
#define SWAP_BIT_MASK   0x0002
#define VALID_BIT_MASK  0x0001

#define NB_PAGES    	16

/* [7..4] = Page number on 4 bits */
/* [3] = Unused 1 bit */
/* [2] = Reference on 1 bit */
/* [1] = SWAP on 1 bit */
/* [0] = Valid on 1 bit */
uint8_t page_table[NB_PAGES] = {0};

uint8_t TDU [NB_PAGES] = {0};
uint8_t compteur_global = 0;
uint8_t TVC = 0;

void replaceLRU(void) {

    // Index de la page à mettre à jour
    int lowestPage = -1;

    for(int i = 0; i < NB_PAGES; ++i) {

        // On vérifie que la page soit valide et on détermine si l'index doit être mis à jour pour celui de la page usée le moins récemment
        if((page_table[i] & VALID_BIT_MASK) && (lowestPage == -1 || TDU[lowestPage] > TDU[i]))
        {
            lowestPage = i;
        }
    }

    // On  met à jour les bits de la page utilisée le moins récemment
    page_table[lowestPage] &= ~VALID_BIT_MASK;
    page_table[lowestPage] |= SWAP_BIT_MASK;
}

void inc_compteur(uint8_t no_page) {

    // Mise à jour du TDU pour la page donnée
    if(no_page >= 0 && no_page < NB_PAGES) {
        TDU[no_page] = ++compteur_global;
    } else {
        printf("Page invalide !");
        return;
    }
}

void replaceWSC(void) {

    // Index de la page à mettre à jour
    int lowestPage = -1;
    
    for(int i = 0; i < NB_PAGES; ++i) {

        // On ne prend pas en compte les pages non valides
		if (!(page_table[i] & VALID_BIT_MASK)) {
			continue;
        }

        // Permet de définir la première page valide, cela sera utile dans le cas où auucne page n'a été évincée
        if(lowestPage == -1) {
            lowestPage = i;
        }

        // Détermine si la page a déjà eu le droit à sa "seconde chance"
        if(page_table[i] & REF_BIT_MASK) {
            page_table[i] &= ~REF_BIT_MASK;
        } else {

            // On vérifie que la page ne soit pas dans l'ensemble de travail
            if(TVC - TDU[i] > DELTA) {
                lowestPage = i;
                break;
            }
        }
    }

    // Si aucune page valide n'a été trouvée, c'est une erreur
    if (lowestPage == -1) {

        printf("Aucune page valide trouvee");
        return;
    } else {

        page_table[lowestPage] &= ~VALID_BIT_MASK;
        page_table[lowestPage] |= SWAP_BIT_MASK;
    }
    
}

void updateTDU(void) {

    for(int i = 0; i < NB_PAGES; ++i) {
        // Mise à jour du TDU des pages
        if(page_table[i] & REF_BIT_MASK & VALID_BIT_MASK) {
            TDU[i] = TVC;
        }
    }
}

void init_page_table(void) {
    int i;

    for (i = 0; i < NB_PAGES; i++)
        page_table[i] |= (i & 0xF) << 4;

    page_table[0] |= VALID_BIT_MASK;
	page_table[1] |= VALID_BIT_MASK;
	page_table[2] |= VALID_BIT_MASK;
	page_table[3] |= SWAP_BIT_MASK;
}

void print_memory(void) {
    int i;

    printf("RAM : ");

    for (i = 0; i < NB_PAGES; i++)
        if (page_table[i] & VALID_BIT_MASK)
            printf("[%d] ", i);

    printf("\nSWAP : ");
    for (i = 0; i < NB_PAGES; i++)
        if (!(page_table[i] & VALID_BIT_MASK) && (page_table[i] & SWAP_BIT_MASK))
            printf("[%d] ", i);

    printf("\n");
}

int ask_user_page_no() {

    char input[4];

    printf("Enter the page to be access: ");
    fflush(stdout);
    fgets(input, sizeof(input), stdin);

    if (input[0] == 'E' || input[0] == 'e')
        return EXIT_MEMRPLC;

    if (!isdigit(input[0]))
        return -1;

    return atoi(input);
}

void sigint_handler(int sig) {
    memreplace_exit();
    exit(0);
}

int main(int argc, char *argv[]) {
    int page_no;
    bool is_valid = true;
    bool gui_run = true; 
    int LRU_run  = 0;

    if (argc < 2 || argc > 3) {
        is_valid = 0;
    }
    else if (strcmp("LRU", argv[1]) == 0) {
        LRU_run = 1;
    }
    else if (strcmp("WSC", argv[1]) == 0) {
        LRU_run = 0;
    }
    else {
        is_valid = 0;
    }

    if (argc == 3) {
        if (strcmp("--ng", argv[2]) == 0) {
            gui_run = false;
        }
    }

    if (!is_valid) {
        printf("Usage: memereplace LRU|WSC [--ng]\n");
        exit(1);
    }

    signal(SIGINT, sigint_handler);

    if (gui_run)
        memreplace_init();
		
    init_page_table();
    print_memory();

    if (gui_run)
        memreplace_gui_update_pages(page_table, NB_PAGES);

    for (;;) {
        page_no = ask_user_page_no();

        if (page_no == EXIT_MEMRPLC) {
            memreplace_exit();
            return 0;
        }
        else {
            if (page_no >= 0 && page_no < NB_PAGES) {
                if (LRU_run) {
                    /* LRU Related */

                    if (!(page_table[page_no] & VALID_BIT_MASK)) { /* VALID = 0 ? */
                        replaceLRU();
                        page_table[page_no] &= ~SWAP_BIT_MASK;
                        page_table[page_no] |= VALID_BIT_MASK;
                    }
                    inc_compteur(page_no);
                }
                else {
                    /* WSC Related */
                    if (!(page_table[page_no] & VALID_BIT_MASK)) {/* VALID = 0 ? */
                        replaceWSC();
                        page_table[page_no] &= ~SWAP_BIT_MASK;
                    }
					page_table[page_no] |= REF_BIT_MASK;
					page_table[page_no] |= VALID_BIT_MASK;
                    ++TVC;
                    updateTDU();
                }

                print_memory();

                if (gui_run)
                    memreplace_gui_update_pages(page_table, NB_PAGES);
            }
            else
                printf("Invalid page number\n");
        }
    }

    return 0;
}
