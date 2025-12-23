#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ncurses.h>

/*
    call_curses:
        Initializes a curses screen from the startup of this program. It uses the screens max y and x to
        display a firework in the middle of the screen as a introduction to the program. Uses multiple
        for loops to display these with mvaddch.

    Args:
        N/A

    Returns:
        Nothing

*/
void call_curses(){
    int max_y, max_x;
    initscr();
    cbreak();
    noecho();
    curs_set(0); 

    getmaxyx(stdscr, max_y, max_x);
    // print the rocket shooting up
    for(int i = 1; i < max_y * 0.6; i++){
        mvaddch(max_y - i - 1, max_x / 2, '#');
        mvaddch(max_y - i, max_x / 2, '|');
        refresh();
        usleep(60000);
        mvaddch(max_y - i - 1, max_x / 2, ' ');
        mvaddch(max_y - i, max_x / 2, ' ');
    }

    // Print the explosion
    for(int i = 0; i < 8; i++){
        for(int j = 1; j < i; j++){
            mvaddch(max_y * 0.4 + j, max_x / 2 + j, '#');
            mvaddch(max_y * 0.4 - j, max_x / 2 - j, '#');
            mvaddch(max_y * 0.4 - j, max_x / 2 + j, '#');
            mvaddch(max_y * 0.4 + j, max_x / 2 - j, '#');
            mvaddch(max_y * 0.4, max_x / 2 + j, '#');
            mvaddch(max_y * 0.4 - j, max_x / 2, '#');
            mvaddch(max_y * 0.4 + j, max_x / 2, '#');
            mvaddch(max_y * 0.4, max_x / 2 - j, '#');
            refresh();
        }
        usleep(50000);
    }
    //Delete the explosion
    for(int i = 0; i < 8; i++){
        for(int j = 1; j < i; j++){
            mvaddch(max_y * 0.4 + j, max_x / 2 + j, ' ');
            mvaddch(max_y * 0.4 - j, max_x / 2 - j, ' ');
            mvaddch(max_y * 0.4 - j, max_x / 2 + j, ' ' );
            mvaddch(max_y * 0.4 + j, max_x / 2 - j, ' ');
            mvaddch(max_y * 0.4, max_x / 2 + j, ' ');
            mvaddch(max_y * 0.4 - j, max_x / 2, ' ');
            mvaddch(max_y * 0.4 + j, max_x / 2, ' ');
            mvaddch(max_y * 0.4, max_x / 2 - j, ' ');
            refresh();
        }
        usleep(30000);
    }

    endwin();
}

/*
    echo_cmd:
        Built-in echo command called from shell loop when "echo" is typed in the shell input line.
        when echo has no arguments, it does nothing. When echo has "" or '' around the arguments, it gets rid 
        of them like a typical shell would and displays the arguments inside. Does the same thing without the 
        quotes but gets rid of nothing. When formatted as echo -n (arg) it doesn't add a newline to the output
        and takes new input from user on the same line.

    Args:
        arg (char *): pointer to a character string that holds the argument after users echo input.

    Returns:
        Nothing.

*/
void echo_cmd(char *arg){
    if(arg == NULL){ // if theres no argument dont print anything
        return;
    }
    char *new_string;
    int len = strlen(arg);

    if(arg[0] == '-' && arg[1] == 'n' && arg[2] == ' '){ //case for -n argument, print with no new line
        char *temp = strtok(arg, " ");
        char *arg = strtok(NULL, " ");
        printf("%s", arg);
        return;
    }

    if (len >= 2 &&
        ((arg[0] == '\'' && arg[len-1] == '\'') || 
        (arg[0] == '"' && arg[len-1] == '"'))){ //case when the argument is encapsulated by single or double quotes
        char new_string[len - 1];
        strncpy(new_string, arg + 1, len - 2);
        new_string[strcspn(new_string, "\n")] = '\0';
        printf("%s\n", new_string);
        }
    else{ //normal case
        printf("%s\n", arg);
    }
}

/*
    pwd_cmd:
        Build-In Print Working Directory command, same as the linux command for the same thing. It
        prints to the terminal the working directory the user is currently in. It obtains this from the 
        getcwd() command. When there is an error, it returns it and prints that to the terminal.

    Args:
        N/A

    Returns:
        Nothing

*/
void pwd_cmd(){
    char cwd[256];
    if(getcwd(cwd, sizeof(cwd)) != NULL){ //get the current working directory
        printf("%s\n", cwd);
    }
    else{
        perror("pwd");
    }
}

/*
    cd_cmd:
        Built-In change directory command, similar to the cd command in linux. Changes the current working
        directory to the one provided, if it exists. If it has no arguments and just cd is entered, it takes the user
        to the home directory.

    Args:
        arg (char *): 

    Returns:
        Nothing

*/
void cd_cmd(char *arg){
    if(arg == NULL){ //if there are no arguments, change it to the home directory
        arg = getenv("HOME");
    }
    if(chdir(arg) != 0){ //try to change directory, if it doesnt work, print an error with cd
        perror("cd");
    }
}

/*
    read_line:
        This function takes input from the user using getline. It then validates the input and 
        makes sure no errors occur and prints out the list of commands if no input is found.

    Args:
        N/A

    Returns:
        input (char *):

*/
char *read_line(){
    char *input = NULL; //initialize input variable
    size_t bufsize = 0;
    
    ssize_t nread = getline(&input, &bufsize, stdin);
    if(nread == -1){
        if(feof(stdin)){ //check for end of file indicator for stdin
            free(input);
            exit(EXIT_SUCCESS);
        }
        else{
            perror("readline");
            free(input);
            exit(EXIT_FAILURE);
        }
    }
    if(strcmp(input, "\n") == 0){
        printf("%s\n", "Here are the list of commands for the shell:");
        printf("%s\n", "echo: displays text from input");
        printf("%s\n", "pwd (print working directory): Displays the absolute path of the current working directory");
        printf("%s\n", "cd (change directory): Navigate the file system in the shell");
        printf("%s\n", "fireworks: Show the fireworks again!");
    }
    return input;
}

