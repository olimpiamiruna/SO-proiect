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

void chDir(const char *dir) {
    char cwd[PATH_MAX]; // director curent

    if (chdir(dir) != 0) {
        perror("Eroare la schimbarea directorului");
        exit(-1);
    }

    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("Eroare la obținerea directorului curent");
        exit(-1);
    }
}

void writeFileInfo(int fo, const char *filePath) {
    char buffer[1024];
    int size, width, height;

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
    if(write(fo, buffer, strlen(buffer)) == -1)
        eroare("eroare la scriere");

    // Citeste dimensiunea înălțimii
    size = read(fd, &height, 4);
    sprintf(buffer, "inaltime: %d\n", height);
    if(write(fo, buffer, strlen(buffer)) == -1)
        eroare("eroare la scriere");
}

void afisareInf(int fo, struct stat fileStat)
{
    char buffer[128];
    sprintf(buffer, "dimensiune fisier: %ld\n", fileStat.st_size);
    if(write(fo, buffer, strlen(buffer)) == -1)
        eroare("eroare la scriere");

    // Identificatorul utilizatorului
    sprintf(buffer, "identificator utilizator: %d\n", fileStat.st_uid);
    if(write(fo, buffer, strlen(buffer)) == -1)
        eroare("eroare la scriere");

    // Timpul ultimei modificări
    sprintf(buffer, "timpul ultimei modificari: %ld\n", fileStat.st_mtime);
     if(write(fo, buffer, strlen(buffer)) == -1)
        eroare("eroare la scriere");

    // Contorul de legături
    sprintf(buffer, "contorul de legaturi: %ld\n", fileStat.st_nlink);
    if(write(fo, buffer, strlen(buffer)) == -1)
        eroare("eroare la scriere");

}


void writePermission(int fo, struct stat fileStat)
{
    char buffer[128];
    sprintf(buffer, "Drepturi de acces user: ");
    if(write(fo, buffer, strlen(buffer)) == -1)
        eroare("eroare la scriere");

    if (fileStat.st_mode & S_IRUSR)
        sprintf(buffer, "R");
    else sprintf(buffer, "-");
    if(write(fo, buffer, strlen(buffer)) == -1)
        eroare("eroare la scriere");

    if (fileStat.st_mode & S_IWUSR)
        sprintf(buffer, "W");
    else sprintf(buffer, "-");
    if(write(fo, buffer, strlen(buffer)) == -1)
        eroare("eroare la scriere");

     if (fileStat.st_mode & S_IXUSR)
        sprintf(buffer, "X\n");
     else sprintf(buffer, "-\n");
     if(write(fo, buffer, strlen(buffer)) == -1)
        eroare("eroare la scriere");

    sprintf(buffer, "Drepturi de acces pentru grup: ");
     if(write(fo, buffer, strlen(buffer)) == -1)
        eroare("eroare la scriere");
    if (fileStat.st_mode & S_IRGRP){
        sprintf(buffer, "R");
    }else sprintf(buffer, "-");
     if(write(fo, buffer, strlen(buffer)) == -1)
        eroare("eroare la scriere");

    if (fileStat.st_mode & S_IWGRP){
        sprintf(buffer, "W");
    }else sprintf(buffer, "-");
     if(write(fo, buffer, strlen(buffer)) == -1)
        eroare("eroare la scriere");

    if (fileStat.st_mode & S_IXGRP)
        sprintf(buffer, "X\n");
    else
        sprintf(buffer, "-\n");
    if(write(fo, buffer, strlen(buffer)) == -1)
        eroare("eroare la scriere");

    sprintf(buffer, "Drepturi de acces altii: ");
   if(write(fo, buffer, strlen(buffer)) == -1)
        eroare("eroare la scriere");
    if (fileStat.st_mode & S_IROTH){
        sprintf(buffer, "R");
    }else sprintf(buffer, "-");
     if(write(fo, buffer, strlen(buffer)) == -1)
        eroare("eroare la scriere");

    if (fileStat.st_mode & S_IWOTH){
        sprintf(buffer, "W");
    }else sprintf(buffer, "-");
     if(write(fo, buffer, strlen(buffer)) == -1)
        eroare("eroare la scriere");

     if (fileStat.st_mode & S_IXOTH){
        sprintf(buffer, "X\n");
    } else sprintf(buffer, "-\n");
     if(write(fo, buffer, strlen(buffer)) == -1)
        eroare("eroare la scriere");
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        eroare("usage ./program <director_intrare>\n");
    }

    DIR *dir = opendir(argv[1]);
    if (dir == NULL) {
        eroare("directorul nu s-a deschis.");
    }

    int fo = open("statistica.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fo == -1) {
        eroare("Eroare la deschiderea fisierului de statistica");
    }


    char buffer[1024];
    struct stat entryStat;

    chDir(argv[1]);


    struct dirent *entry = readdir(dir);
    while (entry != NULL) {
    if ((lstat(entry->d_name, &entryStat)) == -1) {
        eroare("eroare la verificarea fisierelor de tip symbolic link");
    }

    if (entry->d_type == DT_REG || entry->d_type == DT_DIR) {

        if (write(fo, entry->d_name, strlen(entry->d_name)) == -1)
            eroare("eroare la scriere");
        write(fo, "\n", 1);

        if (entry->d_type == DT_REG) {
            // Este un fișier obișnuit
            if (strcmp(FileSuffix(entry->d_name), ".bmp") == 0) {
                writeFileInfo(fo, entry->d_name);
                afisareInf(fo, entryStat);
                writePermission(fo, entryStat);
                write(fo, "\n", 1);
            } else {
                afisareInf(fo, entryStat);
                writePermission(fo, entryStat);
                write(fo, "\n", 1);
            }
        } else if (entry->d_type == DT_DIR) {

            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                entry = readdir(dir);
                write(fo, "\n", 1);
                continue;
            }

            sprintf(buffer, "Nume director: %s\n", entry->d_name);
            if (write(fo, buffer, strlen(buffer)) == -1)
                eroare("eroare la scriere");
            sprintf(buffer, "identificator utilizator: %d\n", entryStat.st_uid);
            if (write(fo, buffer, strlen(buffer)) == -1)
                eroare("eroare la scriere");
            writePermission(fo, entryStat);
            write(fo, "\n", 1);
        }
        else
            if(entry->d_type == DT_LNK)
            {

                if(write(fo, entry->d_name, strlen(entry->d_name)) == -1)
                    eroare("eroare la scriere");

                struct stat linkStat;
                //transforma calea relativa in cale absoluta
                char *r = realpath(entry->d_name, buffer); //r contine adresa bufferului ce contine calea absoluta
                //obtinere informatii despre calea absoluta
                stat(r, &linkStat);


                sprintf(buffer, "Dimensiune legatura: %ld\n", (long)linkStat.st_size);
                if (write(fo, buffer, strlen(buffer)) == -1)
                eroare("eroare la scriere");
                write(fo,"\n",1);

                sprintf(buffer, "Dimensiune fisier target: %ld\n", (long)linkStat.st_size);
                if (write(fo, buffer, strlen(buffer)) == -1)
                    eroare("eroare la scriere");
                writePermission(fo,entryStat);
                write(fo,"\n",1);
            }

    }

    entry = readdir(dir);
}


    if(closedir(dir) == -1)
        eroare("Directorul nu s-a putut inchide");

    if(close(fo) == -1)
        eroare("Fisierul nu s-a putut inchide");

   

    return 0;
}
