#include "lib.h"


int redirect(struct treenode *node);
int execute_cmd(struct treenode *node);
int pwait(pid_t pid);
int builtin(struct treenode *node);

void execute(struct treenode *node, int *pipe1, int *pipe2)
{
	int flag, ret;
	pid_t pid;
	int pv[2];
	struct treenode *tmp;

	if (node == NULL) {
		return;
	}

	switch (node->type) {
		case CMDLIST:
			flag = node->flags;
			execute(node->left, NULL, NULL);
			execute(node->right, NULL, NULL);
			free_treenode(node);
			return; 	//why return? Because the rest will be executed by child.

		case FILTER:
			flag = node->flags;
			pipe(pv);

			tmp = node->left;
			tmp->flags |= PIPE_OUT | (flag & (PIPE_IN | LAST_IN_PAR));
			execute(node->left, pipe1, pv);

			tmp = node->right;
			tmp->flags |= PIPE_IN | (flag & (PIPE_OUT | AND | LAST_IN_PAR));
			execute(node->right, pv, pipe2);

			free_treenode(node);
			return;

		case COMMAND:
			if (builtin(node) == 0) {
				free_treenode(node);
				return;
			}

		case PARENTHESES:
			pid = 0;
			flag = node->flags;

			//if the LAST_IN_PAR, not fork, use the current 
			if ((flag & LAST_IN_PAR) == 0) {
				pid = fork();
				if (pid == -1) {
					perror("fail to fork");
					return;
				}
			}

			// parent
			if (pid != 0) {
				if ((flag & PIPE_IN) != 0) {
					close(pipe1[0]);
					close(pipe1[1]);
				}

				if ((flag & AND) != 0) {
					printf("%d\n", pid);
					free_treenode(node);
					return;
				}

				pwait(pid);
				free_treenode(node);
				return;
			}

			//child
			if (node->in || node->out) {
				if (redirect(node) != 0) {
					exit(-1);
				}
			}

			if ((flag & PIPE_IN) != 0) {
				close(STDIN_FILENO);
				dup(pipe1[0]);
				close(pipe1[0]);
				close(pipe1[1]);
			}

			if ((flag & PIPE_OUT) != 0) {
				close(STDOUT_FILENO);
				dup(pipe2[1]);
				close(pipe2[1]);
				close(pipe2[0]);
			}

			if (node->type == PARENTHESES) {
				tmp = node->child;
				execute(tmp, NULL, NULL);
				exit(1);
			}

			ret = execute_cmd(node);
			exit(ret);
	}
}


int redirect(struct treenode *node)
{
	int fd;

	if (node->in) {
		close(STDIN_FILENO);
		fd = open(node->in, O_RDONLY);
		if (fd < 0) {
			perror("fail to open!");
			return 1;
		}
	}

	if (node->out) {
		if (node->flags & CATENATE) {	// >>
			fd = open(node->out, O_WRONLY);
			if (fd < 0) {
				perror("fail to write!");
				return 1;
			}
			lseek(fd, 0, SEEK_END);
		} else { 	// >
			fd = creat(node->out, 0666);
			if (fd < 0) {
				perror("fail to create!");
				return 1;
			}
		}

		close(STDOUT_FILENO);
		dup(fd);
		close(fd);
	}

	return 0;
}


int execute_cmd(struct treenode *node)
{
	int i, num;
	char **args;
	char *cmd[16];
	extern int errno;

	num = node->num;
	args = node->comm;

	for (i = 0; i < num; i++) {
		cmd[i] = args[i];
	}
	cmd[i] = NULL;

	execvp(cmd[0], cmd);
	
	if (errno == ENOENT) {
		printf("%s", cmd[0]);
		perror(": not found!");
		return 127;
	}

	return 126;
}


int pwait(pid_t pid)
{
	pid_t p;
	int status;

	p = wait(&status);
	if (p == -1) {
		perror("wait failed!");
		exit(-1);
	}

	return status;
}

int builtin(struct treenode *node)
{
	char *cmd;

	cmd = node->comm[0];

	if (strcmp(cmd, "exit") == 0) {
		exit(1);
	}
	return 1;
}
