#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

char *paths = "/bin";

char **splitString(const char *input, int *numWords, char *path)
{
    if (input == NULL)
    {
        char **wordsArray = malloc(sizeof(path));
        wordsArray[0] = path;
        return wordsArray;
    };
    int count = 0;
    const char *temp = input;
    while (*temp != '\0')
    {
        while (*temp == ' ' && *temp != '\0')
        {
            temp++;
        }
        if (*temp != '\0')
        {
            count++;
            while (*temp != ' ' && *temp != '\0')
            {
                temp++;
            }
        }
    }

    *numWords = count;

    // Allocate memory for an array of strings
    char **wordsArray = (char **)malloc(count * sizeof(char *) + sizeof(path));
    if (wordsArray == NULL)
    {
        printf("Memory allocation error!\n");
        exit(EXIT_FAILURE);
    }

    int wordIndex = 1;
    wordsArray[0] = path;
    char *token = strtok((char *)input, " ");
    while (token != NULL)
    {
        wordsArray[wordIndex] = strdup(token);
        if (wordsArray[wordIndex] == NULL)
        {
            printf("Memory allocation error!\n");
            exit(EXIT_FAILURE);
        }
        wordIndex++;
        token = strtok(NULL, " ");
    }

    return wordsArray;
}

void sigintHandler(int sig)
{
    fflush(stdout);
}

int handle_builtins(const char *inp, const char *args)
{
    return 0;
}

int main(int argc, char *argv[], char *envp[])
{

    signal(SIGINT, sigintHandler);

    char *inp_buffer = NULL;
    size_t size = 0;

    while (1)
    {
        printf("dash> ");
        getline(&inp_buffer, &size, stdin);
        inp_buffer = strsep(&inp_buffer, "\n"); // removes \n

        //
        // Sets cmd to the first continous string in inp_buf before a space char and sets args to the rest
        // eg:-
        // inp_buf = "Hey lol wow" then the cmd and args will be the following
        // cmd = "Hey"
        // args = "lol wow"
        //
        char *cmd = strtok(inp_buffer, " ");
        char *args = strtok(NULL, "\0"); // Rest of the string as arguments

        if (cmd == NULL)
        {
            continue;
        }

        if (!strcmp(cmd, "exit"))
            exit(0);

        if (handle_builtins(cmd, args))
            continue;

        // Loops through all paths to see if the command file exist and is executable
        int found = 0;
        char *tmp_paths = paths;
        char *tmp_buf = strsep(&tmp_paths, " ");
        while (tmp_buf != NULL)
        {
            char *path = malloc(strlen(tmp_buf) + strlen("/") + strlen(cmd));
            if (path == NULL)
                continue;
            strcpy(path, tmp_buf);
            strcat(path, "/");
            strcat(path, cmd);
            if (!access(path, X_OK))
            {
                found = 1;
                int c_stat = 0;
                pid_t c_pid = fork();
                if (c_pid == 0)
                {
                    int numWords = 0;
                    char **argv_c = splitString(args, &numWords, path);
                    if (argv_c == NULL)
                        argv_c = argv;
                    execve(path, argv_c, envp);
                }
                else
                {
                    waitpid(c_pid, &c_stat, 0);
                }
                free(path);
                break;
            }
            free(path);
            tmp_buf = strsep(&tmp_paths, " ");
        }
        if (!found)
            printf("Command %s not found\n", cmd);
    }

    return 0;
}