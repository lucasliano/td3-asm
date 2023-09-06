/* Author: Lucas Liaño (2023)
 * Thanks to Ezequiel Gonzalez for making this possible!
 *
 * Implementation of a simpled linked list in C.
 * This example was deeply inspired by the Tutorialspoint's one.
 * 
 * This is a simple implementation, which aims to simplified the use of lists.
 * We will add the prefix 'list_' to each method.
 * 
 * The API consist of the follwing methods:
 * 
 * list_print()      - Prints the list contents.
 * list_append()     - Adds a node at the end of the list.
 * list_pop()        - Removes and returns the node at the specified position.
 * list_search() - Returns the node at the specified index.
 * list_sort()       - Sorts the list
 * 
*/

#ifndef __LIST_H
#define __LIST_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct node_t{
   int data;  // This shall be modified by the user.
   int index;
   struct node_t *next;
} node_t;


void list_print(node_t* list); // This method shall be changed by the user.

void list_insert(node_t** list, int data);
int list_pop(node_t** list, int index);
int list_search(node_t** list, int index);
void list_sort(node_t** list);


#endif /* defined(__LIST_H) */