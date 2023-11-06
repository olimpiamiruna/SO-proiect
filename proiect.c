#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>

char useless_buffer[1024] ; // stocam informatiile pe care nu mi le trebuia din headerul unei imagini


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

void afisareNume(int fo, char numefis[10])
{
      sprintf(useless_buffer,"nume fisier : %s \n",numefis);
    write(fo,useless_buffer,strlen(useless_buffer)); //scriu numele fisierului
}

void writeInBuffer(int fo)
{
    write(fo,useless_buffer, strlen(useless_buffer));
}

void afisareInf(int fo, struct stat fileStat) {
    char useless_buffer[128]; // Alocăm un buffer suficient de mare pentru toate informațiile.

    // Dimensiunea fișierului
    sprintf(useless_buffer, "dimensiune fisier: %ld\n", fileStat.st_size);
    write(fo, useless_buffer, strlen(useless_buffer));

    // Identificatorul utilizatorului
    sprintf(useless_buffer, "identificator utilizator: %d\n", fileStat.st_uid);
    write(fo, useless_buffer, strlen(useless_buffer));

    // Timpul ultimei modificări
    sprintf(useless_buffer, "timpul ultimei modificari: %ld\n", fileStat.st_mtime);
    write(fo, useless_buffer, strlen(useless_buffer));

    // Contorul de legături
    sprintf(useless_buffer, "contorul de legaturi: %ld\n", fileStat.st_nlink);
    write(fo, useless_buffer, strlen(useless_buffer));
}



void writePermissions(char pers[10],int fo, struct stat fileStat)
{
    if(strcmp(pers, "user")==0)
    {
        sprintf(useless_buffer, "Drepturi de acces user:");
        writeInBuffer(fo);

        if (fileStat.st_mode & S_IRUSR){
            sprintf(useless_buffer, "R");
        }
        else
            sprintf(useless_buffer, "-");

        writeInBuffer(fo);

        if (fileStat.st_mode & S_IWUSR){
            sprintf(useless_buffer, "W");
        }else sprintf(useless_buffer, "-");
        writeInBuffer(fo);

        if (fileStat.st_mode & S_IXUSR){
            sprintf(useless_buffer, "X\n");
        } else sprintf(useless_buffer, "-\n");
        writeInBuffer(fo);
    }
    else

        if(strcmp(pers,"grup")==0){
            sprintf(useless_buffer, "Drepturi de acces  pentru grup:");
        writeInBuffer(fo);
        if (fileStat.st_mode & S_IRGRP){
            sprintf(useless_buffer, "R");
        }else sprintf(useless_buffer, "-");
        writeInBuffer(fo);

        if (fileStat.st_mode & S_IWGRP){
            sprintf(useless_buffer, "W");
        }else sprintf(useless_buffer, "-");
        writeInBuffer(fo);

        if (fileStat.st_mode & S_IXGRP){
            sprintf(useless_buffer, "X\n");
        }
        else {
            sprintf(useless_buffer, "-\n");
        }
        writeInBuffer(fo);
    }
    else
        if(strcmp(pers,"oricine")==0){
            sprintf(useless_buffer, "Drepturi de acces altii:");
        writeInBuffer(fo);
        if (fileStat.st_mode & S_IROTH){
            sprintf(useless_buffer, "R");
        }else sprintf(useless_buffer, "-");
        writeInBuffer(fo);

        if (fileStat.st_mode & S_IWOTH){
            sprintf(useless_buffer, "W");
        }else sprintf(useless_buffer, "-");
        writeInBuffer(fo);

        if (fileStat.st_mode & S_IXOTH){
            sprintf(useless_buffer, "X\n");
        } else sprintf(useless_buffer, "-\n");
        writeInBuffer(fo);
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
        printf("fisierul s-a deschis cu succes!!\n!");
    }

    int size = 0;
    int inaltime, latime;

    afisareNume(fo,argv[1]);

    size = read(fo,useless_buffer,18); //citesc informatii de care nu am nevoie

    size = read(fo,&latime,4); //citesc intr-un int dimensiunea latimiii
    sprintf(useless_buffer, "latime: %d\n",latime); //suprascriu ce am in acest buffer, pentru ca nu am nevoie de acei  18 biti oricum

    writeInBuffer(fo);// scriu continutul din buffer in fisier

    size = read(fd,&inaltime,4); //si a inaltimii
    sprintf(useless_buffer, "inaltime: %d\n",inaltime);

    writeInBuffer(fo);

    ////////
    struct stat fileStat;
    if (fstat(fd,&fileStat) == -1 )
    {perror("eroare");
        close(fd);
        exit(1);
    }



    //dimensiune fisier

    afisareInf(fo,fileStat);


    writePermissions("user",fo,fileStat);
    //
    // ////////
    //
    writePermissions("grup",fo,fileStat);
    //
    // ////////
    writePermissions("oricine",fo,fileStat);


    return 0;

}
