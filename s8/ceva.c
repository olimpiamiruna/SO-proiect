#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
int main(void) {

int pids[19];

printf("id proces curent(parinte) - %d\n", getpid());


for(int i = 0; i <= 19; i++) {
    if((    pids[i] = fork()) < 0) {
        perror("EROARE!!!");
        exit(-1);
        }
    if(pids[i] == 0) {
// cod fiu
    printf("id proces curent(fiu nr %d) - %d\n", i, getpid());
// inchidem procesul fiului
    exit(0);
    }
    printf("id proces curent(fiu nr %d) - %d (PARINTE)\n", i, pids[i]);
}
return 0;
}
