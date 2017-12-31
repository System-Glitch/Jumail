/*
 * LinkedList.h
 *
 *  Created on: 22 août 2017
 *      Author: Jérémy
 */

#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

/**
 * One node of a linkedlist. DO NOT use a node outside a linkedlist
 */
typedef struct node {
	void *val;
	struct node * next;
} node_t;

/**
 * Node container
 */
typedef struct linkedlist {
	int length;
	node_t * head;
} linkedlist_t;

/**
 * Creates an empty linkedlist_t. head is set to NULL and length to 0. Returns NULL of the allocation failed.
 */
linkedlist_t *linkedlist_init();

/**
 * Adds a value at the end of the given list.
 */
void linkedlist_add(linkedlist_t * list, void *value);

/**
 * Creates a node with the given nullable value and returns the result.
 * Should be freed on removal from the list.
 */
node_t * linkedlist_create_node(void *value, node_t * next);

/**
 * Removes and frees the node at the given index
 */
void linkedlist_remove_index(linkedlist_t * list, int index);

/**
 * Removes and frees the first node in the list
 */
void linkedlist_pop(linkedlist_t * list);

/**
 * Returns the value of the node at the given index
 */
void * linkedlist_get(linkedlist_t * list, int index);

/**
 * Returns the node at the given index
 */
node_t * linkedlist_get_node(linkedlist_t * list, int index);

/**
 * Inserts a newly created node from the given value at the given index
 */
void linkedlist_insert(linkedlist_t * list, int index, void *value);

/**
 * Adds a newly created node from the given value at the start of the list
 */
void linkedlist_push(linkedlist_t * list, void *value);

/**
 * Safe free of the list. Also frees the values.
 */
void linkedlist_free(linkedlist_t * list);

#endif /* LINKEDLIST_H_ */
