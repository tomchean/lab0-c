#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

#define NEW_ELE(s, size, newh)                        \
    newh = malloc(size + sizeof(list_ele_t));         \
    if (!newh)                                        \
        return false;                                 \
    newh->value = (char *) newh + sizeof(list_ele_t); \
    memcpy(newh->value, s, len);

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));
    if (!q)
        return q;
    q->head = NULL;
    q->tail = NULL;
    q->num = 0;
    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    /* Free queue structure */
    if (!q)
        return;

    while (q->head) {
        list_ele_t *prev;
        prev = q->head;
        q->head = q->head->next;
        free(prev);
    }
    free(q);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(queue_t *q, char *s)
{
    if (!q)
        return false;

    list_ele_t *newh;
    size_t len = strlen(s) + 1;
    NEW_ELE(s, len, newh);

    newh->next = q->head;
    q->head = newh;
    if (!q->tail)
        q->tail = newh;
    ++q->num;
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, char *s)
{
    if (!q)
        return false;

    list_ele_t *newh;
    size_t len = strlen(s) + 1;
    NEW_ELE(s, len, newh);

    newh->next = NULL;
    if (!q->tail) {
        q->head = newh;
    } else {
        q->tail->next = newh;
    }
    q->tail = newh;
    ++q->num;
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return true if successful.
 * Return false if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 * The space used by the list element and the string should be freed.
 */
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    if (!q || !q->head)
        return false;

    // copy sp
    if (sp) {
        size_t s = strlen(q->head->value) + 1;
        if (s > bufsize) {
            memcpy(sp, q->head->value, bufsize - 1);
            memset(sp + bufsize - 1, '\0', 1);
        } else {
            memcpy(sp, q->head->value, s);
        }
    }

    list_ele_t *prev = q->head;
    q->head = q->head->next;
    free(prev);
    --q->num;
    if (!q->head)
        q->tail = NULL;
    return true;
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    return q ? q->num : 0;
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(queue_t *q)
{
    if (!q || q->num <= 1)
        return;

    list_ele_t *tmp1 = NULL;
    list_ele_t *tmp2 = q->head;
    list_ele_t *tmp3 = q->head->next;
    q->tail = q->head;
    while (tmp3) {
        tmp2->next = tmp1;
        tmp1 = tmp2;
        tmp2 = tmp3;
        tmp3 = tmp3->next;
    }
    tmp2->next = tmp1;
    q->head = tmp2;
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
list_ele_t *merge(list_ele_t *e1, list_ele_t *e2)
{
    list_ele_t *temp;
    int result = strcmp(e1->value, e2->value);
    if (result < 0) {
        temp = e1;
        e1 = e1->next;
    } else {
        temp = e2;
        e2 = e2->next;
    }
    list_ele_t *q = temp;

    while (e1 && e2) {
        result = strcmp(e1->value, e2->value);
        if (result < 0) {
            temp->next = e1;
            temp = temp->next;
            e1 = e1->next;
        } else {
            temp->next = e2;
            temp = temp->next;
            e2 = e2->next;
        }
    }

    if (e1)
        temp->next = e1;
    if (e2)
        temp->next = e2;

    return q;
}

list_ele_t *mergeList(list_ele_t *e)
{
    if (!e || !e->next)
        return e;

    list_ele_t *fast = e->next;
    list_ele_t *slow = e;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    fast = slow->next;
    slow->next = NULL;

    list_ele_t *l1 = mergeList(e);
    list_ele_t *l2 = mergeList(fast);

    return merge(l1, l2);
}

void q_sort(queue_t *q)
{
    if (!q || q->num <= 1)
        return;

    q->head = mergeList(q->head);
    q->tail = q->head;
    while (q->tail->next) {
        q->tail = q->tail->next;
    }
}
