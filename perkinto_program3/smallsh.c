#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>

// flags for signals
int turnOffBackground = 0;
int offFlag = 0;
int onFlag = 0;

// global variables for background process arrays
pid_t backgroundProccessStatus[100];
int backgroundProccessPid[100];
int numberBackground = 0;

// struct (linked list)for arguments to store multiple
struct argument
{
    char argument_text[2049];
    struct argument *next;
};
typedef struct argument Argument;

// struct containing all elements of a command
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

// counts the arguments in a command
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

// returns a specific argument at an index
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

// prints all arguments (testing only)
void printArguments(Argument *first)
{
    Argument *temp = first;
    while (temp != NULL)
    {
        printf("%s\n", temp->argument_text);
        temp = temp->next;
    }
}

// checks if the value entered is a comment
int checkComment(char command[2049])
{

    if (command[0] == '#')
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

// checks if the command entered is a built in command
int checkBuiltInCommands(char command[2049])
{
    char exit_command[] = "exit";
    char cd[] = "cd";
    char status[] = "status";

    if (strncmp(command, exit_command, 4) == 0)
    {
        // check if it is exit
        return 0;
    }
    else if (strncmp(command, status, 6) == 0)
    {
        // check if it is status
        return 1;
    }

    else if (command[0] == cd[0] && command[1] == cd[1])
    {
        // check for cd
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

// parses a command given by the user into inital command, arguments, input, output, and background
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

    // loop through every character of the command
    for (int i = 0; i <= strlen(full_command); i++)
    {

        // stage 0 is looking for the initial command
        if (stage == 0)
        {
            // it will be the full command until a space is found
            if (full_command[i] != ' ')
            {
                initialCommand[a] = full_command[i];
                a = a + 1;
                if (full_command[i + 1] == '\0')
                {
                    strcpy(newCommand->command, initialCommand);
                    break;
                }
            }
            else
            {
                // space is found -> save the initial command
                strcpy(newCommand->command, initialCommand);

                // check to see what stage to go to next
                if ((full_command[i + 1] == '>' || full_command[i + 1] == '<' || full_command[i + 1] == '&'))
                {
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
                    strcpy(newCommand->command, initialCommand);
                    stage += 1;
                }
            }
        }
        // stage 1 is looking for the arguments
        else if (stage == 1)
        {
            // the delimiter will be spaces
            if (full_command[i] != ' ')
            {

                arguments[b] = full_command[i];
                b += 1;
                if (full_command[i + 1] == '\0')
                {
                    // if we reach the end of the file, save the arguments
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

                    break;
                }
            }
            else
            {
                // if we find a space, find out if the next character is the end of the file, another argument, an input, an output, or the background signal
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

                    break;
                }
                else if (full_command[i + 1] == '>' || full_command[i + 1] == '<' || full_command[i + 1] == '&')
                {
                    // move to next stage if we find the signifier
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
                    // move to the next argument, perserving the linked list
                    Argument *temp = malloc(sizeof(Argument));
                    strcpy(temp->argument_text, arguments);
                    b = 0;
                    // empty the arguments character array
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
        // stage 2 is looking for inputs
        else if (stage == 2)
        {
            // skip the first two characters because they will be < and a space
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
                // add to input
                input[c] = full_command[i];
                c += 1;
                if (full_command[i + 1] == '\0')
                {
                    strcpy(newCommand->input, input);
                    break;
                }
            }
            else
            {
                // if a space is found move to a different stage
                strcpy(newCommand->input, input);
                if (full_command[i + 1] == '>' || full_command[i + 1] == '&')
                {
                    if (full_command[i + 1] == '>')
                    {

                        stage = 3;
                    }
                    else if (full_command[i + 1] == '&')
                    {

                        stage = 4;
                    }
                }
            }
        }
        // stage 3 is looking for an output, almost exactly the same as stage 2
        else if (stage == 3)
        {

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

                        stage = 2;
                    }
                    else if (full_command[i + 1] == '&')
                    {

                        stage = 4;
                    }
                }
            }
        }
        // stage 4 is checking for the background symbol
        else if (stage == 4)
        {

            if (full_command[i] == '&' && full_command[i + 1] == '\0')
            {

                newCommand->background = 1;
            }
            else
            {
                break;
            }
        }
    }
    // reset everything
    for (int n = 0; initialCommand[n] != NULL; n++)
    {
        initialCommand[n] = '\0';
    }
    for (int m = 0; arguments[m] != NULL; m++)
    {
        arguments[m] = '\0';
    }
    for (int z = 0; input[z] != NULL; z++)
    {
        input[z] = '\0';
    }
    for (int x = 0; output[x] != NULL; x++)
    {
        output[x] = '\0';
    }
}
// handler for the ^C event on a foreground child process
void chandler()
{
    exit(2);
}

