#include <stdlib.h>
#include "cachenode.h"

static struct cache cache;

struct treenode *get_treenode(void)
{
	struct treenode *p;
	if (cache.head == NULL) {
		p = (struct treenode *)malloc(sizeof(struct treenode));
		p->next = NULL;
		return p;
	}

	p = cache.head;
	cache.head = p->next;
	return p;
}


void free_treenode(struct treenode *node)
{
	node->next = cache.head;
	cache.head = node;

}


