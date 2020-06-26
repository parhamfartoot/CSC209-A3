#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <time.h>

#include "pmake.h"
void execute(Action *action);
int need_to_update(char *target_path, char *dep_path);
void parallel(Dependency *dep,Rule *rules,Rule *to_eval);

void run_make(char *target, Rule *rules, int pflag) {
    Rule *to_eval;
    Dependency *dep;

    if (target != NULL) {
        to_eval = search_rule(rules, target);
        dep = to_eval->dependencies;

        if (pflag == 1) {
            parallel(dep,rules,to_eval);
        }
        else {
            while (dep != NULL) {
                run_make(dep->rule->target, rules, pflag);
                if (need_to_update(to_eval->target, dep->rule->target) == 1) {
                        execute(to_eval->actions);
                }
                dep = dep->next_dep;
            }
        }

    } else {
        run_make(rules->target, rules, pflag);
    }
}
//====================================================Helper============================================================

int need_to_update(char *target_path, char *dep_path){
    struct stat target_info;
    struct stat dep_info;

    if ((stat(dep_path, &dep_info) != 0) || (difftime(target_info.st_mtime ,dep_info.st_mtime ) < 0) || (stat(target_path, &target_info) != 0)){
        return 1;
    }
    return 0;
}

void execute(Action *action){
    if (action == NULL){
        return ;
    }
    while (action != NULL){

        pid_t res = fork();

        if (res > 0){
            sleep(5);
            int status;
            if( (wait(&status)) == -1) {
                perror("wait");
                exit(1);
            }
            else {
                    if (WEXITSTATUS(status) == 1){
                        exit(1);
                    }else
                        if(WIFSIGNALED(status)){
                            exit(1);
                        }
            }
        } else if (res == 0){
            execvp(action->args[0], action->args);
            perror("execvp");
            exit(1);
        } else {
            perror("fork");
            exit(1);
        }
        action = action->next_act;
    }
}
void parallel(Dependency *dep,Rule *rules,Rule *to_eval){
    while (dep != NULL) {
        pid_t result = fork();
        if (result == 0) {
            run_make(dep->rule->target, rules, 0);
            if (need_to_update(to_eval->target, dep->rule->target) == 1) {
                exit(1);
            }
            exit(0);
        } else if (result > 0) {
            dep = dep->next_dep;
        } else {
            perror("fork");
            exit(-1);
        }
    }
    int status;
    if ((wait(&status)) == -1) {
        perror("wait");
        exit(1);
    } else {
        if (WIFEXITED(status)) {
            if (WEXITSTATUS(status) == 1) {
                execute(to_eval->actions);
                exit(1);
            } else if (WEXITSTATUS(status) == 0) {
                return;
            }
        } else if (WIFSIGNALED(status)) {
            exit(1);
        }
    }
}

