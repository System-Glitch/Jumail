#include <stdlib.h>
#include "LinkedList.h"

/**
 * Creates an empty linkedlist_t. head is set to NULL and length to 0.
 */
linkedlist_t *linkedlist_init() {
	linkedlist_t *list = malloc(sizeof(linkedlist_t));
	if(list == NULL) return NULL;
	list->length = 0;
	list->head = NULL;
	return list;
}

/**
 * Adds a value at the end of the given list.
 */
void linkedlist_add(linkedlist_t * list, void *value) {
	node_t * node = linkedlist_create_node(value, NULL);
	if(list->length) {
		node_t * last = linkedlist_get_node(list, list->length-1);
		last->next = linkedlist_create_node(value,NULL);
	} else {
		list->head = node;
	}
	list->length++;
}

/**
 * Creates a node with the given nullable value and returns the result.
 * Should be freed on removal from the list.
 */
node_t * linkedlist_create_node(void *value, node_t * next) {
	node_t * node = NULL;
	node = malloc(sizeof(node_t));
	if(node == NULL) {
		exit(1);
	}
	node->val = value;
	node->next = next;
	return node;
}

/**
 * Removes and frees the node at the given index
 */
void linkedlist_remove_index(linkedlist_t * list, int index) {

	if(list->length == 0) return;

	if(!index) {
		linkedlist_pop(list);
		return;
	}

	node_t * current = list->head;
	node_t * temp_node = NULL;
	for(int i = 1; i < index && current != NULL ; i++) {
		current = current->next;
	}

	temp_node = current->next;
	current->next = temp_node->next;
	if(temp_node->val != NULL)
		free(temp_node->val);
	free(temp_node);

	list->length--;
}

/**
 * Removes and frees the first node in the list
 */
void linkedlist_pop(linkedlist_t * list) {
	if(!list->length) return;
	node_t * next_node = NULL;
	node_t ** head = &list->head;

	next_node = (*head)->next;
	if((*head)->val != NULL)
		free((*head)->val);
	free(*head);
	*head = next_node;
	list->length--;
}

/**
 * Returns the value of the node at the given index
 */
void *linkedlist_get(linkedlist_t * list, int index) {
	node_t *node = linkedlist_get_node(list,index);
	return node != NULL ? node->val : NULL;
}

/**
 * Returns the node at the given index
 */
node_t * linkedlist_get_node(linkedlist_t * list, int index) {
	node_t * current = list->head;
	for(int i = 1; i <= index && current != NULL ; i++) {
		current = current->next;
	}
	return current;
}

/**
 * Inserts a newly created node from the given value at the given index
 */
void linkedlist_insert(linkedlist_t * list, int index, void *value) {
	if(index == list->length) {
		linkedlist_add(list,value);
	} else if(!index) {
		linkedlist_push(list,value);
	} else {
		node_t * previous = linkedlist_get_node(list, index-1);
		previous->next = linkedlist_create_node(value,previous->next);

		list->length++;
	}
}

/**
 * Adds a newly created node from the given value at the start of the list
 */
void linkedlist_push(linkedlist_t * list, void *value) {
	node_t ** head = &list->head;
	node_t * newNode = linkedlist_create_node(value,*head);
	*head = newNode;
	list->length++;
}

/**
 * Safe free of the list. Also frees the values.
 */
void linkedlist_free(linkedlist_t * list) {
	if(list == NULL) return;
	node_t * current;
	while ((current = list->head) != NULL) {
		list->head = list->head->next;
		if(current->val != NULL)
			free (current->val);
		free (current);
	}
	free(list);
}
