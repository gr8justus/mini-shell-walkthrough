#include "main.h"

/**
 * main - shell's entry point
 * @argc: arguments counter (unused)
 * @argv: arguments vector
 *
 * Return: 0
 */
int main(__attribute__((unused))int argc, char *argv[])
{
	char *lineptr = NULL, **commands = NULL;
	size_t n = 0, cmd_count = 0;
	ssize_t n_read;
	pid_t child;
	int status;

	while (1) /* while the shell is running, do something */
	{
		if (isatty(STDIN_FILENO))
			printf("$ ");
		fflush(stdout);
		n_read = getline(&lineptr, &n, stdin);
		++cmd_count;

		if (n_read == -1) /* errors and Ctrl+D -> EOF */
		{
			free(lineptr);
			lineptr = NULL;
			if (isatty(STDIN_FILENO))
				printf("\n");

			/*
			 * you are supposed handle the exit code correctly -> this is not
			 * the value you should be using for all cases
			 */
			exit(0);
		}
		
		/* get rid of the newline character */
		lineptr[n_read - 1] = '\0';

		if (*lineptr == '\0')
			continue;

		commands = tokenize(lineptr, " ");
		if (commands == NULL)
			continue;

		/* check for valid executable files as is*/
		if (access(commands[0], X_OK) == -1)
		{
			/* check with the PATH and make final decisions */
			get_cmd_path(&commands[0]);

			/* well, dang it, it doesn't exist for sure */
			if (commands != NULL && access(commands[0], X_OK) == -1)
			{
				fprintf(stderr, "%s: %lu: %s: not found\n",
						argv[0], cmd_count, commands[0]);

				/* error code is 127 */
				free_cmds(commands);
				continue;
			}
		}
		child = fork();

		if (child == -1)
			continue; /* never mind, just try another command */

		if (child == 0)
		{
			if (execve(commands[0], commands, environ) == -1)
				perror("execve");
		}
		else
		{
			/* wait for the child and get the exit status code */
			waitpid(child, &status, 0);
		}

		free(lineptr);
		free_cmds(commands);
		commands = NULL;
		lineptr = NULL;
	}

	return (0);
}

/* TODO: Modularize the functionalities */

/*
 * move the parsing and command execution logic to a different file and within
 * different functions.
 * This time, keep track of the exit codes for later
 */
