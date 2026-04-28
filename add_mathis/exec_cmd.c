/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_cmd.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfaure <mfaure@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/26 23:11:18 by mfaure            #+#    #+#             */
/*   Updated: 2026/04/27 00:08:14 by mfaure           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "pipex/pipex.h"
#include <readline/history.h>
#include <readline/readline.h>

void	free_command(char **tab)
{
	int	i;

	i = 0;
	if (!tab)
		return ;
	while (tab[i])
	{
		free(tab[i]);
		i++;
	}
	free(tab);
}

int	invalid_command(char **cmd)
{
	if (!cmd)
	{
		perror("Invalid command: cmd_str");
		return (0);
	}
	if (!cmd[0])
	{
		free(cmd);
		return (0);
	}
	return (1);
}

void	exec_cmd(char **cmd, char **env)
{
	char	*path;

	if (!invalid_command(cmd))
		exit(127);
	if (strchr(cmd[0], '/'))
	{
		execve(cmd[0], cmd, env);
		perror(cmd[0]);
		exit(127);
	}
	path = find_path(env, cmd[0], 0);
	if (!path || path[0] == '\0')
	{
		perror(cmd[0]);
		free_command(cmd);
		exit(127);
	}
	execve(path, cmd, env);
	perror("execve failed");
	exit(1);
}
