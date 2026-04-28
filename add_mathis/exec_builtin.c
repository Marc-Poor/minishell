/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtin.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfaure <mfaure@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/26 22:12:57 by mfaure            #+#    #+#             */
/*   Updated: 2026/04/28 20:21:45 by mfaure           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	**exec_parent_built_in(t_cmd *curr, char **env)
{
	if (ft_strcmp(curr->argv[0], "cd") == 0)
		return (ft_cd(curr->argv, env));
	if (ft_strcmp(curr->argv[0], "export") == 0)
		return (ft_export_main(curr->argv, env));
	if (ft_strcmp(curr->argv[0], "unset") == 0)
		return (ft_unset(curr->argv, env));
	if (ft_strcmp(curr->argv[0], "exit") == 0)
		ft_exit(curr->argv);
	return (env);
}

int	exec_child_builtin(t_cmd *curr, char **env)
{
	if (!curr->argv[0])
		return (0);
	if (ft_strcmp(curr->argv[0], "echo") == 0)
		return (main_ft_echo(curr->argv));
	if (ft_strcmp(curr->argv[0], "pwd") == 0)
		return (ft_pwd(env));
	if (ft_strcmp(curr->argv[0], "env") == 0)
		return (ft_env(env));
	return (-1);
}

int	handle_parent_builtin(t_cmd *cmd, t_shell *shell)
{
	char	**tmp;

	if (!cmd->argv || !cmd->argv[0])
		return (0);
	if (ft_strcmp(cmd->argv[0], "cd") == 0
		|| ft_strcmp(cmd->argv[0], "export") == 0
		|| ft_strcmp(cmd->argv[0], "unset") == 0
		|| ft_strcmp(cmd->argv[0], "exit") == 0)
	{
		tmp = exec_parent_built_in(cmd, shell->env);
		if (!tmp)
			return (1);
		shell->env = tmp;
		return (2);
	}
	return (0);
}
