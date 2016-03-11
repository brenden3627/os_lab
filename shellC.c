/* Name: Xia
** Email: li.6496@osu.edu.cn
**compile: gcc -o shellC shellC.c
**run    : shellC
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAXLINE 80 /* 80 chars per line, per command, should be enough. */
/** The setup() routine reads in the next command line string storing it in the input buffer.
The line is separated into distinct tokens using whitespace as delimiters.  Setup also
modifies the args parameter so that it holds points to the null-terminated strings  which
are the tokens in the most recent user command line as well as a NULL pointer, indicating the
end of the argument list, which comes after the string pointers that have been assigned to
args. ***/
#define FILENAME "/history.txt"

/*
* Use linked list to hold history records. Contains functions of add, remove, search
*/
struct  node{
    int index; /* the id of command*/
    char argsBuff[MAXLINE]; /* command line buffer*/
    struct node *next; /*point to next command record*/
};

/*
 * saveFile function is used to write capacity, index and history records to specific file
 */
void    saveFile(struct node *start,FILE *ofp,int capacity,int index){
    fprintf(ofp,"%d\n",capacity);
    fprintf(ofp,"%d\n",index);
    start=start->next;
    while(start!=NULL){
        fprintf(ofp,"%d  %s\n",start->index,start->argsBuff);
        start=start->next;
    }

}

