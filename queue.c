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
    queue_contex_t *q_ctx = malloc(sizeof(queue_contex_t));
    if (q_ctx == NULL)
        return NULL;
    q_ctx->q = (struct list_head *) malloc(sizeof(struct list_head));
    INIT_LIST_HEAD(q_ctx->q);
    INIT_LIST_HEAD(&q_ctx->chain);
    q_ctx->size = 0;
    q_ctx->id = 0;  // Just for test
    return q_ctx->q;
}

/* Free all storage used by queue */
void q_free(struct list_head *head) {}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    element_t *e_new = (element_t *) malloc(sizeof(element_t));
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

    queue_contex_t *q_ctx =
        list_entry((struct list_head *const *) head, queue_contex_t, q);
    q_ctx->size++;
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    return NULL;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    return NULL;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    return -1;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
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
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head) {}

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
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
