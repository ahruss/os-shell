#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "all.h"

StringList* newStringList(char* firstElement) {
    StringList* l = malloc(sizeof(StringList));
    l->data = firstElement != NULL ? strdup(firstElement) : NULL;
    l->next = 0;
    l->isQuoted = false;
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
    if (l == NULL) {
        return node;
    }
    else {
        tailOf(l)->next = node;
        return l;
    }
}

/**
 Adds an element to the front of a list and returns the new head of the list.
 */
StringList* listPushFront(StringList* l, char* new) {
    StringList* node = newStringList(new);
    node->next = l;
    return node;
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
        if (l->data != NULL) free(l->data);
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

StringList* listCopy(StringList* old) {
    StringList* new = NULL;
    StringList* node = old;
    while (node != NULL) {
        new = listPush(new, node->data);
        node = node->next;
    }
    return new;
}

bool listContains(StringList* haystack, char* needle) {
    StringList* node = haystack;
    while (node != NULL) {
        if (needle == NULL) {
            if (node->data == needle) {
                return true;
            }
        } else if (node->data == needle || strcmp(needle, node->data) == 0) {
            return true;
        }
        node = node->next;
    }
    return false;
}
