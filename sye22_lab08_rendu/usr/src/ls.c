/*
 * Copyright (C) 2014-2017 Daniel Rossier <daniel.rossier@heig-vd.ch>
 * Copyright (C) 2019-2020 Julián Burella Pérez <julian.burellaperez@heig-vd.ch>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <dirent.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <dirent.h>
#include <sys/stat.h>
#include <syscall.h>

#define BYTES_PER_KILO 1024

/* Convert a size in bytes to a human readable format
 * Only one number after the comma will be displayed
 */
void human_readable_size(const unsigned long int size, char *buf)
{
	char *prefix[] = {"", "K", "M", "G", "T"};
	size_t prefix_idx = 0;
	double final_size = (double) size;

	// TO COMPLETE

	/* 
	 * On divise la taille passée en bytes jusqu'à ce qu'elle soit plus petite que 1024, 
	 * ainsi cela signifie que l'on a atteint la taille à représenter.
	 * A chaque division, on incrémente aussi l'indicateur qui représente l'unitée à utiliser.
	 */
	while (final_size >= BYTES_PER_KILO)
	{
		final_size /= BYTES_PER_KILO;
		prefix_idx++;
	}

	sprintf(buf, "%g%s", final_size, prefix[prefix_idx]);
}

/*
 * Converts given mode to -rwx formatted string
 */
void attr_to_str(mode_t mod, char *buf)
{
	// TO COMPLETE
	if (mod & AM_RD)
		buf[1] = 'r';
	if (mod & AM_WR)
		buf[2] = 'w';
	if (mod & AM_EX)
		buf[3] = 'x';
}

void print_info_file(const char *file, int is_dir, int extended)
{
	int err;
	char date[80];
	char size[80];
	char modes[5] = "----";
	struct stat f_stat;
	struct tm* tm;

	if (!extended)
	{
		printf("%s%s\n", file, is_dir ? "/" : "");
	}
	else
	{
		// TO COMPLETE

		// Récolte des informations sur le fichier donné
		err = stat(file, &f_stat);
		if (err < 0)
		{
			printf("Function stat() error ! Check errno !\n");
			return;
		}


		/* Attributs spéciaux */

		// Vérifie si le fichier est un répertoire ou lien symbolique
		if (is_dir) {

			modes[0] = 'd';
		} else if (f_stat.st_mode & AM_SYM) {

			modes[0] = 'l';

			// On va utiliser un file descriptor pour aller chercher le nom du fichier pointé
			int fd = open(file, O_PATH | O_NOFOLLOW);

			if (fd < 0) {

				printf("Unable to open this symbolic link !\n");
				return;
			}

			char linkTarget[FILENAME_SIZE];

			// Lecture du nom du fichier pointé
			int n = read(fd, linkTarget, FILENAME_SIZE - 1);
			if (n < 0) {

				printf("Unable to read symbolic link !\n");
				if (close(fd) < 0)

					printf("File descriptor closure failed !\n");
				return;
			}
			linkTarget[n] = 0;

			// Création de la chaîne 
			strcat((char*) file, " -> ");
			strcat((char*) file, linkTarget);

			if (close(fd) < 0) {

				printf("Unable to close the file descriptor !\n");
				return;
			}
		}


		/* Taille du fichier */
		human_readable_size(f_stat.st_size, size);

		/* Privilèges */
		attr_to_str(f_stat.st_mode, modes);

		/* Date */
		tm = localtime((const time_t *) &f_stat.st_mtim);
		sprintf(date, "%d-%d-%d %d:%d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min);


		// Résultat final
		printf("%s | %s | %s | %s%s\n", modes, date, size, file, is_dir ? "/" : "");
	}
}

/*
 * Main function of ls application.
 * The ls application is very very short and does not support any options like
 * -l -a etc. It is only possible to give a subdir name to list the directory
 * entries of this subdir.
 */

int main(int argc, char **argv)
{
	char *dir;
	int extended = 0;
	const char *usage = "Usage: ls [-l] [DIR]\n";

	if (argc == 1)
	{
		dir = ".";
	}
	else if ((argc > 1) && (argc < 4))
	{

		if (!strcmp(argv[1], "-l"))
		{
			extended = 1;
			if (argc != 2)
			{
				dir = argv[2];
			}
			else
			{
				dir = ".";
			}
		}
		else if ((argc == 3) && !strcmp(argv[2], "-l"))
		{
			extended = 1;
			dir = argv[1];
		}
		else if (argc == 2)
		{
			dir = argv[1];
		}
		else
		{
			printf("%s", usage);
			exit(1);
		}
	}
	else
	{
		printf("%s", usage);
		exit(1);
	}
	printf("%s\n", dir);

	// Print info for every file in given directory
	// TO COMPLETE. Hint: use opendir and readdir

	// Ouverture du répertoire donné
	DIR *directory = opendir(dir);

	if (directory == NULL)
	{
		printf("Unable to open given directory !\n");
		return EXIT_FAILURE;
	}

	struct dirent *entry;

	// Lecture des fichiers présents dans le répertoire
	while ((entry = readdir(directory)) != NULL)
	{
		print_info_file(entry->d_name, entry->d_type == DT_DIR, extended);
	}

	if (closedir(directory) < 0)
	{
		printf("Unable to close current directory !\n");
		return EXIT_FAILURE;
	}

	exit(0);
}
