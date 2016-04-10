#ifndef LIB_H_
#define LIB_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "cachenode.h"

extern int shell_error;

extern char** lexer(char *orig, char *dst, char **words);

extern struct treenode* parse(char **words, char **end);

extern void execute(struct treenode *node, int *pipe1, int *pipe2);

int in_special(char c, char *str);

void dump_tokens(char **words, char **end);

void dump_ast(struct treenode *node);

void prompt(uid_t uid);
#endif
