#include "exec.h"

const char* cmd_list[] = {"cd", "echo", "exit", "pwd", "type"};

int is_command(const char* command){
    for(int i = 0; i < CMD_LIST_SIZE; i++){
        if(strcmp(command, cmd_list[i]) == 0)
            return i;
        else if(strcmp(command, cmd_list[i]) < 0){
            return -1;
        }
    }
    return -1;
}

int search_builtin_cmd(char* buffer){
    for(int i = 0; i < CMD_LIST_SIZE; i++){
        if(strncmp(buffer, cmd_list[i], strlen(buffer)) == 0){
            buffer = strcpy(buffer, cmd_list[i]);
            int size = strlen(cmd_list[i]) + 1;
            buffer[size - 1] = ' ';
            buffer[size] = '\0';
            return size;
        }
    }
    return -1;
}

char* get_path(bool next){
    static bool first_call = true;
    static char** path_head = NULL;
    static int curr_index = 0;
    static int no_of_paths = 1;

    if(first_call){
        char* path_org = getenv("PATH");
        char* path = strdup(path_org);
        for(int i = 0; path[i] != '\0'; i++){
            if(path[i] == ':')
                no_of_paths++;
        }
        path_head = calloc(no_of_paths, sizeof(char*));
        char* path_sub = strtok(path, ":");
        for(int i = 0; i < no_of_paths; i++){
            path_head[i] = path_sub;
            path_sub = strtok(NULL, ":");
        }
        first_call = false;
    }

    if(next){
        if(curr_index < no_of_paths)
            return path_head[curr_index++];
        else
            return NULL;
    }
    else{
        curr_index = 0;
        return path_head[curr_index++];
    }
}

char* cmd_type(const char* command){
    static char file_path[PATH_MAX];
    int is_cmd = is_command(command);
    if(is_cmd >= 0)
        return (char*)1;
    else{   
        for(char* path = get_path(false); path != NULL; path = get_path(true)){
            snprintf(file_path, PATH_MAX, "%s/%s", path, command);
            if(access(file_path, X_OK) == 0)
                return file_path;
        }
    }
    return NULL;
}

int delete_arg_in_list(char*** arg_list, int pos, int arg_count){
    if (pos < 0 || pos >= arg_count) return arg_count;
    free((*arg_list)[pos]);
    for (int i = pos; i < arg_count - 1; i++)
        (*arg_list)[i] = (*arg_list)[i + 1];
    *arg_list = realloc(*arg_list, (--arg_count)*sizeof(char*));
    return arg_count;
}

int is_append_fstream(const char* cmd_arg){
    int cmd_arg_len = strlen(cmd_arg);
    if(cmd_arg_len > 1 && cmd_arg[cmd_arg_len - 2] == '>' && cmd_arg[cmd_arg_len - 1] == '>'){
        if(cmd_arg_len == 2) return 1;
        else return atoi(cmd_arg);
    }
    else return 0;
}

int is_redir_fstream(const char* cmd_arg){
    int cmd_arg_len = strlen(cmd_arg);
    if(cmd_arg_len > 0 && cmd_arg[cmd_arg_len - 1] == '>'){
        if(cmd_arg_len == 1) return 1;
        else return atoi(cmd_arg);
    }
    else return 0;
}

