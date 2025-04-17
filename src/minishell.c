#include <stdio.h>      // printf, perror, fgets
#include <stdlib.h>     // exit
#include <unistd.h>     // getcwd
#include <string.h>     // strerror, strcspn
#include <errno.h>      

#define MAX_PATH 4096
#define MAX_INPUT 4096

int main(void){
	char cwd[MAX_PATH];
	char input[MAX_PATH];

	while (1) {
        getcwd(cwd, sizeof(cwd));
        printf("[%s]$ ", cwd);
        fflush(stdout);

	if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

	input[strcspn(input, "\n")] = '\0';
    }
	return 0;

} 
