#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main(){
	int fd1[2], fd2[2];
	pid_t pid1, pid2, pid3;
	
	if(pipe(fd1) == -1){
		perror("pipe");
	}
	
	if(pipe(fd2) == -1){
		perror("pipe");
	}
	
	pid1 = fork();
	
	if(pid1 == -1){
		perror("fork");
	}
	
	if(pid1 == 0){
		printf("jestem w potomnym o pid: %d", pid1);
		close(fd1[0]);
		
		if(fd1[1] != STDOUT_FILENO){
			dup2(fd1[1], 1);
			close(fd1[1]);
		}
		
		execlp("ls", "ls", "-l", (char*)NULL);
	}
	
	pid2 = fork();
	
	if(pid2 == -1){
		perror("fork");
	}
	
	if(pid2 == 0){
		printf("jestem w potomnym o pid: %d", pid2);
		close(fd1[1]);
		
		close(fd2[0]);
		
		if(fd1[0] != STDIN_FILENO){
			dup2(fd1[0], 0);
			close(fd1[0]);
		}
		
		if(fd2[1] != STDOUT_FILENO){
			dup2(fd2[1], 1);
			close(fd2[1]);
		}
		
		execlp("grep", "grep", "^d", (char*)NULL);
	}
	
	pid3 = fork();
	
	if(pid3 == -1){
		perror("fork");
	}
	
	if(pid3 == 0){
		printf("jestem w potomnym o pid: %d", pid3);
		close(fd2[1]);
		
		if(fd2[0] != STDIN_FILENO){
			dup2(fd2[0], 0);
			close(fd2[0]);
			close(fd1[0]);
			close(fd1[1]);
		}
		
		execlp("wc", "wc", "-l", (char*)NULL);
	}
	
	
	close(fd1[0]);
	close(fd1[1]);
	close(fd2[0]);
	close(fd2[1]);
	
	wait(NULL);
	wait(NULL);
	wait(NULL);
	
	exit(EXIT_SUCCESS);
	return 0;
}
