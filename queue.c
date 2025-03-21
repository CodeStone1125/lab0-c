#include "queue.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    if (head == NULL || list_empty(head))
        return false;
    q_sort(head, false);
    struct list_head *entry, *safe;
    bool dup = false;
    LIST_HEAD(tmp);
    list_for_each_safe (entry, safe, head) {
        element_t *e_entry = list_entry(entry, element_t, list);
        element_t const *e_safe =
            (safe != head) ? list_entry(safe, element_t, list) : NULL;
        if (e_safe && strcmp(e_entry->value, e_safe->value) == 0) {
            dup = true;
            list_del(&e_entry->list);
            q_release_element(e_entry);
        } else if (dup) {
            list_del(&e_entry->list);
            q_release_element(e_entry);
            dup = false;
        } else {
            list_move_tail(&e_entry->list, &tmp);
        }
    }
    list_splice(&tmp, head);
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
    q_reverseK(head, count);
    return;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head) || k <= 1)
        return;

    struct list_head *curr = head->next;
    struct list_head *prev_tail = head;

    while (curr != head) {
        struct list_head *start = curr;
        struct list_head *end = curr;
        int count = 1;

        while (count < k && end->next != head) {
            end = end->next;
            count++;
        }

        if (count < k)
            break;

        struct list_head *next_group = end->next;

        prev_tail->next = end;
        next_group->prev = start;

        struct list_head *p = start, *q = p->next, *temp;
        while (p != end) {
            temp = q->next;
            q->next = p;
            p->prev = q;
            p = q;
            q = temp;
        }

        start->next = next_group;
        end->prev = prev_tail;

        prev_tail = start;
        curr = next_group;
    }
}

struct list_head *mergeTwoList(struct list_head *left,
                               struct list_head *right,
                               bool descend)
{
    struct list_head result;
    INIT_LIST_HEAD(&result);
    while (!list_empty(left) && !list_empty(right)) {
        element_t *e_left = list_first_entry(left, element_t, list);
        element_t *e_right = list_first_entry(right, element_t, list);
        if ((!descend && strcmp(e_left->value, e_right->value) <= 0) ||
            (descend && strcmp(e_left->value, e_right->value) > 0)) {
            list_del_init(&e_left->list);
            list_add_tail(&e_left->list, &result);
        } else {
            list_del_init(&e_right->list);
            list_add_tail(&e_right->list, &result);
        }
    }
    if (!list_empty(left)) {
        list_splice(&result, left);
        return left;
    } else {
        list_splice(&result, right);
        return right;
    }
}


// /* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *slow = head;
    for (struct list_head const *fast = head;
         fast->next->next != head && fast->next != head;
         fast = fast->next->next) {
        slow = slow->next;
    }
    LIST_HEAD(left);
    LIST_HEAD(right);
    list_cut_position(&left, head, slow);
    list_splice_tail_init(head, &right);
    q_sort(&left, true);
    q_sort(&right, true);
    struct list_head *result = mergeTwoList(&left, &right, true);
    list_splice_tail(result, head);
    if (!descend)
        q_reverse(head);
    return;
}

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
    if (!head || list_empty(head))
        return 0;

    queue_contex_t *first = list_entry(head->next, queue_contex_t, chain);
    if (list_is_singular(head))
        return q_size(first->q);
    // struct list_head *del_tmp = first->q;
    struct list_head *cur = head->next->next;
    while ((uintptr_t) cur != (uintptr_t) head) {
        queue_contex_t *ctx = list_entry(cur, queue_contex_t, chain);
        list_splice_init(ctx->q, first->q);
        ctx->size = 0;
        cur = cur->next;
    }
    q_sort(first->q, descend);
    first->size = q_size(first->q);
    return first->size;
}

/* Reference lib/list_sort.corder */
static struct list_head *lx_merge(struct list_head *a, struct list_head *b)
{
    struct list_head *head = NULL, **tail = &head;

    for (;;) {
        /* if equal, take 'a' -- important for sort stability */
        if (strcmp(list_entry(a, element_t, list)->value,
                   list_entry(b, element_t, list)->value) <= 0) {
            *tail = a;
            tail = &a->next;
            a = a->next;
            if (!a) {
                *tail = b;
                break;
            }
        } else {
            *tail = b;
            tail = &b->next;
            b = b->next;
            if (!b) {
                *tail = a;
                break;
            }
        }
    }
    return head;
}

static void lx_merge_final(struct list_head *head,
                           struct list_head *a,
                           struct list_head *b)
{
    struct list_head *tail = head;
    // u8 count = 0;

    for (;;) {
        /* if equal, take 'a' -- important for sort stability */
        if (strcmp(list_entry(a, element_t, list)->value,
                   list_entry(b, element_t, list)->value) <= 0) {
            tail->next = a;
            a->prev = tail;
            tail = a;
            a = a->next;
            if (!a)
                break;
        } else {
            tail->next = b;
            b->prev = tail;
            tail = b;
            b = b->next;
            if (!b) {
                b = a;
                break;
            }
        }
    }

    /* Finish linking remainder of list b on to tail */
    tail->next = b;
    do {
        b->prev = tail;
        tail = b;
        b = b->next;
    } while (b);

