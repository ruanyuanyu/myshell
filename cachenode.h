#ifndef CACHENODE_H_
#define CACHENODE_H_


enum {
	COMMAND = 1,
	CMDLIST,
	PARENTHESES,
	FILTER
};

enum {
	NONE,
	AND = 0x01,
	CATENATE = 0x02,
	PIPE_IN = 0x04,
	PIPE_OUT = 0x08,
	LAST_IN_PAR = 0x10
};

struct treenode {
	int type;
	int flags;
	struct treenode *left, *right;
	char **comm; 	//for command
	int num; 	//args num
	struct treenode *child; 	//parenthesis
	char *in, *out; //< > >>
	struct treenode *next; 		//for cache
};

struct cache {
	struct treenode *head;
};


struct treenode *get_treenode(void);
void free_treenode(struct treenode *node);

#endif
