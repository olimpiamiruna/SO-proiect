#include <stdio.h>
#include <unistd.h>

int main() {
    const char *targetPath = "text_dir";  // Calea către fișierul țintă
    const char *linkPath = "symlink";      // Calea către link-ul simbolic

    if (symlink(targetPath, linkPath) == 0) {
        printf("Link simbolic creat cu succes.\n");
    } else {
        perror("Eroare la crearea link-ului simbolic");
    }

    return 0;
}
