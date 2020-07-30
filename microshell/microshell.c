#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define SIDE_IN 1
#define SIDE_OUT 0

#define TYPE_END 0
#define TYPE_PIPE 1
#define TYPE_BREAK 2

#define STDIN 0
#define STDOUT 1
#define STDERR 2

typedef struct s_list
{
	int length;
	char **args;
	struct s_list *prev;
	struct s_list *next;
	int type;
	int			pipes[2];
}				t_list;

int	ft_strlen(const char *s)
{
	int i;

	i = 0;
	while (s[i])
		i++;
	return (i);
}

int	errr(const char *s)
{
	write(STDOUT, s, ft_strlen(s));
	return (EXIT_FAILURE);
}

char* ft_strdup(const char *s)
{
	int i;
	char *new;

	if (!s || !(new = malloc(sizeof(char) * (ft_strlen(s) + 1))))
		return (NULL);
	i = 0;
	while (s[i])
	{
		new[i] = s[i];
		i++;
	}
	new[i] = 0;
	return (new);
}

int add_arg(t_list *cmds, char *arg)
{
	char **tmp;
	int i;

	if (!(tmp = malloc(sizeof(char*) * (cmds->length + 2))))
		exit(errr("oh no\n"));
	i = 0;
	while (i < cmds->length)
	{
		tmp[i] = cmds->args[i];
		i++;
	}
	tmp[i] = ft_strdup(arg);
	i++;
	tmp[i] = 0;
	if (cmds->length > 0)
		free(cmds->args);
	cmds->args = tmp;
	cmds->length++;
	return (EXIT_SUCCESS);
}

int push(t_list **cmds, char *arg)
{
	t_list *new;

	if (!(new = (t_list*)malloc(sizeof(t_list))))
		exit(errr("oh no\n"));
	new->args = NULL;
	new->length = 0;
	new->prev = NULL;
	new->next = NULL;
	new->type = TYPE_END;
	if (*cmds)
	{
		(*cmds)->next = new;
		new->prev = *cmds;
	}
	*cmds = new;
	return (add_arg(*cmds, arg));
}

int parse_args(t_list **cmds, char *arg)
{
	int isbreak;
	int ispipe;

	isbreak = (strcmp(";", arg) == 0);
	ispipe = (strcmp("|", arg) == 0);
	if (isbreak && !*cmds) // "primer arg es un ;
		return (EXIT_SUCCESS);
	else if (!isbreak && (!*cmds || (*cmds)->type > TYPE_END))
		return (push(cmds, arg));
		//primer arg o hemos llegado a ; o |
	else if (ispipe)
		(*cmds)->type = TYPE_PIPE;
	else if (isbreak)
		(*cmds)->type = TYPE_BREAK;
	else
		add_arg(*cmds, arg);
	return (EXIT_SUCCESS);
}

void
	rewind_lst(t_list **lst)
{
	while (*lst && (*lst)->prev)
		*lst = ((*lst)->prev);
}

int exec_cmds(t_list **cmds, char **envp)
{
	t_list *crt;
	int ret;

	crt = *cmds;
	ret = EXIT_SUCCESS;
	while (crt)
	{
		if (strcmp("cd", crt->args[0]) == 0)
		{
			printf("%s: %s\n", crt->args[0], crt->args[1]);
			if (crt->length < 2)
				ret = errr("error: bad arguments\n");
			else if (chdir(crt->args[1]) != 0)
			{
				ret = errr("error: cd: cannot change directory to ");
				errr(crt->args[1]);
				errr("\n");
			}
		}
		crt = crt->next;
	}
	(void)envp;
	return (ret);
}

int	main(int argc, char **argv, char **envp)
{
	t_list *cmds;
	int i;
	int ret;

	i = 0;
	cmds = NULL;
	ret = EXIT_SUCCESS;
	while (argv[++i])
		parse_args(&cmds, argv[i]);
	if (cmds)
	{
		rewind_lst(&cmds);
		ret = exec_cmds(&cmds, envp);
	}
	(void)argc, (void)argv, (void)envp, (void)cmds;
	return (ret);
}