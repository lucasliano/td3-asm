/* Implementation of a simpled linked list in C.
 * This example was deeply inspired by the Tutorialspoint's one.
 *
 * See the list.h for more details.
*/

#include "list.h"

void list_print(list_t list)
{
   /*******************************************************************************
   * Prints the list contents. This function shall be changed by the user.
   *
   * @param list List that will be used.
   * 
   * @return None.
   ******************************************************************************/
   node_t *p = list.head;
   printf("\n[");

   //start from the beginning
   while(p != NULL) {
      printf("Index: %d - Addr: %p ",p->index, p);
      p = p->next;
   }
   printf("]\n");
}

void list_append(list_t list, int data)
{
   /*******************************************************************************
   * Adds a node at the end of the list.
   *
   * @param list List that will be used.
   * @param data Pointer to the data.
   * 
   * @return None.
   ******************************************************************************/

   //create a link
   node_t *linkedlist = list.head;
   node_t *new_node = (node_t*) malloc(sizeof(node_t));

   // point it to old first node
   while(linkedlist->next != NULL)
      linkedlist = linkedlist->next;

   //point first to new first node
   linkedlist->next = new_node;
   new_node->data = data;
   new_node->index = linkedlist->index;
}

int list_pop(list_t list, int index)
{
   /*******************************************************************************
   * Removes and returns the node at the specified position.
   *
   * @param list List that will be used.
   * @param index Index that will be returned.
   * 
   * @return Pointer to specified node.
   ******************************************************************************/
   node_t *temp = list.head, *prev;

   // If the list is empty
   if (temp == NULL) 
      return NULL;

   // Find the key to be deleted
   while (temp != NULL && temp->index != index) {
      prev = temp;
      temp = temp->next;
   }

   // Remove the node
   prev->next = temp->next;

   //FIXME: Memory should be free here

   return temp->data;
}

int list_search(list_t list, int index)
{
   /*******************************************************************************
   * Returns the node at the specified index.
   *
   * @param list List that will be used.
   * @param index Index that will be returned.
   * 
   * @return Pointer to specified node.
   ******************************************************************************/
   node_t *temp = list.head, *prev;

   // If the list is empty
   if (temp == NULL) 
      return NULL;

   // Find the key to be deleted
   while (temp != NULL && temp->index != index) {
      prev = temp;
      temp = temp->next;
   }

   return temp->data;
}

void list_sort(list_t list)
{
   /*******************************************************************************
   * Sorts the list.
   *
   * @param list List that will be used.
   * 
   * @return None.
   ******************************************************************************/



   // TODO: Implement

   return 0;
}


// ------------ Test Program ------------

void main(){
   int k=0;

   list_t my_list;
   node_t temp; 

   // Init list
   my_list.head = NULL;

   list_append(my_list, 12);
   list_append(my_list, 22);
   list_append(my_list, 30);
   list_append(my_list, 44);
   list_append(my_list, 50);
   printf("Linked List: ");
   list_print(my_list);

   k = list_pop(my_list, 2);
   printf("\nLinked List after deletion: ");
   list_print(my_list);

   k = list_search(my_list, 3);
   printf("\nUpdated Linked List: ");
   list_print(my_list);
}