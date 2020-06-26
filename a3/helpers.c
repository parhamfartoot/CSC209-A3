#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_ARGS 32
#define MAX_NAME 128

char *remove_tab_and_newline(char *token);

/* Create an array of arguments suitable for passing into execvp 
   from "line". line is expected to start with a tab and contain a
   action from a Makefile. Remember to ensure that the last element
   of the array is NULL.

   It is fine to use MAX_ARGS to allocate enough space for the arguments
   rather than iterating through line twice. You may want to use strtok to
   split the line into separate tokens.

   Return NULL if there are only spaces and/or tabs in the line. No memory
   should be allocated and the return value will be NULL.
 */

char **build_args(char *line) {

    line = &line[1];
    char *arg = remove_tab_and_newline(strtok(line, " "));

    if (arg == NULL){
        return NULL;
    }

    char **args = malloc(sizeof(char*) * MAX_ARGS);
    int i = 0;

    while (arg != NULL){
        args[i] = malloc(sizeof(char) * 5);
        strcpy(args[i], arg);
        i += 1;
        free(arg);
        arg = remove_tab_and_newline(strtok(NULL, " "));
    }
    args[i] = malloc(sizeof(NULL));
    args[i] = NULL;

    return args;
}

/* Return 1 if the line contains only spaces or a comment (#)
   Return 0 if the line contains any other character before a #

   We want to ignore empty lines or lines that contain only a comment.  This
   helper function identifies such lines.
 */
int is_comment_or_empty(char *line) {
    
    for(int i = 0; i < strlen(line); i++){
        if(line[i] == '#') {
            return 1;
        }
        if(line[i] != '\t' && line[i] != ' ') {
            return 0;
        }
    }
    return 1;
}

/* Convert an array of args to a single space-separated string in buffer.
   Returns buffer.  Note that memory for args and buffer should be allocted
   by the caller.
 */
char *args_to_string(char **args, char *buffer, int size) {
    buffer[0] = '\0';
    int i = 0;
    while(args[i] != NULL) {
        strncat(buffer, args[i], size - strlen(buffer));
        strncat(buffer, " ", size - strlen(buffer));
        i++;
    }
    return buffer;
}
//Helper======================================

char *remove_tab_and_newline(char *token){
    if (token == NULL){
        return NULL;
    }
    char *res = malloc(sizeof(char) * strlen(token));
    strcpy(res, token);
    if (res[strlen(res)-1] == '\n' || res[strlen(res)-1] == '\t' || res[strlen(res)-1] == ' '){
        res[strlen(res)-1] = '\0';
    }
    return res;
}


