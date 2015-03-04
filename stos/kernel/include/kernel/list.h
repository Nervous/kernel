/*
 * File: list.h
 * Author: Xavier Deguillard <xavier@lse.epita.fr>
 *
 * Description: generic linked list management
 */

#ifndef LIST_H_
# define LIST_H_

# include <kernel/stos.h>

struct list_node {
	struct list_node* next;
	struct list_node* prev;
};

# define LIST_INIT(head) { &(head), &(head) }

static inline void list_init(struct list_node* head)
{
	head->next = head;
	head->prev = head;
}

static inline void list_insert(struct list_node* head, struct list_node* elm)
{
	assert(head);
	assert(elm);
	/* assert if we try to insert an already inserted node */
	assert(elm->next == elm->prev);
	elm->next = head->next;
	elm->prev = head;
	head->next->prev = elm;
	head->next = elm;
}

static inline void list_remove(struct list_node* elm)
{
	assert(elm);
	assert(elm->prev);
	assert(elm->next);
	elm->prev->next = elm->next;
	elm->next->prev = elm->prev;
	elm->next = JUNK_PTR;
	elm->prev = JUNK_PTR;
}

static inline int list_is_empty(struct list_node* elm)
{
	return elm->next == elm;
}

# define lentry(elm, type, field)	\
	container_of(elm, type, field)

/*
 * Iterate over all the list_node.
 */
# define lfor_each(head, elm)		\
	for (elm = (head)->next; elm != (head); elm = elm->next)

/*
 * Iterate over all the list_node in the reverse order.
 */
# define lfor_each_reverse(head, elm)		\
	for (elm = (head)->prev; elm != (head); elm = elm->prev)

/*
 * Iterate over all the list element, by following the next link.
 */
# define lfor_each_entry(head, elm, field)				\
	for (elm = lentry((head)->next, __typeof__(*elm), field);	\
	     &elm->field != (head);					\
	     elm = lentry((elm)->field.next, __typeof__(*elm), field))

# define lfor_each_entry_safe(head, elm, field)				\
	__typeof__(*elm)* __next;					\
	for (elm = lentry((head)->next, __typeof__(*elm), field),	\
		__next = lentry((elm)->field.next, __typeof__(*elm), field);	\
	     &elm->field != (head);					\
	     elm = __next,						\
		__next = lentry((__next)->field.next, __typeof(*elm), field))

/*
 * Iterate over all the list element, by following the prev link.
 */
# define lfor_each_entry_reverse(head, elm, field)			\
	for (elm = lentry((head)->prev, __typeof__(*elm), field);	\
	     &elm->field != (head);					\
	     elm = lentry((elm)->field.prev, __typeof__(*elm), field))

#endif /* !LIST_H_ */