// executes a program given by the user
int execute(char **argv, Command *newCommand)
{
    int child_status = -5;
    int in_fd, out_fd;
    FILE *original_stdin, *original_stdout;

    // Save original stdin and stdout
    original_stdin = stdin;
    original_stdout = stdout;
    pid_t spawnPid = fork();

    //fork failed !
    if (spawnPid == -1)
    {
        perror("Fork Failure\n");
        return EXIT_FAILURE;
    }
    else if (spawnPid == 0)
    {
        // child process
        // set the signal handler for a foreground process
        if (newCommand->background != 1)
        {
            signal(SIGINT, chandler);
        }
        //set signal to ignore ^C
        signal(SIGTSTP, SIG_IGN);

        if (newCommand->input[0] != '\0')
        {
            // reassign input if required
            in_fd = open(newCommand->input, O_RDONLY);

            if (in_fd == -1)
            {
                printf("Error opening input file");
                fflush(stdout);
                return EXIT_FAILURE;
            }

            if (dup2(in_fd, fileno(original_stdin)) == -1)
            {
                perror("Error redirecting input");
                return EXIT_FAILURE;
            }
            close(in_fd);
        }
        else if (turnOffBackground == 0 && newCommand->background == 1)
        {
            // reassign input for backgound
            in_fd = open("/dev/null", O_RDONLY);
            if (in_fd == -1)
            {
                printf("Error opening input file");
                fflush(stdout);
                return EXIT_FAILURE;
            }

            if (dup2(in_fd, fileno(original_stdin)) == -1)
            {
                perror("Error redirecting input");
                return EXIT_FAILURE;
            }
            close(in_fd);
        }

        if (newCommand->output[0] != '\0')
        {
            // reassign output if required
            out_fd = open(newCommand->output, O_WRONLY | O_CREAT | O_TRUNC, 0666);

            if (out_fd == -1)
            {
                printf("Error opening the output file");
                fflush(stdout);
                return EXIT_FAILURE;
            }

            if (dup2(out_fd, fileno(original_stdout)) == -1)
            {
                printf("Error redirecting output");
                fflush(stdout);
                return EXIT_FAILURE;
            }
            close(out_fd);
        }
        else if (turnOffBackground == 0 && newCommand->background == 1)
        {
            // reassign output for background
            out_fd = open("/dev/null", O_WRONLY);
            if (in_fd == -1)
            {
                printf("Error opening output file");
                fflush(stdout);

                return EXIT_FAILURE;
            }

            if (dup2(in_fd, fileno(original_stdin)) == -1)
            {
                printf("Error redirecting input");
                fflush(stdout);
                return EXIT_FAILURE;
            }
            close(in_fd);
        }
        // run command
        if (execvp(*argv, argv) == -1)
        {
            // check for error
            perror("execvp failed");

            // close input and output
            if (newCommand->input[0] != '\0')
            {
                if (dup2(fileno(original_stdin), fileno(stdin)) == -1)
                {
                    printf("Error restoring stdin");
                    fflush(stdout);
                    return EXIT_FAILURE;
                }
                fclose(original_stdin);
            }
            if (newCommand->output[0] != '\0')
            {
                if (dup2(fileno(original_stdout), fileno(stdout)) == -1)
                {
                    printf("Error restoring stdout");
                    fflush(stdout);
                    return EXIT_FAILURE;
                }
                fclose(original_stdout);
            }
            // exit the child process returning the failure
            exit(EXIT_FAILURE);
        }
        // close input and output
        if (newCommand->input[0] != '\0')
        {
            if (dup2(fileno(original_stdin), fileno(stdin)) == -1)
            {
                printf("Error restoring stdin");
                fflush(stdout);

                return EXIT_FAILURE;
            }
            fclose(original_stdin);
        }
        if (newCommand->output[0] != '\0')
        {
            if (dup2(fileno(original_stdout), fileno(stdout)) == -1)
            {
                printf("Error restoring stdout");
                fflush(stdout);

                return EXIT_FAILURE;
            }
            fclose(original_stdout);
        }
    }
    else
    {
        // check if running in background
        if (newCommand->background == 1 && turnOffBackground == 0)
        {

            signal(SIGINT, SIG_IGN);

            printf("Running in background. PID: %d\n", spawnPid);
            fflush(stdout);

            // add to array
            backgroundProccessPid[numberBackground] = spawnPid;
            backgroundProccessStatus[numberBackground] = child_status;
            numberBackground += 1;
        }
        else
        {
            // if not wait for response
            waitpid(spawnPid, &child_status, 0);
        }
        // check status of all background processes running
        int childStatus;
        for (int i = 0; i < numberBackground; i++)
        {

            int result = waitpid(backgroundProccessPid[i], &childStatus, WNOHANG);
            if (result != 0)
            {
                if (WIFSIGNALED(childStatus))
                {
                    printf("Background process %d terminated by signal %d\n", backgroundProccessPid[i], WTERMSIG(childStatus));
                    fflush(stdout);
                    // remove process from array
                    backgroundProccessPid[i] = backgroundProccessPid[i + 1];
                    backgroundProccessStatus[i] = backgroundProccessStatus[i + 1];
                    i -= 1;
                    numberBackground -= 1;
                }
                else if (WIFEXITED(childStatus))
                {
                    printf("Child %d exited normally with status %d\n", backgroundProccessPid[i], WEXITSTATUS(childStatus));
                    fflush(stdout);
                    // remove process from array
                    backgroundProccessPid[i] = backgroundProccessPid[i + 1];
                    backgroundProccessStatus[i] = backgroundProccessStatus[i + 1];
                    i -= 1;
                    numberBackground -= 1;
                }
            }
        }
        return child_status;
    }
}

