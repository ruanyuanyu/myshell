#include "lib.h"

#define BUFSIZE 1024

int shell_error;

int main(int argc, char *argv[])
{
	uid_t uid;
	char orig[BUFSIZE];
	char dst[BUFSIZE * 2];
	char *words[BUFSIZE / 2];
	char **end;
	struct treenode *tree;

	tree = NULL;
	uid = getuid();

	while (1) {
		shell_error = 0;
		prompt(uid);
		fgets(orig, BUFSIZE, stdin);
		end = lexer(orig, dst, words);
		if (shell_error) {
			printf("lex error!\n");
			continue;
		}
		//dump_tokens(words, end);
		tree = parse(words, end);
		if (shell_error) {
			printf("parse error!\n");
			continue;
		}
		//dump_ast(tree);

		execute(tree, NULL, NULL);

	}

	return 0;
}

