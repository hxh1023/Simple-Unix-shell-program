
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <sys/time.h>

#define MAX_INPUT_LINE 80
#define MAX_LEN_PROMPT 10
#define MAX_NUM_DIRE 10
#define MAX_NUM_ARGU 8
char Dire_list[MAX_NUM_DIRE][MAX_INPUT_LINE];
char *arguments[MAX_NUM_ARGU];
char temp[MAX_INPUT_LINE];
char *envp[] = { 0 };
char *items;
int num_args;
int num_refs;
int dire_num = 0;
char *run_address;
char *ref[MAX_NUM_ARGU];
char *args[MAX_NUM_ARGU];
char *out_file;
char *input_file;

int Check_Path(char *command){
    //To check if the absolute path of the command exist and executable
    int i;
    for(i=0; i<dire_num; i++){
        memset(temp, 0, MAX_INPUT_LINE);
        strncpy(temp, Dire_list[i], strlen(Dire_list[i]));
        strcat(temp, "/");
        strcat(temp, command);
        int bool = access(temp, X_OK);
        if(bool == 0){
            return 1;
        }
    }
    return 0;
}

void Output_in_File(){
    //Execute the single command which stores the command output to a file
    int status;
    int pid;
    int fd;
    //Create a child process
    if ((pid = fork()) == 0) {
        fd = open(out_file, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
        if (fd == -1) {
            fprintf(stderr, "cannot open %s for writing\n", out_file);
            exit(1);
        }
        dup2(fd, 1);
        dup2(fd, 2);
        execve(args[0], args, envp);
    }
    waitpid(pid, &status, 0);
}

void input_in_File(){
    //Execute the single command which has file as command input
    int status;
    int pid;
    int fd;
    //Create a child process
    if ((pid = fork()) == 0) {
        fd = open(input_file, O_RDONLY, S_IRUSR|S_IWUSR);
        if (fd == -1) {
            fprintf(stderr, "%s not exist\n", input_file);
            exit(1);
        }
        dup2(fd, 0);
        execve(args[0], args, envp);
    }
    waitpid(pid, &status, 0);
}

void combined_commands(){
    //Execute the single command which combines multiple commands together
    int status;
    int pid;
    int fd1;
    int fd2;
    //Create a child process
    if ((pid = fork()) == 0) {
        fd1 = open(input_file, O_RDONLY, S_IRUSR|S_IWUSR);
        fd2 = open(out_file, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
        if (fd1 == -1) {
            fprintf(stderr, " %s not exist\n", input_file);
            exit(1);
        }
        if (fd2 == -1) {
            fprintf(stderr, "cannot open %s for writing\n", out_file);
            exit(1);
        }
        dup2(fd1, 0);
        dup2(fd2, 1);
        dup2(fd2, 2);
        execve(args[0], args, envp);
    }
    waitpid(pid, &status, 0);
}

void Output_One_Cmd(){
    //Execute simple commands.
    int pid;
    int status;
    //Create a child process
    if((pid = fork()) == 0){
        execve(temp, args, envp);
    }
    waitpid(pid, &status, 0);
}

int main(int argc, char *argv[]) {
    int back_colon;
    int forward_colon;
    int path_check_flag;
    char input[MAX_INPUT_LINE];
    struct timeval before, after;
    //Open the ".vshrc" file and store path into Dire_list array.
    FILE *fd;
    fd = fopen(".vshrc", "r");
    while(fgets(Dire_list[dire_num], MAX_INPUT_LINE, (FILE*)fd)){
        Dire_list[dire_num][strlen(Dire_list[dire_num]) - 1] = '\0';
        dire_num++;
    }
    fclose(fd);
    //Loop of prompt user to input.
    for(;;) {
        //Reset arguments, input and references
        back_colon = 0;
        forward_colon = 0;
        path_check_flag = 0;
        memset(arguments, 0, MAX_NUM_ARGU);
        memset(input, 0, MAX_INPUT_LINE);
        memset(ref, 0, MAX_NUM_ARGU);
        //Start ask user for input
        fprintf(stdout, "vsh%% ");
        fflush(stdout);
        fgets(input, MAX_INPUT_LINE, stdin);
        if (input[strlen(input) - 1] == '\n') {
            input[strlen(input) - 1] = '\0';
        }
        num_args = 0;
        num_refs = 0;
        items = strtok(input, " ");
        while(items != NULL && num_args < MAX_NUM_ARGU) {
            arguments[num_args] = items;
            num_args++;
            items = strtok(NULL, " ");
        }
        if(strcmp(arguments[0], "exit") == 0){
            //If the command is "exit"
            exit(1);
        }
        gettimeofday(&before, NULL);
        for(int i = 0; i < num_args; i++){
            //Extract command and references
            if(Check_Path(arguments[i]) == 1){
                i ++;
                for(; i < num_args; i++){
                    if(strstr(arguments[i], "::") == 0 && strcmp(arguments[i], "##") != 0){
                        ref[num_refs] = arguments[i];
                        num_refs ++;
                    }
                }
                path_check_flag = 1;
                break;
            }
            if(i == num_args - 1){
                fprintf(stderr, "Absolute Path of the command not exist\n");
            }
        }
        if(path_check_flag == 1){
            for(int i = 0; i < num_args; i++){
                //Extract file name
                 if(strstr(arguments[i], "::")){
                    if(strncmp(arguments[i],"::", 2) == 0){
                        out_file = strtok(arguments[i], "::");
                        forward_colon = 1;
                    }
                    else{
                        input_file = strtok(arguments[i], "::");
                        back_colon = 1;
                    }
                }
            }
            args[0] = temp;
            for(int i = 0; i < num_refs; i++){
                args[i+1] = ref[i];
            }
            args[num_refs + 1] = 0;
            if(forward_colon == 1 && back_colon == 1){
                //If there are output filename and input filename together in a single command
                combined_commands();
            }
            else if(forward_colon == 1){
                //If a single command only has output filename
                Output_in_File();
            }
            else if(back_colon == 1){
                //If a single command only has input filename
                input_in_File();
            }
            else{
                //If it is a simple command.
                Output_One_Cmd();
            }
        }
        gettimeofday(&after, NULL);
        if(strcmp(arguments[num_args-1], "##") == 0){
            fprintf(stdout, "wallclock time:%lu microseconds\n", (after.tv_sec - before.tv_sec) * 1000000 +
                                                   after.tv_usec - before.tv_usec);
        }
    }
}

