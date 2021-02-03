/**
 * Austin Van Braeckel - 1085829 - avanbrae@uoguelph.ca
 * 2021-01-30
 * CIS*3110 Assignment 1
 * Creating a custom shell.
 */

// SOURCES USED:
// - class CourseLink examples
// - man pages
// - http://www.csl.mtu.edu/cs4411.ck/www/NOTES/process/fork/exec.html
// - https://stackoverflow.com/questions/28502305/writing-a-simple-shell-in-c-using-fork-execvp
// - https://stackoverflow.com/questions/50280498/how-to-only-kill-the-child-process-in-the-foreground?fbclid=IwAR3pmr6csA0gT0yvCAnDYu0Q8XEoaVJwyaIOVegXKnE_zVu62X1aKlUYkfk
// - https://stackoverflow.com/questions/12663270/freopen-and-execvp-in-c
// - https://stackoverflow.com/questions/13801175/classic-c-using-pipes-in-execvp-function-stdin-and-stdout-redirection
// - https://www.geeksforgeeks.org/c-program-demonstrate-fork-and-pipe/
// - https://www.geeksforgeeks.org/making-linux-shell-c/
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

#define BUFFER_LEN 1000
#define MAX_PROCESSES 250
#define ARGS_NUM 50
#define PATH_LEN 1000

static int process_count;
static pid_t processes[250];
static char line_cpy[250][BUFFER_LEN];

