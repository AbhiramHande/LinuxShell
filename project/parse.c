#include "parse.h"

void free_arg_list(char** list, long size){
    for(int i = 0; i < size; i++)
        free(list[i]);
    free(list);
    return;
}

void safe_strcat_lim(char** str_left, const char* str_right, int length){
    char* ret_str = strdup(*str_left);

    if(length < 0){
        ret_str = realloc(ret_str, (strlen(*str_left) + strlen(str_right) + 1)*sizeof(char));
        strcat(ret_str, str_right);
    } 
    else{
        char* temp_2 = strndup(str_right, length);
        ret_str = realloc(ret_str, (strlen(*str_left) + strlen(temp_2) + 1)*sizeof(char));
        strcat(ret_str, temp_2);
        free(temp_2);
    }

    free(*str_left);
    *str_left = ret_str;
    return;
}

void delete_char_in_str(char* str, int pos){
    int len = strlen(str);
    if (pos < 0 || pos >= len) return;
    for (int i = pos; i < len; i++)
        str[i] = str[i + 1];
}

char** parse_command(const char* cmd_line){
    static long size = 0;
    static long* size_dyn = &size;
    static char** arg_list = NULL;

    if(cmd_line == NULL)
        return (char**)size_dyn;
    
    free_arg_list(arg_list, *size_dyn);
    arg_list = NULL;
    size = 0;  

    int len = strlen(cmd_line);
    bool sin_quote = false;
    bool dub_quote = false;
    bool str_cat = false;
    int prev = 0;

    char* cmd_line_dup = strdup(cmd_line);

    for(int i = 0; i < len; i++){
        if(!sin_quote && cmd_line_dup[i] == '\\'){
            if(dub_quote && (cmd_line[i+1] == '\\' || cmd_line[i+1] == '\"' || cmd_line[i+1] == '$')) //To insert more checks tp see if i+1 is valid or not
                delete_char_in_str(cmd_line_dup, i);
            else if(!dub_quote)
                delete_char_in_str(cmd_line_dup, i);
            continue;
        }
        else if(!sin_quote && !dub_quote && isspace(cmd_line_dup[i])){
            if(prev == i) prev++;
            else if(!str_cat){
                arg_list = realloc(arg_list, (++size)*sizeof(char*));
                arg_list[size-1] = strndup(cmd_line_dup + prev, i - prev);
                prev = i + 1;
            }
            else{
                safe_strcat_lim(&arg_list[size-1], cmd_line_dup + prev, i - prev);
                prev = i + 1;
            }
            str_cat = false;
        }
        else if(!dub_quote && cmd_line_dup[i] == '\''){ 
            sin_quote = !sin_quote;

            if(prev == i){
                prev++;
                if(!sin_quote) str_cat = true;
                continue;
            }  
            else if(!str_cat){
                arg_list = realloc(arg_list, (++size)*sizeof(char*));
                arg_list[size-1] = strndup(cmd_line_dup + prev, i - prev);
                prev = i + 1;        
            }
            else{ 
                safe_strcat_lim(&arg_list[size-1], cmd_line_dup + prev, i - prev);
                prev = i + 1;
            } 
          
            str_cat = true;
        } 
        else if(!sin_quote && cmd_line_dup[i] == '\"'){
            dub_quote = !dub_quote;
            if(prev == i){
                prev++;
                if(!dub_quote) str_cat = true;
                continue;
            }
            else if(!str_cat){
                arg_list = realloc(arg_list, (++size)*sizeof(char*));
                arg_list[size-1] = strndup(cmd_line_dup + prev, i - prev);
                prev = i + 1; 
            }
            else{
                safe_strcat_lim(&arg_list[size-1], cmd_line_dup + prev, i - prev);
                prev = i + 1;
            }
            str_cat = true;
        } 

        // for(int i = 0; i < size; i++){
        //     if(arg_list[i] == NULL)
        //         printf("(null)\n");
        //     else
        //         printf("%s, %s\n", arg_list[i], cmd_line_dup + i);
        // }
    }

    if(prev != strlen(cmd_line_dup)){
        if(str_cat) safe_strcat_lim(&arg_list[size-1], cmd_line_dup + prev, -1);
        else{
            int string_len = strlen(cmd_line_dup) - prev;
            arg_list = realloc(arg_list, (++size)*sizeof(char*));
            arg_list[size-1] = calloc(string_len + 1, sizeof(char));
            strncpy(arg_list[size-1], cmd_line_dup + prev, string_len);
        }
    }

    arg_list = realloc(arg_list, (size + 1)*sizeof(char*));
    arg_list[size] = NULL;
    return arg_list;
}