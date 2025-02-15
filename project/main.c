#include "exec.h"
#include "parse.h"

int main() {
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    while(true) {
        printf("$ ");
        fflush(stdout);
        // char input[100];
        // fgets(input, 100, stdin);
        // char* cmd_line_save_ptr = NULL;
        // char* cmd_line = strtok_r(input, "\r\n\0", &cmd_line_save_ptr);
        char* cmd_line = listen(stdin);
        if(strlen(cmd_line) != 0){
            char** arg_list = parse_command(cmd_line);
            long* size = (long*)parse_command(NULL);
            #ifdef DEBUG
                for(int i = 0; i < size; i++){
                    if(arg_list[i] == NULL)
                        printf("(null)\n");
                    else
                        printf("%s\n", arg_list[i]);
                }
            #endif
            int ret = exec_command(arg_list, size);
            if(ret != -1)
                return ret;  
           
            //cmd_line = strtok_r(NULL, "\r\n\0", &cmd_line_save_ptr);
        }
        free(cmd_line);
    }
}