// expands $$ into processId
void expandVariables(Command *newCommand, char *str, int processId)
{

    char pidStr[20];
    sprintf(pidStr, "%d", getpid());

    
    char oldW[] = "$$";
    char *pos, temp[1000];
    int index = 0;
    int owlen;

    owlen = strlen(oldW);

    // repeat until all occurences of $$ are found

    while ((pos = strstr(str, oldW)) != NULL)
    {
        strcpy(temp, str);

        // index of current found word
        index = pos - str;

        // terminate str after word found index
        str[index] = '\0';

        // concatenate str with new word
        strcat(str, pidStr);

        // concatenate str with remaining words after
        strcat(str, temp + index + owlen);
    }

    // copy into fullCommand
    strcpy(newCommand->fullCommand, str);
}

// handler for ^Z
void zhandler()
{
    if (turnOffBackground == 0)
    {
        offFlag = 1;
        turnOffBackground = 1;
    }
    else
    {
        onFlag = 1;
        turnOffBackground = 0;
    }
}

// running loop for the shell
void runningLoop()
{
    // save process id
    int processId = getpid();
    int status = 0;

    // set signal handlers
    signal(SIGINT, SIG_IGN);
    signal(SIGTSTP, zhandler);
    while (1)
    {
        // collect user input
        char full_command[2049];
        printf(": ");

        // check flags
        if (onFlag == 1)
        {
            printf("Now Turning Off Background Process Functionality\n");
            fflush(stdout);
            onFlag = 0;
            continue;
        }
        if (offFlag == 1)
        {
            printf("Now Returning Background Process Functionality\n");
            fflush(stdout);
            offFlag = 0;
            continue;
        }

        // collect user input
        gets(full_command);
        fflush(stdout);

        // save command
        Command *newCommand = malloc(sizeof(Command));
        strcpy(newCommand->fullCommand, full_command);

        // check for comment
        if (checkComment(newCommand->fullCommand) == 1)
        {
            continue;
        }

        if (full_command[0] == '\0')
        {
            continue;
        }

        // expand the variable
        expandVariables(newCommand, full_command, processId);

        // check for built ins
        int builtIns = checkBuiltInCommands(full_command);
        switch (builtIns)
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
            chdir(getenv("HOME"));
            break;
        case 3:
        {
            char dir[2049];
            int i = 3;
            int j = 0;

            while (full_command[i - 1] != '\0')
            {
                dir[j] = full_command[i];
                j += 1;
                i += 1;
            }
            j += 1;
            dir[j] = '\0';

            chdir(dir);
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
        // go to next command if a built in is found
        if (builtIns == 0 || builtIns == 1 || builtIns == 2 || builtIns == 3)
        {
            continue;
        }

        // check flags again
        if (onFlag == 1)
        {
            printf("Now Turning Off Background Process Functionality\n");
            fflush(stdout);
            onFlag = 0;
            continue;
        }
        if (offFlag == 1)
        {
            printf("Now Returning Background Process Functionality\n");
            fflush(stdout);
            offFlag = 0;
            continue;
        }
        // parse the command
        parseCommand(full_command, newCommand);

        // count args and make an array to pass to exec family
        int numArgs = countArgument(newCommand->first);
        char *args[numArgs + 2];
        for (int i = 1; i <= numArgs + 1; i++)
        {
            if (i == numArgs + 1)
            {
                args[i] = NULL;
                break;
            }
            else
            {
                args[i] = getArgumentAtIndex(newCommand->first, i - 1)->argument_text;
            }
        }
        args[0] = newCommand->command;
        args[numArgs + 1] = NULL;

        // execute command
        newCommand->exitStatus = execute(args, newCommand);
        status = newCommand->exitStatus;

        // check flags
        if (onFlag == 1)
        {
            printf("Now Turning Off Background Process Functionality\n");
            fflush(stdout);
            onFlag = 0;
            continue;
        }
        if (offFlag == 1)
        {
            printf("Now Returning Background Process Functionality\n");
            fflush(stdout);
            offFlag = 0;
            continue;
        }

        fflush(stdout);
    }
}

int main(int argc, char *argv[])
{
    runningLoop();
}