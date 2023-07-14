# Rajout d'un appel systène dabs SO3

## Espace noyau

### so3/include/syscall.h 
Dans cette section, il faut ajouter une définition pour l'appel système. Cela revient à ajouter le nom de l'appel système, ainsi qu'un numéro qui le représente.

    #define SYSCALL_FORK2 		88

### so3/include/process.h 
On ajoute la déclaration de la méthode "do_fork2". Celle-ci sera la méthode utilisée lorsque l'on voudra fork un process.

    int do_fork2(void);

### so3/kernel/process.c 
Cette partie va de paire avec la précédente. C'est tout simplement la définition de la méthode "do_fork2". Nous avons repris la méthode "do_fork" qui était déjà implémentée et l'avons modifiée pour avoir un affichage personnalisé. Cette définition permettra lors de l'appel de la fonction de fork un process et d'afficher le message qui nous était indiqué dans la consigne. Ci-dessous la modification apportée à la méthode do_fork() de base.

    printk("fork2(): process %s with pid %d forked to child with pid %d\n", parent->name, parent->pid, newp->pid);


### so3/kernel/syscalls.c 
Ici, il s'agit d'ajouter dans le switch notre appel système. Cet ajout permettra de faire le lien entre la définition de l'appel (son numéro) que l'on a défini dans syscall.h et la fonction qui est exécutée qui se trouve dans process.h et process.c.


    case SYSCALL_FORK2:
        result = do_fork2();
        break;

## Espace utilisateur

### usr/lib/libc/include/syscall.h
Ajout dans la partie utilisateur de l'appel système, ainsi que du numéro qui l'identifie. C'est la même tâche que l'on a fait dans le premier syscall.h.

    #define syscallFork2 		    88

### usr/lib/libc/crt0.S 
Défintion du stub pour notre appel système. On définit quel est le mot à utiliser dans le code afin d'accéder à la fonctionnalité que l'on vient d'implémenter.

    SYSCALLSTUB sys_fork2, 			syscallFork2

## Synthèse
Afin d'ajouter un appel système, il y a deux parties principales. On doit d'abord définir ce que notre appel système fait et réaliser son implémentation dans la partie noyau. Dans un second temps, il faut réaliser le lien avec la partie utilisateur. C'est-à-dire la partie du code qui va générer une interruption logiciel et qui est appelée le stub. Une fois ces deux parties réalisés, on peut utiliser notre nouvel appel. Lors de l'appel, on ira interagir avec la partie se situant dans l'espace utilisateur et ensuite le système redirigera vers l'espace noyau.