/**
 * 1. Show the prompt
 * 2. take input from the user
 * 3. print the input to stdout
 * 4. Show the prompt again
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

void free_cmds(char **cmds);
char **tokenize(char *str, const char *delim);
void get_cmd_path(char **pathname);
extern char **environ; 

int main(int argc, char *argv[])
{
	char *lineptr = NULL, **commands = NULL;
	size_t n = 0, cmd_count = 0;
	ssize_t n_read;
	pid_t child;
	int status;

	(void)argc;
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
				fprintf(stderr, "%s: %lu: %s: not found\n", argv[0], cmd_count, commands[0]);
	
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
			waitpid(child, &status, 0);
		}

		free(lineptr);
		free_cmds(commands);
		commands = NULL;
		lineptr = NULL;
	}

	return (0);
}


void get_cmd_path(char **pathname)
{
	char *token = NULL, *path = NULL, *path_dir, *dup_str;
	if (*pathname == NULL || pathname == NULL)
		return;
	
	path_dir = getenv("PATH");
	if (path_dir == NULL)
		return;

	dup_str = strdup(path_dir);
	token = strtok(dup_str, ":");
	
	while (token != NULL)
	{
		path = malloc(sizeof(char) * (strlen(*pathname) + strlen(token) + 2));
		if (path == NULL)
		{
			fprintf(stderr, "Memory alloaction failed\n");
			return;
		}
		sprintf(path, "%s/%s", token, *pathname);
		
		if (access(path, X_OK) == 0)
		{
			free(*pathname);
			*pathname = strdup(path);
			free(path);
			break;
		}
		
		free(path);
		token = strtok(NULL, ":");
	}
	free(dup_str);

}
void free_cmds(char **cmds)
{
	size_t i;

	for (i = 0; cmds[i] != NULL; i++)
	{
		free(cmds[i]);
		cmds[i] = NULL;
	}
	free(cmds);
}
char **tokenize(char *str, const char *delim)
{
	char **commands = NULL, *token, *dup_str;
	size_t n_tokens, i;

	if (str == NULL || *str == '\0')
		return (NULL);

	/* tokenize the input string */
	dup_str = strdup(str);
	token = strtok(dup_str, delim);

	n_tokens = 0;
	while (token != NULL)
	{
		++n_tokens; /* count all the tokens */
		token = strtok(NULL, delim);
	}
	free(dup_str);

	if (n_tokens > 0)
	{
		/* allocate enough memory based on numbers tokens */
		commands = malloc(sizeof(char *) * (n_tokens + 1));
		if (commands == NULL)
			exit(EXIT_FAILURE);

		token = strtok(str, delim);
		i = 0;
		while (token != NULL)
		{
			commands[i] = strdup(token);
			token = strtok(NULL, delim);
			i++;
		}
		commands[i] = NULL;
	}

	return (commands);
}
