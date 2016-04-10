/*
 * cmdlist ::= empty
 * 			| pipeline
 * 			| pipeline '&' cmdlist
 * 			| pipeline ';' cmdlist
 * 
 * pipeline ::= command
 *			| command '|' pipeline
 *
 * command ::= '(' cmdlist ')' [ '<' in ] [ '>' out ]
 * 			| word word* [ '<' in ] [ '>' out ]
 *
 *
 */

/* 
 * 复合命令的执行：
 * 比如：(ls ; whoami)
 * 1. 首先fork一个子shell-1。
 * 2. 在shell-1中fork一个子shell-2。在shell-2中执行命令ls。
 * 3. 当发现whoami命令是复合命令的最后一个命令时，不再fork出子shell，而是
 * 	  在当前shell，即shell-1中执行。
 *
 */

#include "lib.h"


struct treenode* cmdlist(char **start, char **end);
struct treenode* pipeline(char **start, char **end);
struct treenode* command(char **start, char **end);


struct treenode* parse(char **words, char **end)
{
	char **p;

	p = words;

	while (p != end) {
		if (strcmp(*p, "\n") == 0) {
			p++;
			continue;
		}
		break;
	}

	return cmdlist(p, end);
}

struct treenode* cmdlist(char **start, char **end)
{
	char **p;
	char c;
	int nesting;
	struct treenode *node, *left;

	node = left = NULL;
	nesting = 0;

	if (start == end) {
		return NULL;
	}

	for (p = start; p != end; p++) {
		switch (**p) {
			case '(':
				nesting++;
				break;

			case ')':
				nesting--;
				if (nesting < 0) {
					goto error;
				}
				break;

			case '&':
			case ';':
			case '\n':
				if (nesting == 0) {
					c = **p;
					node = get_treenode();
					node->type = CMDLIST;
					node->left = pipeline(start, p);
					node->flags = NONE;

					if (c == '&') {
						left = node->left;
						left->flags |= AND; //

						// ls &
						if (**(p + 1) == '\n') {
							return node;
						}
					}

					// ls ;
					if (c == ';') {
						if (**(p + 1) == '\n') {
							return node;
						}
					}

					if (**p != '\n' && *(p + 1) != NULL) {
						//check 
						if (in_special(**(p + 1), ";&")) {
							goto error;
						}
					}

					node->right = cmdlist(p + 1, end);
					return node;
				}
		}
	}

	if (nesting == 0) {
		return pipeline(start, end);
	}

error:
	shell_error++;
	if (node) {
		free_treenode(node);
	}
	return NULL;
}


struct treenode* pipeline(char **start, char **end)
{
	char **p;
	int nesting;
	struct treenode *node;

	node = NULL;
	nesting = 0;

	for (p = start; p != end; p++) {
		switch (**p) {
			case '(':
				nesting++;
				break;
				
			case ')':
				nesting--;
				if (nesting < 0) {
					goto error;
				}
				break;

			case '|':
				if (nesting == 0) {
					node = get_treenode();
					node->type = FILTER;
					node->left = command(start, p);
					node->right = pipeline(p + 1, end);
					node->flags = NONE;
					
					return node;
				}
				break;
		}
	}

	if (nesting == 0) {
		return command(start, end);
	}

error:
	shell_error++;
	if (node) {
		free_treenode(node);
	}
	return NULL;
}


struct treenode* command(char **start, char **end)
{
	char **p;
	char **lp, **rp;
	char *in, *out;
	char c;
	struct treenode *node;
	int flag;
	int nesting;
	int count;

	node = NULL;
	lp = rp = NULL;
	in = out = NULL;
	flag = 0;
	nesting = 0;
	count = 0;

	if (**end == ')') {
		flag |= LAST_IN_PAR;
	}

	for (p = start; p != end; p++) {
		switch (c = **p) {
			case '(':
				if (nesting == 0) {
					lp = p + 1;
				}
				nesting++;
				break;

			case ')':
				nesting--;
				if (nesting == 0) {
					rp = p;
				}
				break;

			case '>':
				p++;
				if ((p != end) && (**p == '>')) { //>>
					flag |= CATENATE;
				} else {
					p--;
				}
			case '<':
				if (nesting == 0) {
					p++; 	//file
					if (p == end) {
						//error++;
						//p--;
						goto error;
					}
					if (in_special(**p, "<>(")) {
						goto error;
					}
					if (c == '>') {
						out = *p;
					} else { // <
						in = *p;
					}
				}
				break;

			default:
				//(ls)ls
				if (nesting == 0) {
					count++;
				}
		}
	}
	
	if (lp != NULL) {
		//(ls) ls
		if (count != 0) {
			goto error;
		}
		node = get_treenode();
		node->type = PARENTHESES;
		node->child = cmdlist(lp, rp);
	} else {
		// ;
		if (count == 0) {
			goto error;
		}
		node = get_treenode();
		node->type = COMMAND;
		node->comm = start;
		node->num = count;
	}
	node->flags = flag;
	node->in = in;
	node->out = out;

	return node;

error:
	shell_error++;
	if (node) {
		free_treenode(node);
	}
	return NULL;
}




