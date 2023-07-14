#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

    // Vérifie que le nombre d'arguments passés en paramètre est correct (fichier cible et nom du lien)
    if (argc != 3) {
        printf("Usage: %s <target> <symbolic_link>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Ouvre le fichier cible en lecture seule et récupère son file descriptor
    int target_fd = open(argv[1], O_RDONLY);
    
    // En cas d'erreur lors de l'ouverture, affiche un message d'erreur
    if (target_fd == -1) {
        printf("Target file not found or unable to open !\n");
        return EXIT_FAILURE;
    }

    /* 
     * Crée un lien symbolique entre le fichier cible et le lien symbolique.
     * En cas d'erreur lors de la création, affiche un message d'erreur.
     */ 
    if (sys_symlink(target_fd, argv[2]) == -1) {
        printf("Unable to establish link with target file !\n");

        // Si la fermeture du file descriptor échoue, affiche un message d'erreur
        if (close(target_fd) == -1) {
            printf("Unable to close file descriptor of target file !\n");
        }
        return EXIT_FAILURE;
    }

    /* 
     * Ferme le file descriptor du fichier cible.
     * En cas d'erreur lors de la fermeture, affiche un message d'erreur
     */
    if (close(target_fd) == -1) {
        printf("Unable to close file descriptor of target file !\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

