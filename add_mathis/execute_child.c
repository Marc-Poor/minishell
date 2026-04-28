/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_child.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mfaure <mfaure@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/27 00:49:56 by mfaure            #+#    #+#             */
/*   Updated: 2026/04/27 00:52:22 by mfaure           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "pipex/pipex.h"
#include <readline/history.h>
#include <readline/readline.h>

static void	close_fds_child(int prev_fd, t_pipex *p, t_cmd *cmd)
{
	if (prev_fd != -1)
		close(prev_fd);
	if (cmd->next)
	{
		close(p->pipefd[0]);
		close(p->pipefd[1]);
	}
	if (p->infile != -1)
		close(p->infile);
	if (p->outfile != -1)
		close(p->outfile);
}

void	exec_child(t_cmd *curr, t_shell *shell, t_pipex *p, int prev_fd)
{
	int	ret;

	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	if (prev_fd != -1)
		dup2(prev_fd, STDIN_FILENO);
	if (curr->redirs && (curr->redirs->type == T_REDIR_IN
			|| curr->redirs->type == T_HEREDOC))
		dup2(p->infile, STDIN_FILENO);
	if (curr->next)
		dup2(p->pipefd[1], STDOUT_FILENO);
	else if (p->outfile != -1)
		dup2(p->outfile, STDOUT_FILENO);
	close_fds_child(prev_fd, p, curr);
	ret = exec_child_builtin(curr, shell->env);
	if (ret != -1)
		exit(ret);
	exec_cmd(curr->argv, shell->env);
	exit(1);
}
