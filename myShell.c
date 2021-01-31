/**
 * Austin Van Braeckel - 1085829 - avanbrae@uoguelph.ca
 * 2021-01-30
 * CIS*3110 Assignment 1
 * Creating a custom shell.
 */

// SOURCES USED:
// - class courselink examples
// - man pages
// - http://www.csl.mtu.edu/cs4411.ck/www/NOTES/process/fork/exec.html
// - https://stackoverflow.com/questions/28502305/writing-a-simple-shell-in-c-using-fork-execvp
// - https://stackoverflow.com/questions/50280498/how-to-only-kill-the-child-process-in-the-foreground?fbclid=IwAR3pmr6csA0gT0yvCAnDYu0Q8XEoaVJwyaIOVegXKnE_zVu62X1aKlUYkfk
// - 

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

#define BUFFER_LEN 500
#define MAX_PROCESSES 250
#define ARGS_NUM 50
#define PATH_LEN 100

static int process_count;
static pid_t processes[250];
static char line_cpy[250][BUFFER_LEN];

// Handler for when a process is run in the background using '&'
void signalChildHandler(int signalPassed) {
    int *tempInt;
    int i = 0;
    pid_t temp = wait(NULL);

    if (temp != -1) {
        for (i = 0; i < process_count; i++) {
            if (processes[i] == temp) {
                processes[i] = -1;
                break;
            }
        }
        printf("[%d]+ Done\t\t%s\n", i + 1, line_cpy[i]);
        process_count--;
    }
    // if system call interrupted, reset 
    if(errno == EINTR) {
        errno = 0;
    }
}

int main () {

    bool running = true;
    int argc;
    char* argv[ARGS_NUM];
    int i;
    char line[BUFFER_LEN];
    int status = -1;
    struct sigaction sigact;
    sigact.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigact.sa_handler = SIG_IGN;
    memset(&sigact, 0, sizeof(struct sigaction));
    bool background = false;
    process_count = 0;

    while (running) {
        printf("> ");

        if(!fgets(line, BUFFER_LEN, stdin)) break; //stops if user presses CTRL+D
        // remove \n character at the end of the line
        int len = strlen(line);
        if (line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }

        // copy/save command for later use
        strcpy(line_cpy[process_count], line);

        char *token; //split command into separate strings
        token = strtok(line, " ");
        i = 0;
        while(token != NULL){
            argv[i] = token;      
            token = strtok(NULL, " ");
            i++;     
        }         
        argv[i] = NULL; // ensure last value is NULL
        argc = i;

        // EXIT
        if (argv[0] != NULL && strcmp(argv[0], "exit") == 0) {
            printf("myShell terminating...\n");
            printf("\n[Process completed]\n");
            exit(EXIT_SUCCESS);
        }  

        pid_t child_pid = fork(); // fork child

        // Check to see if process should be run in the background
        if (argc >= 1 && argv[argc - 1] != NULL && strcmp(argv[argc - 1], "&") == 0) {
            background = true;
            argv[argc - 1] = NULL; // remove the amperstand
            argc--;
            sigact.sa_flags = SA_RESTART | SA_NOCLDSTOP;
            sigact.sa_handler = SIG_IGN; // ignore
            sigemptyset(&sigact.sa_mask);
            if (sigaction(SIGCHLD, &sigact, NULL) < 0) {
                perror("sigaction()");
                exit(1);
            }
        } else {
            background = false;
        }

        if (child_pid >= 0) { // fork succeeded
            if(child_pid == 0){ // Child

                status = execvp(argv[0], argv);
                if(status == -1) { // catch execvp error
                    fprintf(stderr, "-myShell: %s: command not found\n", line);
                    perror("");
                    exit(EXIT_FAILURE);
                }

            }else{ // Parent

                if (!background) { // NOT running in the background
                    waitpid(child_pid, NULL, 0);
                } else { // Running in the background
                    sigact.sa_flags = SA_RESTART | SA_NOCLDSTOP;
                    sigact.sa_handler = &signalChildHandler;
                    sigaction(SIGCHLD, &sigact, NULL);
                    processes[process_count] = child_pid;
                    process_count = process_count + 1;
                    printf("[%d] %d\n", process_count, child_pid);
                }

            }
        } else {
            perror("fork failed");
            exit(EXIT_FAILURE);
        }

    } // end of running while loop

}

