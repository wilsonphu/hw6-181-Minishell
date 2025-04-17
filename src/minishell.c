#include <stdio.h>      // printf, perror, fgets
#include <stdlib.h>     // exit
#include <unistd.h>     // getcwd
#include <string.h>     // strerror, strcspn
#include <errno.h>     
#include <pwd.h> 

#define MAX_PATH 4096
#define MAX_INPUT 4096
#define BRIGHTBLUE "\x1b[34;1m"
#define DEFAULT    "\x1b[0m"


int main(void){
	char cwd[MAX_PATH];
	char input[MAX_PATH];

	while (1) {
        	getcwd(cwd, sizeof(cwd));
        	//printf("[%s]$ ", cwd);
		printf("[%s%s%s]$ ", BRIGHTBLUE, cwd, DEFAULT);
        	fflush(stdout);
		
		// read input 
		if (fgets(input, sizeof(input), stdin) == NULL) {
            		break;
        	}
		input[strcspn(input, "\n")] = '\0'; 

		// exit 
		if (strcmp(input, "exit") == 0) { 
            		break;
        	}	

		// cd 
		
		if (strncmp(input, "cd", 2) == 0 && (input[2] == '\0' || input[2] == ' ')) {
			char *arg = input + 2;
            		while (*arg == ' '){ 
				arg++;
			}
		        struct passwd *pw = getpwuid(getuid());
                  	if (pw == NULL) {
                        	fprintf(stderr, "Error: Cannot get passwd entry. %s.\n", strerror(errno));
                          	continue;
                  	}	
            		const char *home = pw->pw_dir;
            		char path[MAX_PATH];

            		if (*arg == '\0' || strcmp(arg, "~") == 0) {
                	// cd or cd ~
                		if (home && chdir(home)!= 0){ 
					
    					fprintf(stderr, "Error: Cannot change directory to '%s'. %s.\n", home, strerror(errno));
    					continue;
		
				}
            		}	 
			else if (arg[0] == '~') {
				if (*(arg+1)  == '/' ||*(arg+1)  == '\0'){
                			snprintf(path, sizeof(path), "%s%s", home, arg + 1);
				} else {
                			snprintf(path, sizeof(path), "%s/%s", home, arg + 1);
                		}

				if (chdir(path) != 0) {
            				fprintf(stderr, "Error: Cannot change directory to '%s'. %s.\n", home, strerror(errno));
					continue;
        			}	
            		} else {
                		// cd somedir
                		if (chdir(arg) != 0) {
                    			fprintf(stderr, "Error: Cannot change directory to '%s'. %s.\n", home, strerror(errno));
					continue;
                		}
            		}
			char *args[2048];
			int i = 0;
			char *token = strtok(input, " ");
			while (token != NULL && i < 2047) {
            		args[i++] = token;
            		token = strtok(NULL, " ");
        	}	
        	args[i] = NULL;
		pid_t pid = fork();

		if (pid == 0){
			execvp(args[0], args);
			exit(EXIT_FAILURE);
		} else if (pid>0) {
			int status;
			waitpid (pid, &status, 0);
		} else {
			fprintf("Error: fork() failed. %s.\n", home, strerror(errno));
   		}
		continue;
	}	
	return EXIT_SUCCESS;

} 
