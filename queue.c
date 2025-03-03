#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head =
        (struct list_head *) malloc(sizeof(struct list_head));
    if (head == NULL)
        return NULL;
    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (head == NULL)
        return;
    struct list_head *node;
    struct list_head *safe;
    list_for_each_safe (node, safe, head) {
        element_t *e_new = list_entry(node, element_t, list);
        if (e_new->value)
            free(e_new->value);
        free(e_new);
    }
    free(head);
    return;
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    element_t *e_new = malloc(sizeof(element_t));
    if (e_new == NULL) {
        return false;
    }

    e_new->value = (char *) malloc(sizeof(char) * (1 + strlen(s)));
    if (e_new->value == NULL) {
        free(e_new);
        return false;
    }

    strlcpy(e_new->value, s, 1 + strlen(s));

    INIT_LIST_HEAD(&e_new->list);

    if (head->next == head) {
        head->prev = &e_new->list;
        e_new->list.next = head;
        e_new->list.prev = head;
    } else {
        e_new->list.next = head->next;
        e_new->list.prev = head;
        head->next->prev = &e_new->list;
    }
    head->next = &e_new->list;
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    element_t *e_new = malloc(sizeof(element_t));
    if (e_new == NULL) {
        return false;
    }

    e_new->value = (char *) malloc(sizeof(char) * (1 + strlen(s)));
    if (e_new->value == NULL) {
        free(e_new);
        return false;
    }

    strlcpy(e_new->value, s, 1 + strlen(s));

    INIT_LIST_HEAD(&e_new->list);

    if (head->next == head) {
        head->prev = &e_new->list;
        e_new->list.next = head;
        e_new->list.prev = head;
    } else {
        head->prev->next = &e_new->list;
        e_new->list.prev = head->prev;
        e_new->list.next = head;
    }
    head->prev = &e_new->list;
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (head == NULL || head->next == head)
        return NULL;
    element_t *e_rm = list_entry(head->next, element_t, list);
    list_del(&e_rm->list);
    if (sp != NULL)
        strlcpy(sp, e_rm->value, bufsize);
    return e_rm;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (head == NULL || head->next == head)
        return NULL;
    element_t *e_rm = list_entry(head->prev, element_t, list);
    list_del(&e_rm->list);
    if (sp != NULL)
        strlcpy(sp, e_rm->value, bufsize);
    return e_rm;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    int size = 0;
    if (head == NULL || head->next == head)
        return size;
    struct list_head *node;
    struct list_head *safe;

    list_for_each_safe (node, safe, head) {
        size++;
    }
    return size;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (head == NULL || list_empty(head))
        return false;
    else if (list_is_singular(head)) {
        list_del(head->next);
        return true;
    }
    struct list_head *slow = head->next;
    for (struct list_head const *fast = head->next;
         fast->next != head && fast->next->next != head;
         fast = fast->next->next) {
        slow = slow->next;
    }
    list_del(slow);
    element_t *e = list_entry(slow, element_t, list);
    free(e->value);
    free(e);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (head == NULL || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *swp_tmp;
    struct list_head *first = head->next;
    while (first != head && first->next != head) {
        swp_tmp = first->next;

        first->prev->next = swp_tmp;
        swp_tmp->prev = first->prev;

        swp_tmp->next->prev = first;
        first->next = swp_tmp->next;

        swp_tmp->next = first;
        first->prev = swp_tmp;

        first = first->next;
    }
    return;
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (head == NULL || list_empty(head) || list_is_singular(head))
        return;
    int count = q_size(head);
    for (int i = 0; i < count - 1; i++) {
        list_move_tail(head->next, head);
    }
    return;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (head == NULL || list_empty(head)) {
        return 0;
    } else if (list_is_singular(head)) {
        return 1;
    }

    struct list_head *stack =
        (struct list_head *) malloc(sizeof(struct list_head));
    INIT_LIST_HEAD(stack);

    struct list_head *entry;
    struct list_head *safe;

    list_for_each_safe (entry, safe, head) {
        element_t const *current = list_entry(entry, element_t, list);
        list_del(entry);

        while (!list_empty(stack) &&
               strcmp(list_entry(stack->prev, element_t, list)->value,
                      current->value) > 0) {
            element_t *pop = list_entry(stack->prev, element_t, list);
            list_del(&pop->list);
            free(pop->value);
            free(pop);
        }
        list_add_tail(entry, stack);
    }
    list_splice_tail_init(stack, head);
    free(stack);
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (head == NULL || list_empty(head)) {
        return 0;
    } else if (list_is_singular(head)) {
        return 1;
    }

    struct list_head *stack =
        (struct list_head *) malloc(sizeof(struct list_head));
    INIT_LIST_HEAD(stack);

    struct list_head *entry;
    struct list_head *safe;

    list_for_each_safe (entry, safe, head) {
        element_t const *current = list_entry(entry, element_t, list);
        list_del(entry);

        // Compare to the stack, and pop smaller elements
        while (!list_empty(stack) &&
               strcmp(list_entry(stack->prev, element_t, list)->value,
                      current->value) < 0) {
            element_t *pop = list_entry(stack->prev, element_t, list);
            list_del(&pop->list);
            free(pop->value);
            free(pop);
        }

        // Add the current node to the stack
        list_add_tail(entry, stack);
    }

    list_splice_tail_init(stack, head);
    free(stack);
    return q_size(head);  // Return the size of the updated list
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
