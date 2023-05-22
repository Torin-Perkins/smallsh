#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>

struct command{
    char fullCommand[2049];
    char *command;
    char *arguments;
    char *input;
    char *output;
    int background;
    int exitStatus;
};
typedef struct command Command;
int checkComment(char command[2049]){
    printf("checking comment\n");
    fflush(stdout);

    //fflush(stdout);
    //char comment[2] = "#";
    if(command[0] == '#'){
        return 1;
    }
    else{
        return 0;
    }
}
int checkBuiltInCommands(char command[2049]){
    char exit_command[] = "exit";
    char cd[] = "cd\0";
    char status[] = "status\0";


    if(strcmp(command, exit_command) == 0){
        return 0;
    }
    else if(strcmp(command, status) == 0){
        printf("%d\n", getpid());
        return 1;
    }
    
    else if(command[0] == cd[0] && command[1] == command[1]){
        if(strcmp(command, cd) == 0){
            return 2;
        }
        else{
            return 3;
        }
    }
    
    else {
        return -1;
    }
}
void runningLoop(){
    
    
    int status = 0;
    
    while(1){
        char full_command[2049];
        printf(": ");
        gets(full_command);
       
        fflush(stdout);
        Command * newCommand = malloc(sizeof(Command));
        strcpy(newCommand -> fullCommand, full_command);
        
        if(checkComment(full_command) == 1){
            continue;
        }
        
        if(full_command[0] == '\0'){
            continue;
        }
        

        switch (checkBuiltInCommands(full_command))
        {
        case -1:
            break;
        case 0:
            exit(0);
            break;
        case 1:
            printf("Status: %d\n", status);
            fflush(stdout);
            break;
        case 2:
            chdir("./");
            break;
        case 3:
            { 
            char dir[2049];
            int i = 3;
            int j = 0;
            while(full_command[i] != '\0'){
                dir[j] = full_command[i];
                j += 1;
                i += 1;
            }
            j += 1;
            dir[j] = '\0';
            printf("changing dir %d\n", chdir(dir));
            char cwd[PATH_MAX];
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                printf("Current working dir: %s\n", cwd);
            } else {
                perror("getcwd() error");
                return 1;
            }
            }
        default:
            break;
        }



    }
}





void parseCommand(char* full_command, Command *newCommand){
    int i = 0;
    char *initialCommand;
    char *arguments;
    char *input;
    char *output;

    int stage = 0;

    while(full_command[i] != '/0'){
        if(stage == 0){
            int c = 0;
            if(full_command[i] != ' '){
                initialCommand[c] = full_command[i];
                c += 1;
                if(full_command[i + 1] = '\0'){
                    strcpy(newCommand -> command, initialCommand);
                    break;
                }
            }
            else{
                strcpy(newCommand -> command, initialCommand);
                stage += 1;
            }
        }
        else if(stage == 1){
            int a = 0;
            if(full_command[i] != ' '){
                arguments[a] = full_command[i];
                a += 1;
                if(full_command[i + 1] = '\0'){
                    strcpy(newCommand -> arguments, arguments);
                    break;
                }
            }
            else{
                if(full_command[i + 1] = '\0'){
                    strcpy(newCommand -> arguments, arguments);
                    break;
                }
                else if(full_command[i + 1] == '>' || full_command[i + 1] == '<'){
                    strcpy(newCommand -> arguments, arguments);
                    stage += 1;
                }
                else{
                    continue;
                }
                
            }
        }
    }
}
int main(int argc, char *argv[]){
    runningLoop();
}