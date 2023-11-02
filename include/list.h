/*
 * Quite Fast Fibers - A basic list for our scheduler
 *
 * The list isn't really in-scope for the example, so I won't 
 * comment it too heavily. It's just a simple doubly-linked list
 * in any case, nothing to shout about 📣...
 * 
 * Copyright (c) 2023 Ross Bamford
 * MIT License (see LICENSE.md)
 */

// Structure for a node in the linked list
typedef struct Node {
    struct Node *prev;
    struct Node *next;
} Node;

// Structure for a doubly-linked list with a sentinel node
typedef struct {
    Node *sentinel;
} List;

// Check if the list is empty
bool list_empty(List *list);

// Insert a node at the tail of the list
Node* list_add_tail(List *list, Node *newNode);

// Pop a node from the head of the list
Node* list_pop_head(List *list);

// Initialize the doubly-linked list with a sentinel node
List* list_init(List *list);

// Free the memory allocated for the list, including the sentinel node
void list_free(List *list);
