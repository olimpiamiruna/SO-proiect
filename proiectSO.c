#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>

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
    write(fo, buffer, strlen(buffer));

    // Identificatorul utilizatorului
    sprintf(buffer, "identificator utilizator: %d\n", fileStat.st_uid);
    write(fo, buffer, strlen(buffer));

    // Timpul ultimei modificări
    sprintf(buffer, "timpul ultimei modificari: %ld\n", fileStat.st_mtime);
    write(fo, buffer, strlen(buffer));

    // Contorul de legături
    sprintf(buffer, "contorul de legaturi: %ld\n", fileStat.st_nlink);
    write(fo, buffer, strlen(buffer));

}



void writePermission(char pers[10],int fo, struct stat fileStat)
{
    char buffer[128];
    if(strcmp(pers, "user")==0){
        sprintf(buffer, "Drepturi de acces user: ");
    write(fo, buffer, strlen(buffer));

    if (fileStat.st_mode & S_IRUSR){
        sprintf(buffer, "R");
    }
    else sprintf(buffer, "-");
    write(fo, buffer, strlen(buffer));

    if (fileStat.st_mode & S_IWUSR)
        sprintf(buffer, "W");
    else sprintf(buffer, "-");
    write(fo, buffer, strlen(buffer));

     if (fileStat.st_mode & S_IXUSR)
        sprintf(buffer, "X\n");
     else sprintf(buffer, "-\n");
    write(fo, buffer, strlen(buffer));
    }
    else
        if(strcmp(pers,"grup")==0){
            sprintf(buffer, "Drepturi de acces pentru grup: ");
        write(fo, buffer, strlen(buffer));
        if (fileStat.st_mode & S_IRGRP){
            sprintf(buffer, "R");
        }else sprintf(buffer, "-");
        write(fo, buffer, strlen(buffer));

        if (fileStat.st_mode & S_IWGRP){
            sprintf(buffer, "W");
        }else sprintf(buffer, "-");
        write(fo, buffer, strlen(buffer));

        if (fileStat.st_mode & S_IXGRP){
            sprintf(buffer, "X\n");
        }
        else {
            sprintf(buffer, "-\n");
        }
        write(fo, buffer, strlen(buffer));
    }if(strcmp(pers,"altcineva")==0){

    sprintf(buffer, "Drepturi de acces altii: ");
    write(fo, buffer, strlen(buffer));
    if (fileStat.st_mode & S_IROTH){
        sprintf(buffer, "R");
    }else sprintf(buffer, "-");
    write(fo, buffer, strlen(buffer));

    if (fileStat.st_mode & S_IWOTH){
        sprintf(buffer, "W");
    }else sprintf(buffer, "-");
    write(fo, buffer, strlen(buffer));

     if (fileStat.st_mode & S_IXOTH){
        sprintf(buffer, "X\n");
    } else sprintf(buffer, "-\n");
    write(fo, buffer, strlen(buffer));
    }
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
    write(fo, buffer, strlen(buffer));

    size = read(fd,buffer,18); //citesc informatii de care nu am nevoie

    size = read(fd,&width,4); //citesc intr-un int dimensiunea latimiii
    sprintf(buffer, "latime: %d\n",width);
    write(fo,buffer, strlen(buffer));

    size = read(fd,&height,4); //si a inaltimii
    write(fo,buffer, strlen(buffer));

    struct stat fileStat;
    if (fstat(fd,&fileStat) == -1 )
    {perror("eroare");
        close(fd);
        exit(1);
    }

    afisareInf(fo, fileStat);

    writePermission("user",fo,fileStat);

    writePermission("grup",fo,fileStat);

    writePermission("altcineva",fo,fileStat);


 return 0;
}

