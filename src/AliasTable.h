//
//  AliasTable.h
//  shell
//
//  Created by Eric Pogash on 4/2/15.
//  Copyright (c) 2015 Alexander Russ. All rights reserved.
//

#ifndef __shell__AliasTable__
#define __shell__AliasTable__

#include <stdio.h>
#include "ArgumentList.h"
#include "all.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct AliasList {
    char* alias;
    char *value;
    StringList *argsList;
    struct AliasList* next;
} AliasList;

extern AliasList *aliasList;

/**
 Creates a new list node on the heap and returns a pointer to it.
 */
AliasList* newAliasList(char* firstAlias, char *firstValue, StringList* args);

/**
 Appends an element to the end of a list and returns the new head of the list.
 */
AliasList* aliasListPush(AliasList* l, char* alias, char *value, StringList *args);

/**
 Removes an element corresponding to the value, if it exists, and returns the new head of the list.
 */
AliasList* aliasListRemove(AliasList* l, char* alias);

/* separate args by spaces
 */
StringList* parseAliasArgs(StringList *stringList);
/**
 Find the length of the given list.
 */
unsigned int aliasListLength(AliasList* l);

/*
 Print the alias and value associated with the alias, one per line.
 */
void printAliasList();

/**
 Deallocate all of a list when you're done with it.
 */
void freeAliasList(AliasList* l);

#endif /* defined(__shell__AliasTable__) */
