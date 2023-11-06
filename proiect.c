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

    char nume_fisier[20];

    char useless_buffer[1024] ; // informatiile pe care nu mi le trebuie
    int size = 0;
    int inaltime, latime;
    //sprintf(nume_fisier,"nume fisier : %s",argv[1]);
    //write(fo, nume_fisier,strlen(argv[1]));
    size = read(fd,useless_buffer,18); //citesc informatii inutile
  //  size = read(fo, &dimensiuneFisier,4);
  //  printf("dimensiunea fisierului este de : %d bytes\n", dimensiuneFisier);

    size = read(fd,&latime,4); //citesc intr-un int dimensiunea latimiii
     sprintf(useless_buffer, "latime=%d\n",size);
     write(fo,useless_buffer, strlen(useless_buffer));
  //  printf("latime : %d\n",latime);
    size = read(fd,&inaltime,4); //si a inaltimii
    sprintf(useless_buffer, "inaltime=%d\n",size);
     write(fo,useless_buffer, strlen(useless_buffer));
//    printf("inaltime : %d\n",inaltime);



    struct stat fileStat;
    if (fstat(fd,&fileStat) == -1 )
    {perror("eroare");
        close(fd);
        exit(1);
    }

 //   printf("dimensiunea fisierului este : %ld\n",fileStat.st_size);
    sprintf(useless_buffer, "dimensiune fisier =%ld\n",fileStat.st_size);
     write(fo,useless_buffer, strlen(useless_buffer));
   // printf("user id : %d\n", fileStat.st_uid);
     sprintf(useless_buffer, "identificator utilizator =%d\n",fileStat.st_uid);
     write(fo,useless_buffer, strlen(useless_buffer));
    return 0;

}