void    setup(char inputBuff[], char *args[],int *bkgnd, char inputValue[],struct node *head,FILE *file,int capacity,int index)
{
    int length,  /* Num characters in the command line */
            i,       /* Index for inputBuff arrray          */
            j,       /* Where to place the next parameter into args[] */
            start;   /* Beginning of next command parameter */

    /* Read what the user enters */
    length = read(STDIN_FILENO, inputBuff, MAXLINE);

    strcpy(inputValue,inputBuff);
    j = 0;
    start = -1;

    if (length == 0) {
        saveFile(head,file,capacity,index); /* Cntrl-d was entered, call saveFile() function*/
        fclose(file);                       /* Close the file */
        exit(0);            /* Cntrl-d was entered, end of user command stream */
    }
    if (length < 0){
        perror("error reading command");
        exit(-1);           /* Terminate with error code of -1 */
    }

    /* Examine every character in the input buffer */
    for (i = 0; i < length; i++) {

        switch (inputBuff[i]){
            case ' ':
            case '\t':          /* Argument separators */
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
                inputValue[i]='\0';
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

/*recently() is used to set up the command line get from history record.
*/
int     recently(char* inputBuff, char *args[])
{
    if(inputBuff==NULL){
        printf("Cannot find that command in history\n");
        return 1;
    }

    int i=0,       /* Index for inputBuff arrray          */
            j,       /* Where to place the next parameter into args[] */
            start;   /* Beginning of next command parameter */

    j = 0;
    start = -1;
    /* Examine every character in the input buffer */
    int inputlength=strlen(inputBuff);
    for (;i<inputlength+1;) {
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
            case '\0':
                if(start != -1){
                    args[j] = &inputBuff[start];    /* Set up pointer */
                    j++;
                }
                inputBuff[i] = '\0'; /* Add a null char; make a C string */
                start = -1;
                break;

            case '&':
                inputBuff[i] = '\0';
                break;

            default :             /* Some other character */
                if (start == -1)
                    start = i;
        }
        i++;

    }
    if (start != -1){
        args[j] = &inputBuff[start];
    }else{
        args[j]=NULL;
    }

    return 0;
}



/*removenode function is used to remove node from history
*/
void    removenode(struct node *temp)
{
    struct node *nextnode=temp->next;

    temp->next=temp->next->next;
    free(nextnode);

}

/* Search specific element by its command line value
if find, return the previous of the node
if not find,print error message and return NULL
*/
struct  node* search(char inBuff[],struct node **start){

    struct node *current=*start;

    /*iterate the linked list to find node by its command value*/
    while(current->next!=NULL){
        if(!strcmp(current->next->argsBuff,inBuff)){
            break;
        }
        current=current->next;
    }

    return current;
}

/*search specific element by its prefix
if find, return node,otherwise return null;
*/
struct  node* searchstr(char inBuff[],struct node **start){

    struct node *current=*start;
    struct node *result=NULL;
    int         length=strlen(inBuff);
    char        tempBuff[MAXLINE];

    /*iterate the linked list to find node by its command value*/
    while(current!=NULL){

        strcpy(tempBuff,current->argsBuff);
        tempBuff[length]='\0';

        if(!strcmp(tempBuff,inBuff)){
            result=current;
        }

        current=current->next;
    }

    return result;

}
/* Search specific element by its id
if find, return 1,otherwise return 0 and print error message
*/
int     searchindex(int index,struct node **start,char inBuff[] ){

    if(index<=0){
        printf("r with non-positive number: %d\n",index);   /*error checking*/
        return 0;
    }

    struct node *current=*start;

    /*iterate the linked list to find node by its id*/
    while(current->next!=NULL){

        if(current->next->index==index){
            break;
        }
        current=current->next;
    }
    if(current->next!=NULL) {
        strcpy(inBuff,current->next->argsBuff);
        return 1;
    }else{
        printf("%d is not in history buffer\n",index);
        return 0;
    }
}

/* The showhistory() print history one by one */
void    showhistory(struct node *start){
    /*iterate all linked list from head to tail, print history record one by one*/
    while(start!=NULL){
        printf("%d  %s\n",start->index,start->argsBuff);
        start=start->next;
    }
}

/* The add() routine add input command to history record, which implemented by linked list
first check if input command has existed in history record. if existed, move to tail. Then check the size of
history record, if it meet the capacity of history record, remove the head node.
@args: int no: the id of the command
@args: char inBuff[]: command entered
@args: node start: the head of the linked list
@args: size: record the size of history record.
@args: capacity: the capacity of history record, could change by sethistory(), the default value is 8

***/
void    add(int no,char inBuff[],struct node *start,int* size,int capacity){

    struct node *temp=search(inBuff,&start);    /*call search() to check if the command have existed in history record*/
    if(temp->next!=NULL){                       /*if existed, remove it*/
        removenode(temp);
        *size-=1;
    }
    while(*size >=capacity){                    /*check the record size, if over flow, remove the element from the head of linked list*/
        removenode(start);
        *size-=1;
    }

    struct node *current=start;                /*create a new node*/
    struct node *newnode=(struct node *) malloc(sizeof(struct node));
    newnode->index=no;
    strcpy(newnode->argsBuff, inBuff);
    newnode->next=NULL;

    while(current->next!=NULL){                /*append it to the tail of linked list*/
        current=current->next;
    }
    current->next=newnode;
    *size+=1;
}

/*
 * Function: loadFile()
 * if it exists a history record file, load file, populate the value of capacity, index and linked list.
 */
void    loadFile(struct node *start,FILE *ifp,int *capacity,int *index,int *size){
    int no;
    char inputBuff[MAXLINE];
    fscanf(ifp,"%d",capacity);
    fscanf(ifp,"%d",index);
    printf("capacity = %d\n",*capacity);
    printf("index = %d\n",*index);

    while(1){
        if(fscanf(ifp,"%d %[^\n]\n",&no,inputBuff)==EOF){
            break;
        }

        add(no,inputBuff,start,size,*capacity);
    }
}
int     main(void)
{
    struct node *head=NULL;                 /* The head of linked list*/
    char        inputBuff[MAXLINE];         /* Input buffer  to hold the command entered */
    char        inputValue[MAXLINE];        /* Store input buffer for history record*/
    char        *subinputValue;             /* Store string value along to r command*/
    char        *args[MAXLINE/2+1];         /* Command line arguments */
    char        *history_args[MAXLINE/2+1]; /* Command line arguments */
    int         bkgnd;                      /* Equals 1 if a command is followed by '&', else 0 */
    int         size=0;                     /* The length of history*/
    int         index=1;                    /* The current index of command*/
    int         capacity=8;                 /* the initial capacity of history is 8 */
    FILE        *file=NULL;
    char        filePath[200];
    strcpy(filePath,getenv("HOME"));
    strcat(filePath,FILENAME);



    head=(struct node *) malloc(sizeof(struct node));  /*create a fake head, avoid null point error*/
    file=fopen(filePath,"r");
    
    if (file== NULL){                                   /* check if there exists a history file, if so load it, otherwise create a new history file.*/
        printf("No History File");
    }else {
        loadFile(head,file,&capacity,&index,&size);
        fclose(file);

    }
    file=fopen(filePath,"w+");
    
    
    if(file==NULL) {
        printf("Can't open output file %s\n", filePath);
        exit(-1);
    }



    while (1){                                          /* Program terminates normally inside setup */

        bkgnd = 0;

        printf("CSE2431Sh$");                               /* Shell prompt */
        fflush(0);

        setup(inputBuff, args, &bkgnd,inputValue,head,file,capacity,index);          /* Get next command */

        if(inputBuff[0]=='\0'){                             /*if input is null, break */
            continue;
        }

        if(!strcmp(args[0],"r")&&args[1]==NULL){            /*if r without arguments, show error message*/
            printf("r with no history number\n");
            continue;
        }


        if(!strcmp(args[0],"r")){                           /* Used to check the input value followed by command 'r' is num or string */

            if((args[1][0]>='A')) {
                int         tempi=0;                        /* Start index */
                struct node *temp=NULL;                     /* Point to the command followed by 'r' */

                while(inputValue[tempi]=='r'||inputValue[tempi]==' '){
                    tempi++;
                }

                subinputValue = &inputValue[tempi];           /*copy input value followed by 'r' to variable subinputValue*/

                temp = searchstr(subinputValue,&head);        /* call searchstr() to find target command*/

                if(temp==NULL){
                    printf("can not find that command in history\n");
                    continue;
                }
                else{
                    strcpy(inputValue,temp->argsBuff);
                }
            }else if(searchindex(atoi(args[1]),&head,inputValue)==0){
                continue;
            }

            if(!recently(inputValue,history_args)){
               if(strcmp(history_args[0],"cd") == 0){
                if(history_args[1]==NULL){
                    chdir(getenv("HOME"));
                    
                }else if(chdir(history_args[1])<0){
                        printf("chdir %s:no such file or directory\n",args[1]);
                }
                continue;
                } 
            }



        }
        /*if command is 'sethistory'*/
        if(!strcmp(args[0],"sethistory")){

            if(args[1]==NULL||*args[1]=='0'){
                printf("sethistory: must provide exactly one size\n");
            }else{
                capacity=atoi(args[1]);
            }
            continue;
        }

        /* The command read through command line or execute through command 'rr' or 'r' will store in history*/

        if((!strcmp(args[0],"r"))
           ||(!strcmp(args[0],"rr")&&searchindex(index-1,&head,inputValue)==1)
           ||(strcmp(args[0],"r")&&strcmp(args[0],"rr"))){
            add(index,inputValue,head,&size,capacity);
            index++;
        }

        /** In the child processs, first check the command, if it is 'history','h','rr','r','sethistory',then enter specific routine,
        execute specific operations.otherwise, call execvp()
        ***/

         if(!strcmp(args[0],"cd")){
                if(args[1]==NULL){
                    chdir(getenv("HOME"));
                    
                }else if(chdir(args[1])<0){
                        printf("chdir %s:no such file or directory\n",args[1]);
                }
             
                
        }



        pid_t child;
        child = fork();    /*fork another process*/

        if(child<0) {    /*if fork failed, exit the process*/
            fprintf(stderr,"Fork Failed");
            saveFile(head,file,capacity,size);
            fclose(file);
            exit(-1);
        } else if(child==0) {
           

           if(!strcmp(args[0],"history")||!strcmp(args[0],"h")) {       /*if command is 'history' or 'h', call showhistory()*/
                showhistory(head->next);
                exit(-1);

            } else if((!strcmp(args[0],"r")||!strcmp(args[0],"rr"))&&!recently(inputValue,history_args)) { /*if command is 'r' or 'rr'*/
                if(!strcmp(history_args[0],"history")||!strcmp(history_args[0],"h")){
                    showhistory(head->next);
                    exit(-1);

                }else if(execvp(history_args[0],history_args)){
                    printf("execvp: No such file or directory \n");
                    exit(-1);
                }

            } else if(strcmp(args[0],"sethistory")&&execvp(args[0],args)){   /*execute command line in child process*/
                printf("execvp: No such file or directory \n");
                exit(-1);
            }

        } else {
            int status=0;
            if(bkgnd==0) {
                waitpid(child,&status,0);  /* parent will wait for the child process to complete*/
            }
            if(status==1){
                printf("error!");
            }
        }
    }
}
