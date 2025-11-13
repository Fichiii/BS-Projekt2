#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <limits.h>
#include <sys/wait.h>
#include <linux/limits.h>


//Aufgabe 1-4 bisher

int main(void) {

while (1){

    //Username kriegen
    uid_t id = getuid();
    struct passwd *pw = getpwuid(id);
    if(pw == NULL){
        perror("pw Error\n");
        exit(EXIT_FAILURE);
    }
    //Hostname kriegen
    char hostname[_SC_HOST_NAME_MAX];
    if(gethostname(hostname, _SC_HOST_NAME_MAX) != 0){
        perror("hostname Error\n");
        exit(EXIT_FAILURE);
    }
    //Arbeitsverzeichnis kriegen
    char buf[PATH_MAX];
    if(getcwd(buf, PATH_MAX) == NULL){
        perror("buf Error\n");
        exit(EXIT_FAILURE);
    }

    //Alle printen
    printf("%s@%s  %s\n", pw -> pw_name, hostname, buf);

    //Befehl und Argument einlesen
    char command[64];
    char argument[64];
    //Fehlerbehandlung
    if(scanf("%63s %63s", command, argument) < 2){
        printf("scanf error");
        return -1;
    }
    
    while(getchar() != '\n'){} //Buffer leeren

    printf("Befehl: %s\nArgument: %s\n", command, argument);


    //Kindprozess erstellen
    pid_t pid;

    pid = fork();

    switch(pid){
        //Wenn einFehler auftritt beende
        case -1:
            perror("fork Fehler");
            exit(EXIT_FAILURE);
        //Hier geht das Kindprozess rein und führt execlp aus
        case 0:
            if(execlp(command, command, argument, NULL) == -1){
            perror("execlp Error");
            exit(EXIT_FAILURE);
            }
            exit(EXIT_SUCCESS); //Wird nie passieren, da execlp nie zurückkehrt, aber -Wextra meckert
            
        //Mit waitpid auf den toten Kindprozess warten und diesen entfernen, bevor man in den nächstenDurchlauf geht
        default:
            waitpid(pid, NULL, 0);
    }

    //TODO Aufgabe 5 hier machen
}

    return 0;
}