int exec_command(char** arg_list, long* arg_count){
    char cwd_path[PATH_MAX];
    char* file_path = NULL;
    char* command = arg_list[0];

    int cmd_int = is_command(command);
    int file_nos[2];
    file_nos[0] = dup(STDOUT_FILENO);
    file_nos[1] = dup(STDERR_FILENO);

    for(int i = 0; i < *arg_count; i++){
        FILE* file_ptr = NULL;
        int stream = 0;
        if(stream = is_append_fstream(arg_list[i])){
            if(i + 1 == *arg_count){
                fprintf(stderr, "syntax error near unexpected token 'newline'");
                exit(EXIT_FAILURE);
            }
            file_ptr = fopen(arg_list[i+1], "a");
            dup2(fileno(file_ptr), stream);
            fclose(file_ptr);
        }
        else if(stream = is_redir_fstream(arg_list[i])){
            if(i + 1 == *arg_count){
                fprintf(stderr, "syntax error near unexpected token 'newline'");
                exit(EXIT_FAILURE);
            }
            file_ptr = fopen(arg_list[i+1], "w");
            dup2(fileno(file_ptr), stream);
            fclose(file_ptr);
        }
        else continue;

        *arg_count = delete_arg_in_list(&arg_list, i, *arg_count);
        *arg_count = delete_arg_in_list(&arg_list, i, *arg_count);
        i--;
    }

    int ret_val = -1;
    switch (cmd_int){
        case 0:
            if(*arg_count > 2)
                fprintf(stdout, "cd: Too many arguments");
            else if(*arg_count == 1 || strcmp(arg_list[1], "~") == 0)
                chdir(getenv("HOME"));
            else if(chdir(arg_list[1]) != 0)
                fprintf(stdout, "cd: %s: No such file or directory\n", arg_list[1]);
            break;

        case 1:
            for(int i = 1; i < *arg_count; i++){
                if(i != 1)
                    fprintf(stdout, " ");
                fprintf(stdout, "%s", arg_list[i]);
            }
            fprintf(stdout, "\n");
            break;
        
        case 2:
            if(*arg_count > 2){
                fprintf(stdout, "exit: too many arguments\n");
                break;
            }

            char *endptr;
            long ret = strtol(arg_list[1], &endptr, 10);
            if(*endptr != '\0'){
                fprintf(stdout, "exit: %s: numeric argument required\n", arg_list[1]);
                break;
            }
            ret_val = (int)ret % 256;
            break;
        
        case 3:
            getcwd(cwd_path, sizeof(cwd_path));
            fprintf(stdout, "%s\n", cwd_path);
            break;

        case 4:
            if(*arg_count == 1)
                break;
            
            for(int i = 1; i < *arg_count; i++){
                file_path = cmd_type(arg_list[i]);
                if(file_path == NULL)
                    fprintf(stdout, "%s: not found\n", arg_list[i]);
                else if(file_path == (char*)1)
                    fprintf(stdout, "%s is a shell builtin\n", arg_list[i]);
                else
                    fprintf(stdout, "%s is %s\n", arg_list[i], file_path);
            }
            break;

        default:
            file_path = cmd_type(command);
            if(file_path == NULL){
                fprintf(stdout, "%s: command not found\n", command);
                break;
            }

            arg_list[0] = strdup(command);
            pid_t proc_id = fork();
            arg_list = realloc(arg_list, (++(*arg_count))*sizeof(char*));
            arg_list[*arg_count - 1] = NULL;
            if(proc_id == 0){
                execv(file_path, arg_list);
                exit(EXIT_FAILURE);
            }
            else{
                int status;
                waitpid(proc_id, &status, 0);
            }
            break;
    }

    dup2(file_nos[0], STDOUT_FILENO);
    dup2(file_nos[1], STDERR_FILENO);
    close(file_nos[0]);
    close(file_nos[1]);
    return ret_val;
}

void disable_canonical_mode(struct termios* original){
    tcgetattr(STDIN_FILENO, original);
    struct termios new = *original;
    new.c_lflag &= ~(ICANON | ECHO);
    new.c_cc[VMIN] = 1;
    new.c_cc[VTIME] = 0;

    tcsetattr(STDERR_FILENO, TCSANOW, &new);
}

void enable_canonical_mode(struct termios* original){
    tcsetattr(STDIN_FILENO, TCSANOW, original);
}

char* listen(FILE* stream){
    long arg_max = sysconf(_SC_ARG_MAX);
    struct termios orginal_terminal_settings;
    char* buffer = calloc(arg_max, sizeof(char));
    int num_chars = -1;
    int index = 0;
    bool loop = true;
    char ch;

    disable_canonical_mode(&orginal_terminal_settings);

    while(loop){
        read(STDIN_FILENO, &ch, 1);

        switch(ch) {
        case '\n':
            write(STDOUT_FILENO, &ch, 1);
            buffer[index] = '\0';
            loop = false;
            break;
        
        case '\t':
            num_chars = search_builtin_cmd(buffer);
            if(num_chars != -1){
                write(STDOUT_FILENO, buffer + index, num_chars - index);
                index = num_chars;
            }
            else{
                write(STDOUT_FILENO, "\a", 1);
            }
            break;

        case '\b':
        case 127:
            if (index > 0) {
                index--;
                write(STDOUT_FILENO, "\b \b", 3);
            }
            else {
                write(STDOUT_FILENO, "\a", 1);
            }
            break;

        default:
            buffer[index++] = ch;
            write(STDOUT_FILENO, &ch, 1);
            break;
        }
    }
    enable_canonical_mode(&orginal_terminal_settings);
    return buffer;
}
