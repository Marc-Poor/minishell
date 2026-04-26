/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mathisseguin <mathisseguin@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 13:27:17 by mseguin           #+#    #+#             */
/*   Updated: 2026/04/26 22:06:07 by mathissegui      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>

volatile sig_atomic_t	g_sig = 0;

static void	handle_sigint(int sig)
{
	(void)sig;
	g_sig = SIGINT;
	printf("\n");
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

void	setup_signals(void)
{
	signal(SIGINT, handle_sigint);
	signal(SIGQUIT, SIG_IGN);
}

static int	is_only_spaces(char *s)
{
	int	i;

	i = 0;
	while (s[i])
	{
		if (s[i] != ' ' && s[i] != '\t')
			return (0);
		i++;
	}
	return (1);
}

static void	process_line(char *line, t_shell *shell)
{
	t_token	*toks;
	t_cmd	*cmds;

	toks = tokenize_line(line, shell);
	if (!toks)
	{
		printf("Erreur de syntaxe : quotes non fermées\n");
		return ;
	}
	if (check_syntax(toks))
	{
		clear_token(&toks);
		return ;
	}
	//print_tokens(toks);
	cmds = parse_commands(toks);
	if (!cmds)
	{
		clear_token(&toks);
		return ;
	}
	//print_cmds(cmds);
	execute(cmds, shell);
	free_cmds(&cmds);
	clear_token(&toks);
}

int	main(int ac, char **av, char **env)
{
	char	*line;
	t_shell *shell;

	shell = malloc(sizeof(t_shell));
	shell->env = copy_tab(env);
	if (ac == 0 || !av[0])
		return 0;
	setup_signals();
	while (1)
	{
		g_sig = 0;
		line = readline("minishell$ ");
		if (!line)
		{
				printf("exit\n");
			break ;
		}
		if (line[0] != '\0' && !is_only_spaces(line))
			add_history(line);
		if (line[0] != '\0' && !is_only_spaces(line))
			process_line(line, shell);
		free(line);
	}
	clear_history();
	return (0);
}