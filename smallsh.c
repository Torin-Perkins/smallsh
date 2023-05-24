#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>

struct argument
{
    char argument_text[2049];
    struct argument *next;
};
typedef struct argument Argument;

struct command
{
    char fullCommand[2049];
    char command[2049];
    Argument *first;
    char input[2049];
    char output[2049];
    int background;
    int exitStatus;
};
typedef struct command Command;

int countArgument(Argument *first)
{
    Argument *temp = first;
    int counter = 0;
    while (temp != NULL)
    {
        temp = temp->next;
        counter += 1;
    }
    return counter;
}

Argument *getArgumentAtIndex(Argument *first, int index)
{
    Argument *temp = first;
    int counter = 0;
    while (counter != index)
    {
        temp = temp->next;
        if (temp == NULL)
        {
            break;
        }
        counter += 1;
    }
    return temp;
}

void printArguments(Argument *first)
{
    Argument *temp = first;
    while (temp != NULL)
    {
        printf("%s\n", temp->argument_text);
        temp = temp->next;
    }
}
int checkComment(char command[2049])
{
    printf("checking comment\n");
    fflush(stdout);

    // fflush(stdout);
    // char comment[2] = "#";
    if (command[0] == '#')
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int checkBuiltInCommands(char command[2049])
{
    char exit_command[] = "exit";
    char cd[] = "cd\0";
    char status[] = "status\0";

    if (strcmp(command, exit_command) == 0)
    {
        return 0;
    }
    else if (strcmp(command, status) == 0)
    {
        printf("%d\n", getpid());
        return 1;
    }

    else if (command[0] == cd[0] && command[1] == command[1])
    {
        if (strcmp(command, cd) == 0)
        {
            return 2;
        }
        else
        {
            return 3;
        }
    }

    else
    {
        return -1;
    }
}

void parseCommand(char full_command[2049], Command *newCommand)
{

    char initialCommand[2049];
    char arguments[2049];
    char input[2049];
    char output[2049];

    int stage = 0;
    int a = 0;
    int b = 0;
    int c = -2;
    int d = -2;
    int numArgs = 0;

    Argument *head = NULL;
    Argument *tail = NULL;

    printf("Parsing command of length: %d\n", strlen(full_command));

    for (int i = 0; i <= strlen(full_command); i++)
    {
        printf("Inting on iteration %d with char %c\n", i, full_command[i]);
        // fflush(stdout);

        if (stage == 0)
        {
            printf("Stage 0\n");
            fflush(stdout);
            if (full_command[i] != ' ')
            {
                initialCommand[a] = full_command[i];
                a = a + 1;
                if (full_command[i + 1] == '\0')
                {
                    // printf("this is weird\n");

                    printf("Ending Search: Initial Command: %s\n", initialCommand);
                    fflush(stdout);
                    strcpy(newCommand->command, initialCommand);
                    break;
                }
            }
            else
            {
                strcpy(newCommand->command, initialCommand);

                if((full_command[i+1] == '>' || full_command[i+1] == '<' || full_command[i+1] == '&')){
                     if (full_command[i+1] == '<')
                    {
                        stage = 2;
                    }
                    else if (full_command[i+1] == '>')
                    {
                        stage = 3;
                    }
                    else if (full_command[i+1] == '&')
                    {
                        stage = 4;
                    }
                    printf("Moving to stage %d: Initial Command: %s\n", stage, initialCommand);

                }
                else{
                    printf("Moving to stage 1: Initial Command: %s\n", initialCommand);
                    fflush(stdout);
                    strcpy(newCommand->command, initialCommand);
                    stage += 1;
                }
            }
        }

        else if (stage == 1)
        {
            printf("Stage 1\n");
            fflush(stdout);
            if (full_command[i] != ' ')
            {
               
                arguments[b] = full_command[i];
                b += 1;
                if (full_command[i + 1] == '\0')
                {

                    printf("Ending Search: Arguments Command 1: %s\n", arguments);
                    if (numArgs < 1)
                    {

                        Argument *temp = malloc(sizeof(Argument));
                        strcpy(temp->argument_text, arguments);
                        temp->next = NULL;

                        head = temp;
                    }
                    else
                    {
                        Argument *temp = malloc(sizeof(Argument));
                        strcpy(temp->argument_text, arguments);
                        tail->next = temp;
                       
                    }
                    newCommand->first = head;
                    printf("Is it print arguments\n");
                    printArguments(newCommand->first);
                    fflush(stdout);
                    // strcpy(newCommand -> arguments, arguments);
                    break;
                }
            }
            else
            {
                if (full_command[i + 1] == '\0')
                {
                    if (numArgs < 1)
                    {
                        Argument *temp = malloc(sizeof(Argument));
                        strcpy(temp->argument_text, arguments);
                        temp->next = NULL;
                        head = temp;
                    }
                    else
                    {
                        Argument *temp = malloc(sizeof(Argument));
                        strcpy(temp->argument_text, arguments);
                        tail->next = temp;
                       
                    }
                    newCommand->first = head;
                    printf("Ending Search: Arguments Command: %s\n", arguments);
                    printf("Is it print arguments\n");
                    printArguments(newCommand->first);
                    fflush(stdout);
                    // strcpy(newCommand -> arguments, arguments);
                    break;
                }
                else if (full_command[i + 1] == '>' || full_command[i + 1] == '<' || full_command[i + 1] == '&')
                {
                    printf("Continuing Search: Arguments Command: %s\n", arguments);
                    if (numArgs < 1)
                    {
                        Argument *temp = malloc(sizeof(Argument));
                        strcpy(temp->argument_text, arguments);
                        head = temp;
                    }
                    else
                    {
                        Argument *temp = malloc(sizeof(Argument));
                        strcpy(temp->argument_text, arguments);
                        tail->next = temp;
                        
                    }
                    newCommand->first = head;
                    printArguments(newCommand->first);
                    fflush(stdout);
                    // strcpy(newCommand -> arguments, arguments);
                    if (full_command[i + 1] == '<')
                    {
                        stage = 2;
                    }
                    else if (full_command[i + 1] == '>')
                    {
                        stage = 3;
                    }
                    else if (full_command[i + 1] == '&')
                    {
                        stage = 4;
                    }
                }
                else
                {
                    Argument *temp = malloc(sizeof(Argument));
                    strcpy(temp->argument_text, arguments);
                    b = 0;
                    for (int j = 0; arguments[j] != NULL; j++)
                    {
                        arguments[j] = '\0';
                    }
                    if (head == NULL)
                    {
                        head = temp;
                        tail = temp;
                    }
                    else
                    {
                        tail->next = temp;
                        tail = temp;
                    }
                    numArgs += 1;
                }
            }
        }
        else if (stage == 2)
        {
            printf("Stage 2\n");
            fflush(stdout);
            if (c == -2)
            {
                c = -1;
                continue;
            }
            if (c == -1)
            {
                c = 0;
                continue;
            }
            if (full_command[i] != ' ')
            {
                printf("Adding char %c to input\n", full_command[i]);
                input[c] = full_command[i];
                c += 1;
                if (full_command[i + 1] == '\0')
                {
                    printf("Ending Search: Input Command: %s\n", input);
                    strcpy(newCommand->input, input);
                    break;
                }
            }
            else
            {
                printf("Continuing Search: Input Command: %s\n", input);

                strcpy(newCommand->input, input);
                if (full_command[i + 1] == '>' || full_command[i + 1] == '&')
                {
                    if (full_command[i + 1] == '>')
                    {
                        printf("Moving to Stage 3: Input Command: %s\n", input);

                        stage = 3;
                    }
                    else if (full_command[i + 1] == '&')
                    {
                        printf("Moving to Stage 4: Input Command: %s\n", input);

                        stage = 4;
                    }
                }
            }
        }
        else if (stage == 3)
        {
            printf("Stage 3\n");
            fflush(stdout);
            if (d == -2)
            {
                d = -1;
                continue;
            }
            if (d == -1)
            {
                d = 0;
                continue;
            }
            if (full_command[i] != ' ')
            {
                output[d] = full_command[i];
                d += 1;
                if (full_command[i + 1] == '\0')
                {
                    printf("Ending Search: Output Command: %s\n", output);

                    strcpy(newCommand->output, output);
                    break;
                }
            }
            else
            {
                strcpy(newCommand->output, output);
                if (full_command[i + 1] == '<' || full_command[i + 1] == '&')
                {
                    if (full_command[i + 1] == '<')
                    {
                        printf("Moving to Stage 2: Output Command: %s\n", output);

                        stage = 2;
                    }
                    else if (full_command[i + 1] == '&')
                    {
                        printf("Moving to Stage 4: Output Command: %s\n", output);

                        stage = 4;
                    }
                }
            }
        }
        else if (stage == 4)
        {
            printf("Stage 4\n");
            fflush(stdout);
            if (full_command[i] == '&' && full_command[i + 1] == '\0')
            {
                printf("Background is true\n");
                newCommand->background = 1;
            }
            else
            {
                break;
            }
        }
    }
    printf("Loop exited\n");
    fflush(stdout);
    for(int n = 0; initialCommand[n] != NULL; n++){
        initialCommand[n] = '\0';
    }
    for(int m = 0; arguments[m] != NULL; m++){
        arguments[m] = '\0';
    }
    for(int z = 0; input[z] != NULL; z++){
        input[z] = '\0';
    }
    for(int x = 0; output[x] != NULL; x++){
        output[x] = '\0';
    }
}   

int execute(char **argv, Command *newCommand)
{
    int child_status = -5;
    int in_fd, out_fd;
    FILE *original_stdin, *original_stdout;
    
    // Save original stdin and stdout
    original_stdin = stdin;
    original_stdout = stdout;
    pid_t spawnPid = fork();
    if(spawnPid == -1){
        perror("Fork Failure\n");
        return EXIT_FAILURE;
    }
    else if(spawnPid == 0){
        //child process
         if(newCommand -> input[0] != '\0'){
            if(newCommand -> background == 1){
                in_fd = open("/dev/null", O_RDONLY);
            }
            else{
                in_fd = open(newCommand -> input, O_RDONLY);
            }

            if(in_fd == -1){
                perror("Error opening input file");
                return EXIT_FAILURE;
            }

            if(dup2(in_fd, fileno(original_stdin)) == -1){
                perror("Error redirecting input");
                return EXIT_FAILURE;
            }
            close(in_fd);
            /*
            in_fd = open(newCommand -> input, O_RDONLY);
            if (in_fd == -1) {
                perror("Error opening input");
                return EXIT_FAILURE;
            }
            if (dup2(in_fd, fileno(stdin)) == -1) {
                perror("Error redirecting stdin");
                return EXIT_FAILURE;
            }
            close(in_fd);
            */
            
        }
        if(newCommand -> output[0] != '\0'){
            if(newCommand -> background == 1){
                out_fd = open("/dev/null", O_WRONLY);
            }
            else{
                out_fd = open(newCommand -> output, O_WRONLY | O_CREAT | O_TRUNC, 0666);
            }

            if(out_fd == -1){
                perror("Error opening the output file");
                return EXIT_FAILURE;
            }

            if(dup2(out_fd, fileno(original_stdout)) == -1){
                perror("Error redirecting output");
                return EXIT_FAILURE;
            }
            close(out_fd);
            //printf("Redirecting Output");
            //fflush(stdout);
            /*
            out_fd = open(newCommand -> output, O_WRONLY | O_CREAT | O_TRUNC, 0666);
            if (out_fd == -1) {
                perror("Error opening output");
                return EXIT_FAILURE;
            }
            if (dup2(out_fd, fileno(stdout)) == -1) {
                perror("Error redirecting stdout");
                return EXIT_FAILURE;
            }
            close(out_fd);
            */
        }
        if(execvp(*argv, argv) == -1){
            perror("execvp failed");
            return EXIT_FAILURE;
        }
        
        if(newCommand -> input[0] != '\0'){
            if (dup2(fileno(original_stdin), fileno(stdin)) == -1) {
                perror("Error restoring stdin");
                return EXIT_FAILURE;
            }
             fclose(original_stdin);
        }
        if(newCommand -> output[0] != '\0'){
            if (dup2(fileno(original_stdout), fileno(stdout)) == -1) {
                perror("Error restoring stdout");
                return EXIT_FAILURE;
            }
            fclose(original_stdout);
        }
        

    }
    else{
        if(newCommand -> background == 1){
            printf("Running in background. PID: %d\n", spawnPid);
        }
        else{
            waitpid(spawnPid, &child_status, 0);
            if(WIFEXITED(child_status)){
                printf("Child process exited with status: %d\n", WEXITSTATUS(child_status));
            } else if (WIFSIGNALED(child_status)) {
                printf("Child process terminated by signal: %d\n", WTERMSIG(child_status));
            }
        }
    return child_status;   
    }
}

void runningLoop()
{

    int status = 0;

    while (1)
    {
        char full_command[2049];
        printf(": ");
        gets(full_command);
        fflush(stdout);
        
        Command *newCommand = malloc(sizeof(Command));
        strcpy(newCommand->fullCommand, full_command);

        if (checkComment(full_command) == 1)
        {
            continue;
        }

        if (full_command[0] == '\0')
        {
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
            while (full_command[i] != '\0')
            {
                dir[j] = full_command[i];
                j += 1;
                i += 1;
            }
            j += 1;
            dir[j] = '\0';
            printf("changing dir %d\n", chdir(dir));
            char cwd[PATH_MAX];
            if (getcwd(cwd, sizeof(cwd)) != NULL)
            {
                printf("Current working dir: %s\n", cwd);
            }
            else
            {
                perror("getcwd() error");
                return 1;
            }
        }
        default:
            break;
        }
        parseCommand(full_command, newCommand);
        int numArgs = countArgument(newCommand->first);
        char *args[numArgs + 2];
        for (int i = 1; i <= numArgs+1; i++)
        {
            if (i == numArgs+1)
            {
                args[i] = NULL;
                break;
            }
            else{
                args[i] = getArgumentAtIndex(newCommand->first, i-1)->argument_text;
                printf("ARGS at %d: %s\n", i, args[i]);
                fflush(stdout);
            }
        }
        args[0] = newCommand -> command;
        //strcpy(args[0], newCommand -> command);
        args[numArgs+1] = NULL;

        for(int j = 0; j <= numArgs + 1; j++){
            printf("Args: %s\n", args[j]);
        }
        //redirect input
         int in_fd, out_fd;
        FILE *original_stdin, *original_stdout;
    
        // Save original stdin and stdout
        original_stdin = stdin;
        original_stdout = stdout;

        
        
        newCommand -> exitStatus = execute(args, newCommand);
        status = newCommand -> exitStatus;
       
      
       
        
    }
}

int main(int argc, char *argv[])
{
    runningLoop();
}