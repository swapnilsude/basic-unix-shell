#include<stdio.h> 
#include<string.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<sys/wait.h> 
#include<readline/readline.h> 
#include<readline/history.h> 

/* Parsing given input to check if pipe exists */
int parsePipe(char* line, char *linepipe[2]){
	for (int i = 0; i < 2; i++) { 
		linepipe[i] = strsep(&line, "|"); 
		if (linepipe[i] == NULL) 
			break; 
	}
}

/* Parsing given input to check if semicolon (;) exists */
void parseSemic(char* line, char** argvs){ 
	for (int i = 0; i < 64; i++) {
		argvs[i] = strsep(&line, ";"); 
		if (argvs[i] == NULL) 
			break; 
		if (strlen(argvs[i]) == 0) 
			i--; 
	}
}

/* Parsing given input to check if blank (;) exists */
void parseBlank(char* lines, char** argv) 
{ 
	for (int i = 0; i < 64; i++) { 
		argv[i] = strsep(&lines, " "); 
		if (argv[i] == NULL) 
			break; 
		if (strlen(argv[i]) == 0) 
			i--; 
	}
}

/* Execute the non pipe cmd 
Checks if there is & trailing for background program
Replacing & with \0 (NULL) 
If the & background exists don't wait for the chid to finish
If not background wait for child to finish*/
void  execute(char **argv)
{
	pid_t  pid;
	int background=0;
	for (int i=0; i<64 ; i++){
		if (argv[i]==NULL){
			if(argv[i-1][strlen(argv[i-1])-1] == '&')
        	{
            	background = 1;      // if & exixts
            	argv[i-1][strlen(argv[i-1])-1] = '\0';
        	}
			break;
		}
	}

	pid = fork();                      // Fork new process
	if(pid == 0 && background == 1)    // Child process with background
	{
		setpgid(pid, 0);               // creates a new process group leader pid
		execvp(*argv, argv);
		printf("Command not found\n");
		exit(1);
	}
	else if(pid == 0 && background != 1)    // child process without background
	{
		execvp(*argv, argv);
        printf("Command not found\n");
		exit(1);
	}
	else if(background != 1)   //wait for child to finish if not a background process
	{
		wait(NULL);
	}
}

/* Execute the pipe cmd
Creates two child process and uses dup2 to send STDOUT_FILENO from 
cmd 1 to STDIN_FILENO of cmd 2 */
void executePipe(char **argv, char **argv2){
	int pipefd[2];
	pid_t pid;
	pipe(pipefd);

	if (pipe(pipefd) < 0)    {perror("Pipe could not be initialized\n"); exit(1);}
    
    if(fork() == 0){            // child 1
		close(pipefd[0]);       // close read end of pipe for the write operation
		dup2(pipefd[1], 1);     // 1 - STDOUT_FILENO
		close(pipefd[1]);
		execvp(*argv, argv);
		perror("Command not found\n");
		exit(1);
	}
	if(fork() == 0){            //child 2
		close(pipefd[1]);       // close write end of pipe for read operation
		dup2(pipefd[0], 0);     // 0 - STDIN_FILENO
		close(pipefd[0]);
		execvp(*argv2, argv2);
		perror("Command not found\n");
		exit(1);
    }
    close(pipefd[0]);           //close pipe
    close(pipefd[1]);
    wait(NULL);                 //waiting for both childen process to end
    wait(NULL);
}

/////// MAIN ///////
void  main(void)
{
	char *line;
    char *lines;
	char *argv[64];
	char *argv2[64];
	char *argvs[64];
	char *linepiped[2];
	int parsecheck;
     
	while (1) { 
		line = readline("ENTS669> ");   // Take input 
        add_history(line);              // for using cursor for previous cmd
		parsePipe(line,linepiped);      // function to split pipe cmd
		if (linepiped[1] == NULL){      // if pipe is not present
			parseSemic(line,argvs);     // function to split cmd at ;
            /* Loops through the multiple cmds splitted by ; */
            for(int i=0; i<64 ; i++){   
                if (argvs[i]==NULL)
                    break;
				lines=argvs[i];
				parseBlank(lines, argv);  //function to parse blank spaces
				if (*(void**)argv != NULL){
                    /* checks if cmd is cd or exit if not 
                    it uses a function to use execvp */
					if (strcmp(argv[0], "exit") == 0)
						exit(0);
					else if (strcmp(argv[0], "cd") ==0){
						chdir(argv[1]);
					}
					else{
						execute(argv);
					}
				}
                memset(argv, 0, sizeof(argv)); //clear argv
			}
		}
        // if pipe is present
		else{
			parseBlank(linepiped[0], argv);     //function to parse blank spaces
			parseBlank(linepiped[1], argv2);    //function to parse blank spaces
			executePipe(argv,argv2);            //executing the pipe cmd
		}
	}
}