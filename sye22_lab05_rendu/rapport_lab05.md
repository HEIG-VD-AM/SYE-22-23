# Rapport laboratoire n°5 - SYE - Alexis Martins

## Étape 2 - Ajout de renice()

### B)
Au lancement de "time_loop", il ne se passe rien jusqu'au lancement d'une commande. Au moment où l'on lance la commande, le compteur de time_loop démarre jusqu'à interruption. Cela s'explique par le fait que les deux processus aient la même priorité. Il n'y a pas de préemption.

Le shell reste alors dans l'état running pendant que time_loop est en waiting. Au lancement de la commande, le shell passe en waiting et time_loop en running. Le lancement de la commande "met en pause" le shell. Si time_loop n'était pas lancé en arrière-plan, alors il s'exécuterait directement. 

### C)
Pour le changement de la priorité du shell, on peut utiliser la fonction renice que l'on vient de coder. Il ne faut pas oublier que le shell possède un pid de 1 et une priorité de 10. On va donc définir une priorité inférieure à 10 pour le pid 1 avec la commande suivante :
    
    renice <pid> <prio> --> renice 1 2

Ensuite, on peut lancer une commande comme précédemment et on remarque que cette fois-ci l'exécution est instantanée. Cela s'explique à cause de la priorité de time_loop qui est de 10 et celle du shell qui est de 2 dans l'exemple ci-dessus. time_loop étant plus prioritaire que le Shell, cette fois-ci il s'exécute.

### D)
Si on change la priorité du shell et que l'on lance en mode "bloquant". Alors le compteur prendra le dessus sur le shell et on ne peut plus revenir sur le shell complètement. On peut écrire sur le shell, mais le compteur continu. On retrouve alors des passages entre le shell et le compteur lorsque l'on esaie d'écrire.

## Étape 3 - Exploration de la gestion mémoire dans le noyau de SO3

### A)
La gestion du tas se situe dans le fichier suivant :
    
    so3/mm/heap.c

### B)

L'algorithme utilisé est le Quick-Fit. On peut le voir à l'initialisation de la heap.

    /*
    * Initialization of the quick-fit list.
    */
    void heap_init(void)

### C)
La gestion de la MMU se fait dans les fichiers ci-dessous selon l'architecture de notre processeur.
    
    so3/arch/arm32/mmu.c
    ou/et
    so3/arch/arm64/mmu.c

### D)

    uint32_t do_translate(uint32_t vaddr) {
		return virt_to_phys_pt(vaddr);
    }

### F)

Exemple de sortie ci-dessous

    so3% memory
    Adresse virtuelle : 0x20c018 
    Adresse physique : 0x905cf018 
    so3% memory
    Adresse virtuelle : 0x20c018 
    Adresse physique : 0x949e8018 

On remarque que les adresses virtuelles sont toujours identiques, tandis que les adresses physiques changent. 

On peut expliquer cela, car les adresses virtuelles dépendent du programme que l'on exécute. Ce programme étant toujours le même, alors il a toujours le même emplacement mémoire allouée. Par contre si on clonait notre programme et qu'on le changeait légèrement, alors l'adresse serait différente.

Pour les adresses physiques, le problème vient du fait que la mémoire n'est pas spécifique au programme. Elle peut donc changer selon ce qui se passe sur la machine.

On remarque que les adresses ont toutes la même série de chiffre à la fin. Cela s'explique à cause d'une offset qui "décale" l'adresse où commence les pages. Les pages ont une taille de 0x1000, et c'est pour cette raison que l'on retrouve alors à chaque fois la même série "018" en fin d'adresse. L'incrémente de 0x1000 n'impacte pas les trois derniers chiffres.