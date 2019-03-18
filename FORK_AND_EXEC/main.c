#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#define USAGE_STR "Usage: %s str1 [str2 ... strN]\n"
#define POWER_ERR "Argument's length(%lu) has to be a power of 2. Input string has been shortened.\n"
#define ERROR(FUNC_NAME) show_error_and_exit(#FUNC_NAME" failed")

_Bool is_power_of(size_t value, short base) {
    if(!value)
        return 0;
    else if(value == 1)
        return 1;

    while((value % base) == 0)
        value /= base;

    return value == 1;
}
// This function performs a single cut on a string 'part_size' bytes from the beginning.
// Returns the original string if a length is equal to 1. It is guaranteed to fail only if calloc() invocation fails.
// Otherwise returns a first part of the string and sets first argument as another part.
char* cut_string(char** string, size_t part_size) {
    char* new_string = NULL;

    if(strlen(*string) == 1)
        return *string;
    
    if((new_string = (char*)calloc(part_size + 1, 1)) == NULL)
        return NULL;

    memcpy(new_string, *string, part_size);
    *string = *string + part_size;
    return new_string;
}

int standarize(char** string) {
    if(*string == NULL)
        return 1;

    for(size_t i = strlen(*string); i > 1; i--) {
        if(is_power_of(i, 2)) {
            *string += strlen(*string) - i;             
            return 0;
        }
    } 

    return 1;
}

void show_error_and_exit(const char* message) {
    perror(message);
    exit(EXIT_FAILURE);
}

int main(int argc, char** argv) {
    if(argc == 1) {
        printf(USAGE_STR, argv[0]);
        exit(EXIT_FAILURE);
    }

    if(!is_power_of(strlen(argv[argc - 1]), 2)) {
        printf(POWER_ERR, strlen(argv[argc - 1]));

        if(standarize(&argv[argc - 1]))
            ERROR(standarize());
    }

    char* args1[argc + 1], *args2[argc + 1];
    char* half1 = NULL, *half2 = NULL;

    int i;
    for(i = 0; i < argc; i++) {
        args1[i] = argv[i];
        args2[i] = argv[i];
    }

    half2 = argv[i - 1];
    if(strlen(argv[i - 1]) > 1) {
        half1 = cut_string(&half2, strlen(argv[i - 1]) / 2);

        if(!half1)
            ERROR(calloc());

        pid_t pid = fork();

        if(pid == -1) 
            ERROR(fork());

        if(pid == 0) {
            args1[i++] = half1;
            args1[i] = NULL;
            execv(argv[0], args1);
            ERROR(execv());
        }
        else if(pid > 0) {
            pid_t pid2 = fork();

            if(pid2 == -1)
                ERROR(fork());
            
            if(pid2 == 0) {
                args2[i++] = half2;
                args2[i] = NULL;
                execv(argv[0], args2);
                ERROR(execv());
            }
        }
    }

    if(wait(NULL) == -1 && errno != ECHILD)
        ERROR(wait());

    printf("%d ", getpid());
    for(int i = 1; i < argc; i++) {
        printf("%s ", argv[i]);
    }
    puts(" ");

    free(half1);
    exit(EXIT_SUCCESS);
}