//
//  list.h
//  OSShell
//
//  Created by Alexander Russ on 3/27/15.
//  Copyright (c) 2015 Alexander Russ. All rights reserved.
//

#ifndef OSShell_list_h
#define OSShell_list_h
#include "types.h"
typedef struct StringList {
    char* data;
    struct StringList* next;
} StringList;

/**
 Creates a new list node on the heap and returns a pointer to it.
 */
StringList* newStringList(char* firstElement);

/**
 Finds the end of a list given the head
 */
StringList* tailOf(StringList* l);

/**
 Appends an element to the end of a list and returns the new head of the list.
 */
StringList* listPush(StringList* l, char* new);

/**
 Adds an element to the front of a list and returns the new head of the list.
 */
StringList* listPushFront(StringList* l, char* new);

/**
 Gets the node at index i of the given list
 
 Dies if the index is out of bounds
 */
char* findElement(StringList* l, int i);

/**
 Find the length of the given list.
 */
unsigned int listLength(StringList* l);

/**
 Deallocate all of a list when you're done with it.
 */
void freeList(StringList* l);

/**
 Take a list of strings and join them together separated by spaces
 */
char* joinWords(StringList*);
/**
 Take a word as it came from the command line and remove backslash escpaes.
 */
char* unescape(char*, bool);

/**
 Returns true iff any of the elements of the list matches the second argument
 */
bool listContains(StringList*, char*);
#endif
