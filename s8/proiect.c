#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>

#define maxProc 100
#define maxDir 100

int nrScrieriVect[maxProc];

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
int writeImgDtls(char *outName, struct stat fileStat, char *name, char *iDir, char *oDir) {
    int nrScrieri = 0;
    char buffer[1024];

    chDir("..");
    chDir(oDir);

    int fo = open(outName, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR);
    if (fo == -1)
        eroare("Eroare la deschiderea sau crearea fisierului");

    sprintf(buffer, "nume fisier:  %s\n", name);
    if (write(fo, buffer, strlen(buffer)) == -1)
        eroare("eroare la scriere");
    nrScrieri++;

    // Adaugă informațiile specifice imaginii
    sprintf(buffer, "dimensiune fisier: %ld\n", fileStat.st_size);
    if (write(fo, buffer, strlen(buffer)) == -1)
        eroare("eroare la scriere");

    // Identificatorul utilizatorului
    sprintf(buffer, "identificator utilizator: %d\n", fileStat.st_uid);
    if (write(fo, buffer, strlen(buffer)) == -1)
        eroare("eroare la scriere");

    // Timpul ultimei modificări
    sprintf(buffer, "timpul ultimei modificari: %ld\n", fileStat.st_mtime);
    if (write(fo, buffer, strlen(buffer)) == -1)
        eroare("eroare la scriere");

    // Contorul de legături
    sprintf(buffer, "contorul de legaturi: %ld\n", fileStat.st_nlink);
    if (write(fo, buffer, strlen(buffer)) == -1)
        eroare("eroare la scriere");

    chDir("..");
    chDir(iDir);

    // Deschide fișierul original pentru citire
    int fi = open(name, O_RDONLY);
    if (fi == -1)
        eroare("Eroare la deschiderea fisierului original pentru citire");


    if (read(fi, buffer, 18) == -1)
        eroare("eroare la citirea informatiilor nesemnificative");

    int width, height;

    // Citeste dimensiunea latimii
    if (read(fi, &width, 4) == -1)
        eroare("eroare la citirea latimii");

    sprintf(buffer, "latime: %d\n", width);
    if (write(fo, buffer, strlen(buffer)) == -1)
        eroare("eroare la scriere");
    nrScrieri++;

    // Citeste dimensiunea înălțimii
    if (read(fi, &height, 4) == -1)
        eroare("eroare la citirea inaltimii");

    sprintf(buffer, "inaltime: %d\n", height);
    if (write(fo, buffer, strlen(buffer)) == -1)
        eroare("eroare la scriere");
    nrScrieri++;

    writePermission(fo,fileStat);
    nrScrieri = nrScrieri + 3;

    if(close(fo) == -1)
        eroare("eroare inchidere");
    if(close(fi) == -1)
        eroare("eroare inchidere");

    chDir("..");
    chDir(iDir);

    return nrScrieri;
}

void convtoGray(struct dirent *array)
{
    int size, width, height;
    char useless[1024];
    int fo = open (array->d_name,O_RDWR );
    if(fo == -1)
        eroare("nu s-a putut deschide fisierul");

    read(fo,useless,18); // pana la lungime/latime
    read(fo, &width,4);
    read(fo,&height,4);
    read(fo,useless,28); //pana la ColorTable

    long int nrTotalPix = width * height;
    unsigned char pix[3]; //red, green, blue

    //muta cursorul la inceputul zonelor de pixeli, adica dupa cei 54 de biti din header
    lseek(fo, 54, SEEK_SET);
    //SEEK_SET  ->  mutarea cursorului se face relativ la inceputul fisierului.
    for(long int i = 0; i< nrTotalPix;i++) //citire pixeli din .bmp
    {
        read(fo, pix, sizeof(pix));
        //se calculeaza valoarea gri
        unsigned char gray = 0.299 * pix[2] + 0.587 * pix[1] + 0.114 * pix[0];
        //R, G și B sunt suprascrise/inlocuite cu valoarea de gri
        pix[0] = pix[1] = pix[2] = gray;
        //revenire la inceputul pixelului curent
        lseek(fo, -3, SEEK_CUR);
        //SEEK_CUR  ->  mutarea cursorului se face relativ la pozitia curenta a cursorului.
        //scriere pixel modificat
        write(fo, pix, sizeof(pix));
    }
}


