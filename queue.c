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
    struct list_head *new = malloc(sizeof(struct list_head));
    if (!new)
        return NULL;  // to check if allocation was successful
    INIT_LIST_HEAD(new);
    return new;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;

    element_t *cur, *next;
    list_for_each_entry_safe (cur, next, head, list) {
        list_del(&cur->list);
        q_release_element(cur);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *new = malloc(sizeof(element_t));
    char *tmp = strdup(s);
    if (!new || !tmp) {
        free(new);
        free(tmp);
        return false;  // to check if allocation was successful
    }

    new->value = tmp;
    list_add(&new->list, head);
    // free(tmp);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *new = malloc(sizeof(element_t));
    char *tmp = strdup(s);
    if (!new || !tmp) {
        free(new);
        free(tmp);
        return false;  // to check if allocation was successful
    }

    new->value = tmp;
    list_add_tail(&new->list, head);
    // free(tmp);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *rm_element = list_first_entry(head, element_t, list);
    list_del(head->next);
    if (sp) {
        strncpy(sp, rm_element->value, bufsize);
        sp[bufsize - 1] = '\0';
    }
    return rm_element;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *rm_element = list_last_entry(head, element_t, list);
    list_del(head->prev);
    if (sp) {
        strncpy(sp, rm_element->value, bufsize);
        sp[bufsize - 1] = '\0';
    }
    return rm_element;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;
    struct list_head *slow = head->next;
    for (struct list_head *fast = head->next;
         fast != head && fast != head->prev;) {
        slow = slow->next;
        fast = fast->next->next;
    }
    list_del(slow);
    q_release_element(list_entry(slow, element_t, list));
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)  // false
{
    if (!head || list_empty(head))
        return false;
    element_t *front, *back;
    bool is_dup = false;
    list_for_each_entry_safe (front, back, head, list) {
        if (front->value == back->value) {
            is_dup = true;
            list_del(&front->list);
            q_release_element(front);
        } else {
            if (is_dup) {
                is_dup = false;
                list_del(&front->list);
                q_release_element(front);
            } else {
                is_dup = false;
            }
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    q_reverseK(head, 2);
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *cur, *next;
    list_for_each_safe (cur, next, head) {
        list_move(cur, head);
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (!head || list_empty(head) || k == 1)
        return;

    int len = q_size(head);
    if (k > len)
        return;

    struct list_head *cur, *safe, *tmp_head = head, rse;
    INIT_LIST_HEAD(&rse);
    int count = 0;
    list_for_each_safe (cur, safe, head) {
        count++;
        if (count == k) {
            count = 0;
            list_cut_position(&rse, tmp_head, cur);
            q_reverse(&rse);
            list_splice(&rse, tmp_head);
            tmp_head = safe->prev;
        }
    }
}

void merge_list(struct list_head **left, struct list_head *right, bool descend)
{
    // merge with recursive
    // if (!right)
    //     return;
    // if (!left) {
    //     *left = right;
    //     return;
    // }
    // if (descend) {
    //     char *s1 = list_entry(*left, element_t, list)->value;
    //     char *s2 = list_entry(right, element_t, list)->value;
    //     if (strcmp(s1, s2) > 0) {
    //         left->next = merge_list(left->next, right, descend);
    //         return left;
    //     } else {
    //         right->next = merge_list(left, right->next, descend);
    //         return right;
    //     }
    // } else {
    //     if (strcmp(s1, s2) < 0) {
    //         left->next = merge_list(left->next, right, descend);
    //         return left;
    //     } else {
    //         right->next = merge_list(left, right->next, descend);
    //         return right;
    //     }
    // }
}
/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || !head->next)
        return;

    // step1. split from middle
    struct list_head *slow = head;
    for (struct list_head *fast = head->next; fast && fast->next;
         fast = fast->next->next)  // not sure whether fit link list
        slow = slow->next;

    LIST_HEAD(new_list);
    struct list_head *mid = slow->next;
    list_cut_position(&new_list, head, mid);

    // step2. recursive
    q_sort(head, descend);
    q_sort(&new_list, descend);

    // merge_list(&head, &new_list, descend);
}

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
    if (!head)
        return 0;

    // step1. split from middle
    struct list_head *slow = head;
    for (struct list_head *fast = head->next; fast && fast->next;
         fast = fast->next->next)  // not sure whether fit link list
        slow = slow->next;

    LIST_HEAD(new_list);
    struct list_head *mid = slow->next;
    list_cut_position(&new_list, head, mid);

    // step2. recursive
    q_sort(head, descend);
    q_sort(&new_list, descend);

    // head = merge_list(list_entry(head, queue_contex_t, chain)->q,
    //                   list_entry(&new_list, queue_contex_t, chain)->q,
    //                   descend);

    return 0;
}
