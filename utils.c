#include "main.h"

void get_cmd_path(char **command)
{
	char *dir = NULL, *path = NULL, *path_dir, *dup_str;

	if (*command == NULL || command == NULL)
		return; /* no command was received */

	path_dir = getenv("PATH");
	if (path_dir == NULL)
		return;

	dup_str = strdup(path_dir);
	dir = strtok(dup_str, ":");

	while (dir != NULL)
	{
		/* let's get the right size and allocate memory for it */
		path = malloc(sizeof(char) * (strlen(*command) + strlen(dir) + 2));
		if (path == NULL)
		{
			fprintf(stderr, "Memory alloaction failed\n");
			return;
		}
		/* build up the full command path and check for execute permissions */
		sprintf(path, "%s/%s", dir, *command);

		/*
		 * Uncomment the line below to get debug messages on which path is
		 * being check at any given moment
		 */

		/* printf("Debug [get_cmd_path]: Checking [%s]\n", path); */

		if (access(path, X_OK) == 0)
		{
			free(*command);
			*command = strdup(path);
			free(path);
			break;
		}

		free(path);
		dir = strtok(NULL, ":"); /* keep searching, we've not found it yet */
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
	size_t num_of_tokens, i;

	if (str == NULL || *str == '\0')
		return (NULL);

	/* tokenize the input string */
	dup_str = strdup(str);
	token = strtok(dup_str, delim);

	num_of_tokens = 0;
	while (token != NULL)
	{
		++num_of_tokens; /* count all the tokens */
		token = strtok(NULL, delim);
	}
	free(dup_str);

	if (num_of_tokens > 0)
	{
		/* allocate enough memory based on the number of tokens */
		commands = malloc(sizeof(char *) * (num_of_tokens + 1));
		if (commands == NULL)
			exit(EXIT_FAILURE);

		token = strtok(str, delim);
		i = 0;
		while (token != NULL)
		{
			commands[i++] = strdup(token);
			token = strtok(NULL, delim);
		}
		commands[i] = NULL;
	}

	return (commands);
}