int imgPr(int input, struct dirent *array, struct stat entryStat, char* iDir, char* oDir)
{
    char statisticaName[30];
    strcpy(statisticaName,array->d_name);
    strcat(statisticaName,"_statistica.txt");

    int nrScrieri = writeImgDtls(statisticaName,entryStat,array->d_name,iDir,oDir);

    int pid;
    if ((pid == fork()) < 0 )
        eroare("eroare la crearea procesului secund");

    if(pid == 0)
    {
        convtoGray(array);
        exit(0);
    }

    int status;
    if ( waitpid(pid, &status, 0) == -1 ) {
        exit(-1);
    }

    printf("Cel de-al doilea proces s-a incheiat cu PID-ul %d si codul %d\n", pid, 0);


    return nrScrieri;

}

int winF(int input, char *outName, struct stat fileStat, char *name, char* iDir, char* oDir)
{
    int nrScrieri = 0;
    char buffer[1024];

    chDir("..");
    chDir(oDir);

    int fo = open(outName,O_RDWR);
    if(fo==-1) {
        printf("fisierul %s nu exista\nvom crea fisierul\n", outName);
        fo = creat(outName, S_IRUSR | S_IWUSR | S_IXUSR);
        if(fo ==-1) {
            perror("fisierul nu s-a putut crea");
            return 0;
        }
        else {
            printf("fisierul %s a fost creat cu succes!\n\n", outName);
        }
    }
    else {
        printf("fisierul %s s-a deschis cu succes!\n", outName);
    }

    sprintf(buffer, "nume fisier:  %s\n", name);
    if(write(fo, buffer, strlen(buffer)) == -1)
        eroare("eroare la scriere");
    nrScrieri++;

    if(fstat(fo, &fileStat) == -1)
        eroare("nu s a citit dimensiunea fisierului\n");


    sprintf(buffer, "dimensiune fisier: %ld\n", fileStat.st_size);
    if(write(fo, buffer, strlen(buffer)) == -1)
        eroare("eroare la scriere");
    nrScrieri++;

    sprintf(buffer, "identificator utilizator: %d\n", fileStat.st_uid);
    if(write(fo, buffer, strlen(buffer)) == -1)
        eroare("eroare la scriere");
    nrScrieri++;


  // Timpul ultimei modificări
    sprintf(buffer, "timpul ultimei modificari: %ld\n", fileStat.st_mtime);
   if(write(fo, buffer, strlen(buffer)) == -1)
        eroare("eroare la scriere");
    nrScrieri++;

     // Contorul de legături
    sprintf(buffer, "contorul de legaturi: %ld\n", fileStat.st_nlink);
    if(write(fo, buffer, strlen(buffer)) == -1)
        eroare("eroare la scriere");
    nrScrieri++;

    writePermission(fo,fileStat);
    nrScrieri = nrScrieri + 3;
    chDir("..");
    chDir(iDir);

    return nrScrieri;
}



int writeFilePr(int input, struct dirent *array, struct stat entryStat, char *iDir, char* oDir) {

    char statisticaName[30];
    strcpy(statisticaName,array->d_name);
    strcat(statisticaName,"_statistica.txt");

    int nrScrieri = winF(input,statisticaName,entryStat,array->d_name,iDir,oDir);

    if(nrScrieri != 0)
        printf("detaliile fisierului %s au fost scrise in %s!\n", array->d_name, statisticaName);

    return nrScrieri;
}

int WinDir(char *outName, struct stat fileStat, char *name, char* iDir, char* oDir)
{
    int nrScrieri = 0;
    char buffer[1024];

    chDir("..");
    chDir(oDir);

    int fo = open(outName,O_RDWR);
    if(fo==-1) {
        printf("fisierul %s nu exista\nvom crea fisierul\n", outName);
        fo = creat(outName, S_IRUSR | S_IWUSR | S_IXUSR);
        if(fo ==-1) {
            perror("fisierul nu s-a putut crea");
            return 0;
        }
        else {
            printf("fisierul %s a fost creat cu succes!\n\n", outName);
        }
    }
    else {
        printf("fisierul %s s-a deschis cu succes!\n", outName);
    }

    sprintf(buffer, "nume director:  %s\n", name);
    if(write(fo, buffer, strlen(buffer)) == -1)
        eroare("eroare la scriere");
    nrScrieri++;

    sprintf(buffer, "identificator utilizator: %d\n", fileStat.st_uid);
    if(write(fo, buffer, strlen(buffer)) == -1)
        eroare("eroare la scriere");
    nrScrieri++;
    if(write(fo, buffer, strlen(buffer)) == -1)
        eroare("eroare la scriere");
    nrScrieri = nrScrieri + 3;

    chDir("..");
    chDir(iDir);

    return nrScrieri;
}

int writeDirPerm(struct dirent *array, struct stat entryStat, char *iDir, char* oDir)
{
    char statisticaName[30];
    strcpy(statisticaName,array->d_name);
    strcat(statisticaName,"_statistica.txt");

    int nrScrieri = WinDir(statisticaName,entryStat,array->d_name,iDir,oDir);
    return nrScrieri;
}


