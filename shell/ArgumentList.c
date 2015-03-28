//
//  list.c
//  OSShell
//
//  Created by Alexander Russ on 3/27/15.
//  Copyright (c) 2015 Alexander Russ. All rights reserved.
//

#include "ArgumentList.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

StringList* newStringList(char* firstElement) {
    StringList* l = malloc(sizeof(StringList));
    l->data = strdup(firstElement);
    l->next = 0;
    return l;
}

StringList* tailOf(StringList* l) {
    StringList* n = l;
    while (n->next != 0) {
        n = n->next;
    }
    return n;
}

StringList* listPush(StringList* l, char* new) {
    StringList* node = newStringList(new);
    tailOf(l)->next = node;
    return l;
}

StringList* nodeAtIndex(StringList* l, int i) {
    if (i == 0) {
        return l;
    } else {
        if (l->next == 0) {
            fprintf(stderr, "Tried to access element out of bounds at index %d", i);
            exit(3);
        }
        return nodeAtIndex(l->next, i-1);
    }
}

char* findElement(StringList* l, int i) {
    StringList* node = nodeAtIndex(l, i);
    return node->data;
}

unsigned int listLength(StringList* l) {
    if (l == 0) {
        return 0;
    }
    else {
        return 1 + listLength(l->next);
    }
}

void freeList(StringList* l) {
    if (l != NULL) {
        freeList(l->next);
        free(l->data);
        free(l);
    }
}
