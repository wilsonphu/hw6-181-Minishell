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
                        	perror("getpwuid");
                          	continue;
                  	}	
            		const char *home = pw->pw_dir;
            		char path[MAX_PATH];

            		if (*arg == '\0' || strcmp(arg, "~") == 0) {
                	// cd or cd ~
                		if (home && chdir(home)!= 0){ 
					perror("cd");
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
            				perror("cd");
					continue;
        			}	
            		} else {
                		// cd somedir
                		if (chdir(arg) != 0) {
                    			perror("cd");	
					continue;
                		}
            		}
   		}
		continue;
	}	
	return EXIT_SUCCESS;

} 
