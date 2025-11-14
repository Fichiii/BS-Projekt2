#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <limits.h>
#include <sys/wait.h>
#include <linux/limits.h>
#include <string.h>
#include <stdbool.h>


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

    //Gaze Zeile wird eingelesen
    char command_line[512];
    //Fehlerbehandlung
    if(fgets(command_line, 512, stdin) == NULL){
        printf("fgets Fehler\n");
        return -1;
    }

    //cutte bis zum erstn '&&'
    char *command_str = strtok(command_line, "&&");

    while (command_str != NULL){
        //Schreibe bis zum '&&' die strings in command und argument
        char command[512];
        char argument[512];

        //Fehlerbehandlung
        int num_arg = sscanf(command_str, "%s %s", command, argument);
        
        if(num_arg <= 0){
            printf("sccanf error\n");
            return -1;
        }

        bool command_valid = false;

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
                //Wenn nur der command ohne Argumente mitgegeben wurde
                if(num_arg == 1){
                    if(execlp(command, command, NULL) == -1){
                perror("execlp Error");
                exit(EXIT_FAILURE);
                }
                exit(EXIT_SUCCESS); //Wird nie passieren, da execlp nie zurückkehrt, aber -Wextra meckert

                }
                //Wenn ein Argument mitgegeben wurde
                else{
                if(execlp(command, command, argument, NULL) == -1){
                perror("execlp Error");
                exit(EXIT_FAILURE);
                }
                exit(EXIT_SUCCESS); //Wird nie passieren, da execlp nie zurückkehrt, aber -Wextra meckert
                }
                
            //Mit waitpid auf den toten Kindprozess warten und diesen entfernen, bevor man in den nächstenDurchlauf geht
            default:
                int status = 0;

                waitpid(pid, &status, 0);
                
                //Wenn Kind normal terminiert ist und von außen getötet wurde, setze command_valid auf 1
                if(WIFEXITED(status)){
                    int exit_status = WEXITSTATUS(status);
                    printf("Exit status: %d\n", exit_status);
                    
                    if(exit_status == 0){
                        command_valid = true;
                    }
                }

        }

        //Wenn der command geklappt hat, mache weiter, sonst brich Schleife ab
        if(command_valid){
                command_str = strtok(NULL, "&&");
            }
            else{
                break;
            }
    }

}

    return 0;
}
