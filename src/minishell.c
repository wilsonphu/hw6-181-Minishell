#include <stdio.h>      // printf, perror, fgets
#include <stdlib.h>     // exit
#include <unistd.h>     // getcwd
#include <string.h>     // strerror, strcspn
#include <errno.h>     
#include <pwd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h> 

#define MAX_PATH 4096
#define MAX_INPUT 4096
#define MAX_TOKEN 2048
#define BRIGHTBLUE "\x1b[34;1m"
#define DEFAULT    "\x1b[0m"

volatile sig_atomic_t interrupted = 0;

void handle_sigint(int sig) {
    (void)sig;  
    interrupted = 1;
    write(STDOUT_FILENO, "\n", 1);
}

int main(void){

	struct sigaction sa;
	sa.sa_handler = handle_sigint;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGINT, &sa, NULL) == -1) { 
        	fprintf(stderr, "Error: Cannot register signal handler. %s.\n", strerror(errno));
        	exit(EXIT_FAILURE);
    	}

	char cwd[MAX_PATH];
	char input[MAX_PATH];

	while (1) {
		if (getcwd(cwd, sizeof(cwd)) == NULL) {
			fprintf(stderr,"Error: Cannot get current working directory. %s.\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
        	
        	//printf("[%s]$ ", cwd);
		printf("[%s%s%s]$ ", BRIGHTBLUE, cwd, DEFAULT);
        	fflush(stdout);
		
		// read input 
		if (fgets(input, sizeof(input), stdin) == NULL) {
			if (interrupted) {
            			interrupted = 0;
            			clearerr(stdin);  
            			continue;
        		}
			if (feof(stdin)){
				break;
			}
			fprintf(stderr, "Error: Failed to read from stdin. %s.\n", strerror(errno));
            		exit(EXIT_FAILURE);
        	}
		input[strcspn(input, "\n")] = '\0';

		// exit 
		if (strcmp(input, "exit") == 0) { 
            		break;
        	}	
		char *args[MAX_TOKEN];
              	int i = 0;
                char *token = strtok(input, " ");
                while (token != NULL && i < MAX_TOKEN-1) {
                        args[i++] = token;
                        token = strtok(NULL, " ");
                }
                args[i] = NULL;
		if (i == 0) continue;  
		// cd 
		if (args[0] && strcmp(args[0], "cd") == 0) {
			if (i > 2) {
        			fprintf(stderr, "Error: Too many arguments to cd.\n");
        			exit(EXIT_FAILURE);
    			}

		        struct passwd *pw = getpwuid(getuid());
                  	if (pw == NULL) {
                        	fprintf(stderr, "Error: Cannot get passwd entry. %s.\n", strerror(errno));
                          	exit(EXIT_FAILURE);
                  	}
	
            		const char *home = pw->pw_dir;
            		char path[MAX_PATH];

            		if (args[1] == NULL || strcmp(args[1], "~") == 0) {
                	// cd or cd ~
                		if (home && chdir(home)!= 0){ 
    					fprintf(stderr, "Error: Cannot change directory to '%s'. %s.\n", home, strerror(errno));
					exit(EXIT_FAILURE);
				}
            		}	 
			else if (args[1][0] == '~') {
				if (args[1][1]  == '/' || args[1][1]  == '\0'){
                			snprintf(path, sizeof(path), "%s%s", home, args[1] + 1);
				} else {
                			snprintf(path, sizeof(path), "%s/%s", home, args[1] + 1);
                		}

				if (chdir(path) != 0) {
            				fprintf(stderr, "Error: Cannot change directory to '%s'. %s.\n", path, strerror(errno));
					exit(EXIT_FAILURE);
        			}	
            		} else {
                		// cd somedir
                		if (chdir(args[1]) != 0) {
                    			fprintf(stderr, "Error: Cannot change directory to '%s'. %s.\n", args[1], strerror(errno));
					exit(EXIT_FAILURE);
                		}
            		}
			continue;
		}
		
		pid_t pid = fork();

		if (pid == 0){
			execvp(args[0], args);
			fprintf(stderr,"Error: exec() failed. %s.\n", strerror(errno));
			exit(EXIT_FAILURE);
		} else if (pid>0) {
			int status;
			if (waitpid(pid, &status, 0) == -1) {
				fprintf(stderr,"Error: wait() failed. %s.\n", strerror(errno));
			}	
		} else {
			fprintf(stderr,"Error: fork() failed. %s.\n",strerror(errno));
			
   		}		
	}	
	return EXIT_SUCCESS;

} 