    /* And the final links to make a circular doubly-linked list */
    tail->next = head;
    head->prev = tail;
}

void lx_sort(struct list_head *head)
{
    struct list_head *list = head->next, *pending = NULL;
    size_t count = 0; /* Count of pending */

    if (list == head->prev) /* Zero or one elements */
        return;

    /* Convert to a null-terminated singly-linked list. */
    head->prev->next = NULL;

    do {
        size_t bits;
        struct list_head **tail = &pending;

        /* Find the least-significant clear bit in count */
        for (bits = count; bits & 1; bits >>= 1)
            tail = &(*tail)->prev;
        /* Do the indicated merge */
        if (bits) {
            struct list_head *a = *tail, *b = a->prev;

            a = lx_merge(b, a);
            /* Install the merged result in place of the inputs */
            a->prev = b->prev;
            *tail = a;
        }

        /* Move one element from input list to pending */
        list->prev = pending;
        pending = list;
        list = list->next;
        pending->next = NULL;
        count++;
    } while (list);

    /* End of input; merge together all the pending lists. */
    list = pending;
    pending = pending->prev;
    for (;;) {
        struct list_head *next = pending->prev;

        if (!next)
            break;
        list = lx_merge(pending, list);
        pending = next;
    }
    /* The final merge, rebuilding prev links */
    lx_merge_final(head, pending, list);
}

static void rebuild_list_link(struct list_head *head)
{
    if (!head)
        return;
    struct list_head *node, *prev;
    prev = head;
    node = head->next;
    while (node) {
        node->prev = prev;
        prev = node;
        node = node->next;
    }
    prev->next = head;
    head->prev = prev;
}

void sediment_sort(struct list_head *head)
{
    struct list_head *list = head->next;

    if (list == head->prev) /* Zero or one elements */
        return;

    /* Convert to a null-terminated singly-linked list. */
    head->prev->next = NULL;

    struct list_head *new_tail = NULL; /* New bound of sort */
    struct list_head *__prev;          /* Temporary pointer for swap */
    bool done = false;

    do {
        int swapped = -1;
        struct list_head *last_swapped = new_tail;
        // done = false;
        __prev = head;
        for (struct list_head *node = head->next; node != new_tail;
             node = node->next) {
            element_t const *e = list_entry(node, element_t, list);
            if (node->next != new_tail) {
                element_t const *e_next =
                    list_entry(node->next, element_t, list);
                if (strcmp(e->value, e_next->value) > 0) {
                    /* Swap two adjacency node */
                    struct list_head *__next = node->next->next;
                    __prev->next = node->next;
                    node->next->next = node;
                    node->next = __next;
                    node = __prev->next;
                    last_swapped = __next;
                    swapped = 1;
                }
            }
            __prev = node;
        }
        new_tail = last_swapped;
        done = (swapped == -1);

    } while (!done);
    rebuild_list_link(head);
}

void insert_bst(struct list_head **bst_root, struct list_head *node)
{
    node->prev = NULL;
    node->next = NULL;
    if (!*bst_root) {
        *bst_root = node;
        return;
    }
    element_t const *e_node = list_entry(node, element_t, list);
    element_t const *e_root = list_entry(*bst_root, element_t, list);
    if (strcmp(e_node->value, e_root->value) <= 0) {
        insert_bst(&(*bst_root)->prev, node);
    } else {
        insert_bst(&(*bst_root)->next, node);
    }
    return;
}

void traverse(struct list_head **bst_root, struct list_head **head)
{
    struct list_head *work;
    if (*bst_root) {
        traverse(&(*bst_root)->prev, head);
        work = (*bst_root)->next;
        list_add_tail(*bst_root, *head);
        traverse(&work, head);
    }
    return;
}

void tree_sort(struct list_head *head)
{
    struct list_head *bst_root = NULL;
    struct list_head *node;
    struct list_head *safe;
    list_for_each_safe (node, safe, head) {
        list_del_init(node);
        insert_bst(&bst_root, node);
    }
    traverse(&bst_root, &head);
    return;
}

void quick_sort(struct list_head *head)
{
    struct list_head list_less, list_greater;
    element_t *pivot;
    element_t *item = NULL, *is = NULL;

    if (list_empty(head) || list_is_singular(head))
        return;

    INIT_LIST_HEAD(&list_less);
    INIT_LIST_HEAD(&list_greater);

    pivot = list_first_entry(head, element_t, list);
    list_del(&pivot->list);

    list_for_each_entry_safe (item, is, head, list) {
        if (strcmp(item->value, pivot->value) < 0)
            list_move_tail(&item->list, &list_less);
        else
            list_move_tail(&item->list, &list_greater);
    }

    quick_sort(&list_less);
    quick_sort(&list_greater);

    list_add(&pivot->list, head);
    list_splice(&list_less, head);
    list_splice_tail(&list_greater, head);
}

void shuffle(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *node = head->next;
    for (int i = q_size(head); i >= 2; i--) {
        int random = rand() % i;
        for (int j = 0; j < random; j++)
            node = node->next;

        list_del(node);
        list_add_tail(node, head);
        node = head->next;
    }
    return;
}
