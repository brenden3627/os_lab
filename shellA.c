/* Name: Xia 
** Email: li.6496@osu.edu.cn
**compile: gcc -o shellA shellA.c
**run    : shellA
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXLINE 80 /* 80 chars per line, per command, should be enough. */

/** The setup() routine reads in the next command line string storing it in the input buffer.
The line is separated into distinct tokens using whitespace as delimiters.  Setup also 
modifies the args parameter so that it holds points to the null-terminated strings  which 
are the tokens in the most recent user command line as well as a NULL pointer, indicating the
end of the argument list, which comes after the string pointers that have been assigned to
args. ***/

void setup(char inputBuff[], char *args[],int *bkgnd)
{
    int length,  /* Num characters in the command line */
        i,       /* Index for inputBuff arrray          */
        j,       /* Where to place the next parameter into args[] */
        start;   /* Beginning of next command parameter */

    /* Read what the user enters */
    length = read(STDIN_FILENO, inputBuff, MAXLINE);  

    j = 0;
    start = -1;

    if (length == 0)
        exit(0);            /* Cntrl-d was entered, end of user command stream */

    if (length < 0){
        perror("error reading command");
	exit(-1);           /* Terminate with error code of -1 */
    }
    
    /* Examine every character in the input buffer */
    for (i = 0; i < length; i++) {
 
        switch (inputBuff[i]){
        case ' ':
        case '\t' :          /* Argument separators */

            if(start != -1){
                args[j] = &inputBuff[start];    /* Set up pointer */
                j++;
            }

            inputBuff[i] = '\0'; /* Add a null char; make a C string */
            start = -1;
            break;

        case '\n':             /* Final char examined */
            if (start != -1){
                args[j] = &inputBuff[start];     
                j++;
            }

            inputBuff[i] = '\0';
            args[j] = NULL; /* No more arguments to this command */
            break;

        case '&':
            *bkgnd = 1;
            inputBuff[i] = '\0';
            break;
            
        default :             /* Some other character */
            if (start == -1)
                start = i;
	}
 
    }    
    args[j] = NULL; /* Just in case the input line was > 80 */
} 

int main(void)
{
    char inputBuff[MAXLINE]; /* Input buffer  to hold the command entered */
    char *args[MAXLINE/2+1];/* Command line arguments */
    int bkgnd;             /* Equals 1 if a command is followed by '&', else 0 */
    

    while (1){            /* Program terminates normally inside setup */

	bkgnd = 0;

	printf("CSE2431Sh$");  /* Shell prompt */
        fflush(0);

     setup(inputBuff, args, &bkgnd);       /* Get next command */

	/* Fill in the code for these steps:  
	 (1) Fork a child process using fork(),
	 (2) The child process will invoke execvp(),
	 (3) If bkgnd == 0, the parent will wait, 
		o/w returns to the setup() function. */
     
     pid_t child;
     child=fork();    /*fork another process*/
     if(child<0) {    /*if fork failed, exit the process*/
        fprintf(stderr,"Fork Failed");
        exit(-1);
     }else if (child==0){  
        if(execvp(args[0],args)){   /*execute command line in child process*/
            printf("execvp: No such file or directory \n");
            exit(-1);
        }  
     }else {
        if(bkgnd==0) {
            waitpid(child,&bkgnd,0);  /* parent will wait for the child process to complete*/
        }
     }
     
    }
}
