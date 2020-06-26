#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <ctype.h>
#include "pmake.h"

/* Print the list of actions */
void print_actions(Action *act) {
    while(act != NULL) {
        if(act->args == NULL) {
            fprintf(stderr, "ERROR: action with NULL args\n");
            act = act->next_act;
            continue;
        }
        printf("\t");

        int i = 0;
        while(act->args[i] != NULL) {
            printf("%s ", act->args[i]) ;
            i++;
        }
        printf("\n");
        act = act->next_act;
    }    
}

/* Print the list of rules to stdout in makefile format. If the output
   of print_rules is saved to a file, it should be possible to use it to 
   run make correctly.
 */
void print_rules(Rule *rules){
    Rule *cur = rules;

    while(cur != NULL) {
        if(cur->dependencies || cur->actions) {
            // Print target
            printf("%s : ", cur->target);
            
            // Print dependencies
            Dependency *dep = cur->dependencies;
            while(dep != NULL){
                if(dep->rule->target == NULL) {
                    fprintf(stderr, "ERROR: dependency with NULL rule\n");
                }
                printf("%s ", dep->rule->target);
                dep = dep->next_dep;
            }
            printf("\n");
            
            // Print actions
            print_actions(cur->actions);
        }
        cur = cur->next_rule;
    }
}

/* Create the rules data structure and return it.
   Figure out what to do with each line from the open file fp
     - If a line starts with a tab it is an action line for the current rule
     - If a line starts with a word character it is a target line, and we will
       create a new rule
     - If a line starts with a '#' or '\n' it is a comment or a blank line 
       and should be ignored. 
     - If a line only has space characters ('', '\t', '\n') in it, it should be
       ignored.
 */
Rule *parse_file(FILE *fp) {

    char line[MAXLINE];
    char *token;
    Rule *rules = NULL;
    Rule *current = NULL;
    Rule *rule = NULL;
    Dependency *dep;
    Action *action = NULL;

    while (fgets(line, MAXLINE, fp)) {

        if (isalpha(line[0])) {

            //Gets target
            token = remove_tab_and_newline(strtok(line, " : "));

            if (rules == NULL){
                // if its the first rule
                rules = search_rule(rules, token);
                current = rules;
            } else {
                // if not the first rule
                current = search_rule(rules, token);
            }

            rule = current;
            dep = NULL;
            action = NULL;

            free(token);
            //Gets dependencies
            token = remove_tab_and_newline(strtok(NULL, " : "));

            while (token != NULL){
                    current = search_rule(rules, token);
                if (dep == NULL){
                    rule->dependencies = make_dep(current);
                    dep = rule->dependencies;
                } else {
                    dep->next_dep = make_dep(current);
                    dep = dep->next_dep;
                }
                free(token);
                token = remove_tab_and_newline(strtok(NULL, " "));
            }

        } else
            if (line[0] == '\t') {

                //if first action
                if (action == NULL){
                    rule->actions = make_action(line);
                    action = rule->actions;
                } else {
                    //get last action
                    while (action->next_act != NULL){
                        action = action->next_act;
                    }
                    action->next_act = make_action(line);
                    action = action -> next_act;
                }
        }
    }
    return rules;
}

/* Return the rule with the given target if it already exists in rules.
 * Otherwise return a new node with the given target.
 */
Rule *search_rule(Rule *rules, char *target){
    Rule *curr = rules;
    Rule *temp = NULL;
    if (target == NULL){
        return NULL;
    }
    while (curr != NULL){
        if (strcmp(curr->target,target) == 0){
            return curr;
        } else {
            temp = curr;
            curr = curr->next_rule;
        }
    }

    Rule *rule = malloc(sizeof(Rule));

    char *rule_target = malloc(sizeof(char) * strlen(target));
    strcat(rule_target, target);

    rule->target = rule_target;
    rule->dependencies = NULL;
    rule->actions = NULL;
    rule->next_rule = NULL;

    if (temp != NULL){
        temp->next_rule = rule;
    }

    return rule;
}

Dependency *make_dep(Rule *dep_rule){
    Dependency *dependecy = malloc(sizeof(Dependency));
    dependecy->rule = dep_rule;
    dependecy->next_dep = NULL;
    return dependecy;
}

Action *make_action(char *line){
    Action *action = malloc(sizeof(Action));
    action->args = build_args(line);
    action->next_act = NULL;
    return action;
}