// Handler for when a process is run in the background using '&'
void signal_child_handler(int signalPassed) {
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
    char** parsed_pipe = malloc(sizeof(char*) * ARGS_NUM);
    char** parsed_pre_pipe = malloc(sizeof(char*) * ARGS_NUM);
    int i;
    char line[BUFFER_LEN];
    int status = -1;
    struct sigaction sig_act;
    sig_act.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sig_act.sa_handler = SIG_IGN;
    memset(&sig_act, 0, sizeof(struct sigaction));
    bool background = false;
    process_count = 0;
    bool write_out = false;
    bool read_in = false;
    bool piping = false;
    int pipe_fd[2]; // stores two ends of the pipe

    while (running) {
        printf("> ");

        write_out = false;
        read_in = false;

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

        // check for redirects to files (reading/writing), and piping 
        if (argc >= 3) {
            if (strcmp(argv[argc - 2], "<") == 0) {
                read_in = true;
            } else if (strcmp(argv[argc - 2], ">") == 0) {
                write_out = true;
            } else { // check if piping
                for (i = 0; i < argc; i++) {
                    if (strcmp(argv[i], "|") == 0) {
                        piping = true;
                    }
                }
            }
        }

        if (piping) {
            // set-up pipes
            if (pipe(pipe_fd) == -1) { 
                perror("Pipe Failed"); 
                exit(EXIT_FAILURE);
            }

            // separate the commands before and after the '|' in argv
            int j = 0;
            bool passed_pipe_char =  false;
            for (i = 0; i < argc; i++) {
                if (strcmp(argv[i], "|") == 0) {
                    passed_pipe_char = true;
                    parsed_pre_pipe[j + 1] = NULL;
                    j = -1; // -1 so that it is = 0 in the next iteration
                } else if (passed_pipe_char) {
                    parsed_pipe[j] = malloc(strlen(argv[i]) + 1);
                    strcpy(parsed_pipe[j], argv[i]);
                } else {
                    parsed_pre_pipe[j] = malloc(strlen(argv[i]) + 1);
                    strcpy(parsed_pre_pipe[j], argv[i]);
                }
                j++;
            }
        }

        // reset signal action
        sig_act.sa_flags = SA_RESTART | SA_NOCLDSTOP;
        sig_act.sa_handler = SIG_IGN;                
        sigaction(SIGCHLD, &sig_act, NULL);

        pid_t child_pid = fork(); // fork child

        // Check to see if process should be run in the background
        if (argc >= 1 && argv[argc - 1] != NULL && strcmp(argv[argc - 1], "&") == 0) {
            background = true;
            argv[argc - 1] = NULL; // remove the amperstand
            argc--;
            sig_act.sa_flags = SA_RESTART | SA_NOCLDSTOP;
            sig_act.sa_handler = SIG_IGN; // ignore
            sigemptyset(&sig_act.sa_mask);
            if (sigaction(SIGCHLD, &sig_act, NULL) < 0) {
                perror("sigaction()");
                exit(1);
            }
        } else {
            background = false;
        }

        if (child_pid >= 0) { // fork succeeded
            if( child_pid == 0) { // Child
                
                if(write_out) {
                    // set the file as the output
                    freopen(argv[argc - 1], "w+", stdout);
                    // remove the redirect and file name from the args
                    argv[argc - 2] = NULL;
                    argv[argc - 1] = NULL;
                    argc -= 2;
                    status = execvp(argv[0], argv);
                    if(status == -1) { // catch execvp error
                        fprintf(stderr, "-myShell: %s: command not found\n", line);
                        perror("command not found");
                        exit(EXIT_FAILURE);
                    }

                } else if(read_in) {
                    // set the file as the input
                    freopen(argv[argc - 1], "r", stdin);
                    // remove the redirect from the args ('<' or '>'), but leave file name
                    argv[argc - 2] = argv[argc - 1];
                    argv[argc - 1] = NULL;
                    argc--;
                    status = execvp(argv[0], argv);
                    if(status == -1) { // catch execvp error
                        fprintf(stderr, "-myShell: %s: command not found\n", line);
                        perror("command not found");
                        
                    }

                } else if(piping) {
                    close(pipe_fd[1]); // closing pipe write
                    dup2(pipe_fd[0], STDIN_FILENO); // replacing stdin with pipe read
                    status = execvp(parsed_pipe[0], parsed_pipe);
                    if(status == -1) { // catch execvp error
                        fprintf(stderr, "-myShell: %s: command not found\n", line);
                        perror("command not found");
                        exit(EXIT_FAILURE);
                    }
                    piping = false; //done piping so reset to default
                } else {
                    status = execvp(argv[0], argv);
                    if(status == -1) { // catch execvp error
                        fprintf(stderr, "-myShell: %s: command not found\n", line);
                        perror("command not found");
                        exit(EXIT_FAILURE);
                    }
                }
//FIX THIS SECTION BELOW --------------------------------------------------------------------------------------
            } else { // Parent
                if (piping) {
                    close(pipe_fd[0]); // closing pipe read
                    dup2(pipe_fd[1], STDOUT_FILENO); // replacing stdout with pipe write
                    status = execvp(parsed_pre_pipe[0], parsed_pre_pipe);
                    if(status == -1) { // catch execvp error
                        fprintf(stderr, "-myShell: %s: command not found\n", line);
                        perror("command not found");
                        exit(EXIT_FAILURE);
                    }
                } else if (!background) { // NOT running in the background
                    waitpid(child_pid, NULL, 0);
                    // check to see if anything running in the background finished
                    sig_act.sa_flags = SA_RESTART | SA_NOCLDSTOP;
                    sig_act.sa_handler = &signal_child_handler;
                    sigaction(SIGCHLD, &sig_act, NULL);
                } else { // Running in the background
                    sig_act.sa_flags = SA_RESTART | SA_NOCLDSTOP;
                    sig_act.sa_handler = &signal_child_handler;
                    sigaction(SIGCHLD, &sig_act, NULL);
                    processes[process_count] = child_pid;
                    process_count++;
                    printf("[%d] %d\n", process_count, child_pid);
                }

            }
        } else {
            perror("fork failed");
            exit(EXIT_FAILURE);
        }

    } // end of running (infinite) while loop

}

// FIX REDIRECT SO IT CAN DO MULTIPLE (______ < ________ > ______)