int WinSymLink(char *outName, struct stat fileStat, struct stat targetStat, char *name, char* iDir, char* oDir)
{
    int nrScrieri = 0;
    char buffer[1024];

    chDir("..");
    chDir(oDir);

    int fo = open(outName,O_RDWR);
    if(fo==-1) {
        printf("fisierul %s nu exista\nvom crea fisierul\n", outName);
        fo = creat(outName, S_IRUSR | S_IWUSR | S_IXUSR);
        if(fo ==-1) {
            perror("fisierul nu s-a putut crea");
            return 0;
        }
        else {
            printf("fisierul %s a fost creat cu succes!\n\n", outName);
        }
    }
    else {
        printf("fisierul %s s-a deschis cu succes!\n", outName);
    }

    sprintf(buffer, "nume legatura:  %s\n", name);
    if(write(fo, buffer, strlen(buffer)) == -1)
        eroare("eroare la scriere");
    nrScrieri++;

    sprintf(buffer, "dimensiune legatura: %ld\n", fileStat.st_size);
    if(write(fo, buffer, strlen(buffer)) == -1)
        eroare("eroare la scriere");
    nrScrieri++;

    sprintf(buffer, "dimensiune fisier target : %ld\n", targetStat.st_size);
    if(write(fo, buffer, strlen(buffer)) == -1)
        eroare("eroare la scriere");
    nrScrieri++;

    writePermission(fo,fileStat);
    nrScrieri = nrScrieri + 3;

    chDir("..");
    chDir(iDir);

    return nrScrieri;
}

int writeSymLinkPerm(struct dirent *array, struct stat fileStat, char *iDir, char* oDir)
{
    char statisticaName[30];
    strcpy(statisticaName,array->d_name);
    strcat(statisticaName,"_statistica.txt");

    struct stat targetStat;

    char buff[1024];
    char *r = realpath( array->d_name, buff);
    stat(r,&targetStat);

    int nrScrieri = WinSymLink(statisticaName,fileStat,targetStat,array->d_name,iDir,oDir);

    return nrScrieri;

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

    int pid[maxProc];
    int status = 0;

    struct stat entryStat;

    struct dirent *entry = readdir(dir);
    struct dirent *entryVect[maxDir];

    chDir(argv[1]);

    int i = 0;

    while (entry != NULL && i < maxDir) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
            entryVect[i++] = entry;
        entry = readdir(dir);
    }

    int fileOut;
    int intrari = i;


    for(int i = 0;i <intrari;i++)
    {
        if((pid[i] = fork() )< 0)
            eroare("eroare la crearea procesului");
        if(pid[i] == 0){

            if(lstat(entryVect[i]->d_name, &entryStat) == -1)
                eroare("error");
            int nrofWLines = 0; // linii scrise in statistica.txt

            if(entryVect[i]->d_type == DT_REG){
                printf("fisier\n");
                if (strcmp(FileSuffix(entryVect[i]->d_name), ".bmp") == 0){
                    nrofWLines = imgPr(fileOut,entryVect[i],entryStat,argv[1],argv[2]);
                }
                else
                {
                    nrofWLines = writeFilePr(fileOut,entryVect[i], entryStat, argv[1], argv[2]);

                }
            }

            if(entryVect[i]->d_type == DT_DIR)
            {
                nrofWLines = writeDirPerm(entryVect[i], entryStat, argv[1], argv[2]);
            }

            if(entryVect[i]->d_type == DT_LNK)
            {
                printf("symlinl");
                nrofWLines = writeSymLinkPerm(entryVect[i], entryStat, argv[1], argv[2]);
            }
        exit(nrofWLines);
        }
    }

    for(int j = 0; j < intrari; j++) {
        if ( waitpid(pid[j], &status, 0) == -1 ) {
            perror("waitpid failed");
            return EXIT_FAILURE;
        }
        if ( WIFEXITED(status) ) {
            const int exit_status = WEXITSTATUS(status);
            nrScrieriVect[j] = exit_status;
        }
    }
         printf("S-a încheiat procesul cu pid-ul %d și codul %d\n", pid[i], nrScrieriVect[i]);




     for(i = 0; i < intrari; i++) {
        printf("proces %d: nr scrieri fisier %s = %d\n", pid[i], entryVect[i]->d_name, nrScrieriVect[i]);
     }


    if(closedir(dir) == -1)
        eroare("directorul nu s-a putut inchide");

    if(closedir(outDir) == -1)
        eroare("directorul nu s-a putut inchide");


    return 0;
}
