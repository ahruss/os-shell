//
//  AliasTable.c
//  shell
//
//  Created by Eric Pogash on 4/2/15.
//  Copyright (c) 2015 Alexander Russ. All rights reserved.
//

#include "AliasTable.h"
void freeAliasList(AliasList* l);

AliasList *aliasList = NULL;

AliasList* newAliasList(char* firstAlias, char *firstValue, StringList* args) {
    AliasList* l = malloc(sizeof(AliasList));
    l->alias = strdup(firstAlias);
    l->value = strdup(firstValue);
    l->argsList = args;
    l->next = 0;
    return l;
}

unsigned int aliasListLength(AliasList* l) {
    if (l == 0) {
        return 0;
    }
    else {
        return 1 + aliasListLength(l->next);
    }
}

AliasList *getAliasFromString(char* string) {
    AliasList *l = aliasList;
    while (l != NULL) {
        if (strcmp(string, l->alias) == 0){
            return l;
        }
        l = l->next;
    }
    return NULL;
}

bool isValidAlias(char* alias, char *value) {
    AliasList* oldAlias = getAliasFromString(value);
    while (oldAlias != NULL) {
        if (strcmp(oldAlias->value, alias) == 0) {
            return false;
        }
        oldAlias = getAliasFromString(oldAlias->value);
     }
    return true;
}

AliasList* aliasListPush(AliasList* l, char* alias, char *value, StringList *args) {
    if(!isValidAlias(alias, value)) {
        printf("Error: infinite expansion would result.");
        return 0;
    }
    if(aliasListLength(l) == 0) {
        l = newAliasList(alias, value, args);
        aliasList = l;
    } else {
        AliasList* n = l;
        while (n->next != 0) {
            if(strcmp(n->alias, alias) == 0) {
                n->value = value;
                n->argsList = args;
                return l;
            }
            n = n->next;
        }
        n->next = newAliasList(alias, value, args);
        return n;
    }
    return l;
}

AliasList* aliasListRemove(AliasList* l, char* alias) {
    if(aliasListLength(l) <= 0){
        return l;
    } else if (aliasListLength(l) == 1) {
        AliasList* first = l->next;
        if (strcmp(first->alias, alias) == 0) {
            freeAliasList(l);
            return 0;
        }
        return l;
    }
    if(strcmp(l->alias, alias) == 0) {
        l = l->next;
        return l;
    }
    
    AliasList* first = l->next;
    AliasList* second = l;
    while (first != 0) {
        if(strcmp(first->alias, alias) == 0) {
            AliasList *tmp = first;
            second->next = first->next;
            free(first);
            return tmp;
        }
        first = first->next;
        second = second->next;
    }
    return l;
}

void printAliasList() {
    AliasList *l = aliasList;
    for(int i = 0; i < aliasListLength(aliasList); i++) {
        printf("%s=%s", l->alias, l->value);
        StringList *tempList = l->argsList;
        for(int j = 0; j < listLength(tempList); j++) {
            printf(" %s", tempList->data);
            tempList = tempList->next;
        }
        printf("\n");
        l = l->next;
    }
}

StringList* parseAliasArgs(StringList *stringList) {
    StringList *tempList = stringList;
    StringList *argsList = newStringList(tempList->data);
    tempList = tempList->next;
    char* args = tempList->data;
    
    char* str = args;
    char** argsArray  = NULL;
    char*  p    = strtok (str, " ");
    int n_spaces = 0;
    /* split string by spaces, append to  argsArray*/
    while (p) {
        argsArray = realloc (argsArray, sizeof (char*) * ++n_spaces);
        argsArray[n_spaces-1] = p;
        p = strtok (NULL, " ");
    }
    
    /* needed for NULL terminate */
    argsArray = realloc (argsArray, sizeof (char*) * (n_spaces+1));
    argsArray[n_spaces] = 0;
    
    /* add each string to the end of the argsList, return the list*/
    for(int i = 0; i < n_spaces; i++) {
        listPush(argsList, argsArray[i]);
    }
    return argsList;
}

void freeAliasList(AliasList* l) {
    if (l != NULL) {
        freeAliasList(l->next);
        freeList(l->argsList);
        free(l->alias);
        free(l->value);
        free(l);
    }
}