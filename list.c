/*
 * Quite Fast Fibers - A basic list for our scheduler
 * 
 * Copyright (c) 2023 Ross Bamford
 * MIT License (see LICENSE.md)
 */

#include <stdlib.h>
#include <stdbool.h>

#include "list.h"

bool list_empty(List *list) {
    return list->sentinel->next == list->sentinel;
}

Node* list_add_tail(List *list, Node *newNode) {
    newNode->next = list->sentinel;
    newNode->prev = list->sentinel->prev;

    list->sentinel->prev->next = newNode;
    list->sentinel->prev = newNode;

    return newNode;
}

Node* list_pop_head(List *list) {
    if (list_empty(list)) {
        return NULL;
    } else {
        Node *temp = list->sentinel->next;
        list->sentinel->next = temp->next;
        temp->next->prev = list->sentinel;
        return temp;
    }
}

List* list_init(List *list) {
    list->sentinel = (Node *)malloc(sizeof(Node));
    list->sentinel->next = list->sentinel;
    list->sentinel->prev = list->sentinel;
    return list;
}

void list_free(List *list) {
    while (!list_empty(list)) {
        list_pop_head(list);
    }
    free(list->sentinel);
}