/*
    split_line:
        This function takes the users input from read_line, and then breaks them up into tokens.
        It uses strtok to store each token, which is an pointer to a character, and makes a new pointer for that
        pointer. It will reallocate the size of tokens when needed, and null terminate the final
        index of tokens for iterating through the array.

    Args:
        line (char *):

    Returns:
        tokens (char **): Pointer to a pointer to a character.

*/
char **split_line(char *line){
    int bufsize = 64;
    int position = 0;
    char *token;
    char **tokens = malloc(bufsize * sizeof(char*)); //This will hold all of the arguments
    token = strtok(line, " \n"); //Get the first argument, either delimited by a space or a newline character
    while(token != NULL){ //While the token does not equal null keep getting tokens
        tokens[position] = token; //put the next token in the tokens at position (starts at 0 and increments every time for correct indexing)
        position++;
        if(position > bufsize){ //if position is greater than bufsize (our allocated memory for tokens) we add another 64 bits to tokens memory with realloc
            bufsize += 64;
            tokens = realloc(tokens, bufsize * sizeof(char*));
        }
        token = strtok(NULL, " \n"); //get a new token
    }
    tokens[position] = NULL; //NULL terminate tokens to find end of tokens for indexing elsewhere
    return tokens;
}

/*
    shell_execute:
        Creates a child and parent process that both do different things. The child process will take the args argument and save the 0 index of that to the cmd variable. It will then get the
        path variable and a directory (dir) variable that is tokens from the path variable. While the directory variable is not null, it will check if the path variable plus the 
        command variable, / seperated for execution, is an executable command. If it is, it will execute said command and that process will end. The parent process waits until that child process is
        finished to continue on.

    Args:
        args (char **): pointer to a pointer that points to a character. Stores every argument that has 
        been inputed for iterating through.

    Returns:
        1 or 0 (int): 1 on success and 0 on failure.
*/
int shell_execute(char **args){
    pid_t child_pid;
    int status;
    child_pid = fork(); //create a seperate process
    if(child_pid < 0){ //error check the process
        fprintf(stderr, "fork failed\n");
    }
    else if(child_pid == 0){ //child process runs here
        char *cmd = args[0]; //get the command inputted at index 0 of args
        char *path_env = getenv("PATH"); //get the path variables and store them here for comparison and tokenizing
        char copy_path[512];

        strncpy(copy_path, path_env, sizeof(copy_path)); //set copy_path variable to the path_env so we can edit and tokenize the path
        copy_path[sizeof(copy_path) - 1] = '\0'; //null terminate the copy_path

        char fullpath[512];
        char *dir = strtok(copy_path, ":"); //get first token delimited by :

        while(dir != NULL){ //while there is a token to check
            snprintf(fullpath, sizeof(fullpath), "%s/%s", dir, cmd); //append the command given, with the path token
            if(access(fullpath, X_OK) == 0){ //check if that fullpath is an executable file
                execv(fullpath, args); //execute it
                perror("execv"); //if it gets here it means that the child process failed
                exit(EXIT_FAILURE);
            }

            dir = strtok(NULL, ":"); //get a new token
        }
        exit(EXIT_SUCCESS);
    }
    else{ //parent process
        waitpid(child_pid, &status, 0); //wait for the child process to end to continue
    }
    return 1;
}

/*
    shell_loop:
        Main shell loop, it calls all the other helper functions to do the main loop. It prints out the shell input line (>) and then calls read_line and split_line for input parsing.
        It then checks against built in functions first for faster execution. echo, pwd, cd, and exit are built in these. Free's up the space used and continues looping until exit is called.

    Args:
        N/A

    Returns:
        Nothing 

*/
void shell_loop(){
    char *line;
    char **args;
    int status = 1;
    printf("%s", "Welcome to Hunter's Shell! type enter to get the list of commands\n");
    do{
        printf("> ");
        line = read_line(); //get the line inputted by user
        args = split_line(line); //split the line into arguments

        if(strcmp(args[0], "echo") == 0){ //echo function

            if((args[1] != NULL) && (strcmp(args[1], "-n") == 0)){ //functionality for -n argument
                char new_args[256];
                strcpy(new_args, args[1]);// get the -n argument into a new string
                strcat(new_args, " "); //append the string with a space
                strcat(new_args, args[2]); //add the rest of the arguments to that string
                echo_cmd(new_args); //run command
                free(line);
                free(args);
                continue;
            }
            echo_cmd(args[1]); //run command
            free(line);
            free(args);
            continue; //keep looping
        }
        else if(strcmp(args[0], "pwd") == 0){ //pwd command comparison
            pwd_cmd();
            free(line);
            free(args);
            continue;
        }
        else if(strcmp(args[0], "cd") == 0){ //cd command comparison
            cd_cmd(args[1]);
            free(line);
            free(args);
            continue;
        }
        else if(strcmp(args[0], "exit") == 0){ //exit command comparison, will exit the program if true
            printf("%s", "Exiting Shell, Goodbye\n");
            free(line);
            free(args);
            break;
        }
        else if(strcmp(args[0], "fireworks") == 0){ //run fireworks again
            call_curses();
            free(line);
            free(args);
            continue;
        }
        status = shell_execute(args); //status of the shell, if 0, will end the program

        free(line);
        free(args);
    } while(status);
}

int main(){
    call_curses();
    shell_loop();
    return 0;
}