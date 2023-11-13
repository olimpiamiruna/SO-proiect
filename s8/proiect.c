#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <ctype.h>

void eroare(const char *message) {
    perror(message);
    exit(-1);
}

const char *FileSuffix(const char path[]) {
    const char *result;
    int n = strlen(path);
    int i = n - 1;
    while ((i > 0) && (path[i] != '.') && (path[i] != '/') && (path[i] != '\\')) {
        i--;
    }
    if ((i > 0) && (i < n - 1) && (path[i] == '.') && (path[i - 1] != '/') && (path[i - 1] != '\\')) {
        result = path + i;
    } else {
        result = path + n;
    }
    return result;
}
void chDir(const char* dir) {
    char cwd[PATH_MAX];
    chdir(dir);

    if (getcwd(cwd, sizeof(cwd)) != NULL)
    printf("Current working dir: %s\n\n", cwd);
     else
        eroare("getcwd() eroare");

}

void writeFileInfo(int fo, const char *filePath) {
    char buffer[1024];
    int size, width, height;

    chDir("intrare");
    // Deschide fișierul pentru citire
    int fd = open(filePath, O_RDONLY);
    if (fd == -1) {
        perror("Eroare la deschiderea fișierului");
        return;
    }

    // Citeste informatii pe care nu le folosesti
    size = read(fd, buffer, 18);

    // Citeste dimensiunea latimii
    size = read(fd, &width, 4);
    sprintf(buffer, "latime: %d\n", width);
    write(fo, buffer, strlen(buffer));

    // Citeste dimensiunea înălțimii
    size = read(fd, &height, 4);
    sprintf(buffer, "inaltime: %d\n", height);
    write(fo, buffer, strlen(buffer));

    chDir("..");
    // Închide fișierul
    if(close(fd) == -1 )
    {
    eroare("fisierul nu s-a putut inchide");
    }
}



int createFile(const char *fout)
{
    int fo = open (fout,O_RDWR );
    if(fo == -1)
    {
        printf  ("fisierul nu exista \n");
        fo = creat (fout, S_IRUSR | S_IWUSR | S_IXUSR);
        if (fo == -1 )
        {
            eroare("fisierul nu s-a creat");
        }
        else
            printf("fisierul s-a creat cu succes");


    }else
        printf("fisierul s-a deschis cu succes!!\n");

    return fo;

}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        eroare("nu sunt destule argumente!");
    }

    DIR *dir = opendir(argv[1]);
    if (dir == NULL) {
        eroare("directorul de intrare nu s-a deschis.");
    }

    DIR *outDir = opendir(argv[2]);
    if (outDir == NULL) {
        eroare("directorul de iesire nu s-a deschis.");
    }
    chDir(argv[2]);

    int fo = createFile("statistica.txt");


    chDir("..");

    struct stat dirStat;
    if (fstat(dirfd(dir), &dirStat) == -1) {
        closedir(dir);
        close(fo);
        eroare("eroare");
    }

    char buffer[1024];

    struct dirent *entry = readdir(dir);
    while (entry != NULL) {
            if (strcmp(FileSuffix(entry->d_name), ".bmp") == 0){
                write(fo, entry->d_name, strlen(entry->d_name));
                write(fo,"\n",1);
                writeFileInfo(fo, entry->d_name);

                write(fo,"\n",1);
            }

            // if(entry->d_type == DT_LNK)
            // {
            //     //printf("legatura simbolica!!!!\n");
            //     char buffer[1024];
            //     write(fo, entry->d_name, strlen(entry->d_name));
            //
            //     struct stat linkStat;
            //     if (lstat(entry->d_name, &linkStat) == -1) {
            //         perror("Eroare la obținerea informațiilor despre legătură");
            //
            //     }
            //
            //     sprintf(buffer, "Dimensiune legatura: %ld\n", (long)linkStat.st_size);
            //     write(fo, buffer, strlen(buffer));
            //     write(fo,"\n",1);
            //
            //     struct stat targetStat;
            //     if (stat(entry->d_name, &targetStat) == 0) {
            //         sprintf(buffer, "Dimensiune fisier target: %ld\n", (long)targetStat.st_size);
            //         write(fo, buffer, strlen(buffer));
            //
            //     } else {
            //         perror("Eroare la obtinerea informatiilor despre fisierul target");
            //     }
            //      write(fo,"\n",1);
            // }
            //
            // if(entry->d_type == DT_DIR )
            // {
            //  //   printf("director !!!!\n");
            //
            //     write(fo, entry->d_name, strlen(entry->d_name));
            //     write(fo,"\n",1);
            //     sprintf(buffer, "identificator utilizator: %d\n", dirStat.st_uid);
            //     write(fo, buffer, strlen(buffer));
            //
            //
            //     write(fo,"\n",1);
            // }
            //
            // if(entry->d_type == DT_REG )
            // {
            //
            //     //fisier obisnuit, deci nu se scrie nimic in statistica!!!!
            // }


        entry = readdir(dir);
    }

    if(closedir(dir) == -1) {
    eroare("directorul nu s-a putut inchide");
  }
  if(close(fo) == -1) {
    eroare("fisierul nu s-a putut inchide");
  }


    return 0;
}
