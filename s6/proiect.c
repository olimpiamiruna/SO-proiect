#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>

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
    }
    else {
    result = path + n;}
return result;
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

    sprintf(buffer, "Drepturi de acces grup: ");
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

int main(int argc, char **argv)
{
    if(argc != 2){
        printf("Usage ./program ");
        exit(-1);
    }

    //verificare extensie
    if(strcmp(FileSuffix(argv[1]), ".bmp") !=0)
    {
        printf("fisierul de intrare nu are extensia buna");
        exit(-1);
    }

    int fd = open(argv[1], O_RDWR);

    if(fd == -1){
        printf("\nError Opening File!!\n");
        exit(-1);
    }
    else {
        printf("fisierul %s s-a deschis cu succes!\n", argv[1]);
    }

    char fout[]="statistica.txt";
    int fo = open (fout,O_RDWR );
    if(fo == -1)
    {
        printf  ("fisierul nu exista \n");
        fo = creat (fout, S_IRUSR | S_IWUSR | S_IXUSR);
        if (fo == -1 )
        {
            printf("fisierul nu s-a creat");
            exit(-1);
        }
        else{
            printf("fisierul s-a creat cu succes");
        }

    }else {
        printf("fisierul s-a deschis cu succes!!\n");
    }

    char buffer[1024] ; // stocam informatiile pe care nu mi le trebuia din headerul unei imagini
    int size = 0;
    int height, width;

    //scriere numele fisierului
    sprintf(buffer, "nume fisier:  %s\n", argv[1]);
    if(write(fo, buffer, strlen(buffer)) == -1)
        eroare("eroare la scriere");

    size = read(fd,buffer,18); //citesc informatii de care nu am nevoie

    size = read(fd, &width, 4); // Citesc dimensiunea latimii
    sprintf(buffer, "latime: %d\n", width);
    if(write(fo, buffer, strlen(buffer)) == -1)
        eroare("eroare la scriere");

    size = read(fd, &height, 4); // Citesc dimensiunea înălțimii
    sprintf(buffer, "inaltime: %d\n", height);
    if(write(fo, buffer, strlen(buffer)) == -1)
        eroare("eroare la scriere");


    struct stat fileStat;
    if (fstat(fd,&fileStat) == -1 )
    {perror("eroare");
        close(fd);
        exit(1);
    }

    afisareInf(fo, fileStat);

    writePermission(fo,fileStat);

    if(close(fo) == -1)
        eroare("fisierul nu s-a putut inchide");
    if(close(fd) == -1)
        eroare("fisierul nu s-a putut inchide");

 return 0;
}

