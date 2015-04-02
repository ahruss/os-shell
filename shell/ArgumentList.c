//
//  list.c
//  OSShell
//
//  Created by Alexander Russ on 3/27/15.
//  Copyright (c) 2015 Alexander Russ. All rights reserved.
//

#include "all.h"
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

char getEscapeSequence(char c) {
    switch (c) {
        case 'a': return '\a';
        case 'n': return '\n';
        case 'b': return '\b';
        case 'f': return '\f';
        case 't': return '\t';
        default: return c;
    }
}

char* unescape(char* word, bool isString) {
    unsigned long length = strlen(word);
    char* unescaped = malloc(sizeof(char) * (length + 1));
    bool isEscaped = false;
    int j = 0;
    for (int i = 0; i < length; ++i) {
        if (word[i] != '\\' || isEscaped) {
            if (isString && isEscaped) {
                unescaped[j] = getEscapeSequence(word[i]);
            } else {
                unescaped[j] = word[i];
            }
            ++j;
            isEscaped = false;
        } else if (word[i] == '\\') {
            isEscaped = true;
        } else {
            isEscaped = false;
        }
    }
    // make sure we have the null terminator at the end of the escaped string.
    unescaped[j] = '\0';
    return unescaped;
}

char* joinWords(StringList* list) {
    if (list == NULL) return NULL;
    unsigned long length = 0;
    StringList* node = list;
    while (node != NULL) {
        length += strlen(node->data) + 1;
        node = node->next;
    }
    char* joined = malloc(sizeof(char) * length);
    strcpy(joined, unescape(list->data, true));
    node = list->next;
    while (node != NULL) {
        char* unescaped = unescape(node->data, true);
        strcat(joined, unescaped);
        free(unescaped);
        node = node->next;
    }
    return joined;
}