#include "lib.h"


int in_special(char c, char *str)
{
	char *p;

	p = str;

	while (*p) {
	if (*p++ == c)
		return 1;
	}
	return 0;
}


void dump_tokens(char **words, char **end)
{
	int idx;

	idx = 0;

	while (&words[idx] != end) {
		if (strcmp(words[idx], "\n") == 0) {
			printf("new line\n");
		} else {
			printf("%s\n", words[idx]);
		}
		idx++;
	}

}


void dump_ast(struct treenode *node)
{
	if (node == NULL) {
		return;
	}

	switch (node->type) {
		case COMMAND: {
			int i = 0;
			printf("begin-COMMAND\n");
			for (i = 0; i < node->num; i++) {
				printf("%s ", node->comm[i]);
			}
			printf("\nend-COMMAND\n");
			break;
		}
			
		case CMDLIST: {
			printf("begin-CMDLIST\n");
			dump_ast(node->left);
			dump_ast(node->right);
			printf("end-CMDLIST\n");
			break;
		}

		case PARENTHESES: {
			printf("begin-PARENTHESES\n");
			dump_ast(node->child);
			printf("end-PARENTHESES\n");
			break;
		}

		case FILTER: {
			printf("begin-FILTER\n");
			dump_ast(node->left);
			dump_ast(node->right);
			printf("end-FILTER\n");
			break;
		}
	}
}

void prompt(uid_t uid)
{
	if (uid == 0) {
		printf("#");
	} else {
		printf("$");
	}
	printf(" ");

}
